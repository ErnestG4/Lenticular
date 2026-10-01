// Copyright 2026 Combust.
// SPDX-License-Identifier: MIT
//
// Lean pitch tracker for pitch-synchronous grains: a 512-bit ring of input
// signs at 8x decimation (64 bytes of state), autocorrelated by XOR at
// update time. Covers 50..500 Hz — bass and voice, where grain/cycle
// interference hurts most. Reports 0 when unvoiced; holds its estimate
// while FROZEN (a frozen buffer keeps its pitch even in silence).

#ifndef CLOUDS_DSP_ARS_PITCH_H_
#define CLOUDS_DSP_ARS_PITCH_H_

#include "stmlib/stmlib.h"

namespace clouds {

class ArsPitchTracker {
 public:
  void Init() {
    for (int i = 0; i < kWords; ++i) bits_[i] = 0;
    write_ = 0;
    accumulator_ = 0.0f;
    decimate_ = 0;
    lp_ = 0.0f;
    since_update_ = 0;
    period_ = 0.0f;
    raw_hist_[0] = raw_hist_[1] = raw_hist_[2] = 0.0f;
    raw_idx_ = 0;
    previous_bit_ = 0;
    samples_since_epoch_ = 0.0f;
  }

  // One input sample at the module rate; hold = don't update (FREEZE).
  inline void Push(float sample, bool hold) {
    if (hold) return;
    samples_since_epoch_ += 1.0f;
    // ~1 kHz one-pole strips formants/harmonics before the sign is taken,
    // so zero crossings track the fundamental.
    lp_ += 0.18f * (sample - lp_);
    accumulator_ += lp_;
    if (++decimate_ < 8) return;
    decimate_ = 0;
    const int bit = accumulator_ > 0.0f ? 1 : 0;
    if (bit && !previous_bit_) {
      samples_since_epoch_ = 0.0f;  // pitch mark (+/-8 samples of jitter)
    }
    previous_bit_ = bit;
    accumulator_ = 0.0f;
    const int w = write_ >> 5;
    const uint32_t mask = 1u << (write_ & 31);
    bits_[w] = (bits_[w] & ~mask) | (bit ? mask : 0u);
    write_ = (write_ + 1) & (kBits - 1);
    if (++since_update_ >= 128) {  // every 1024 input samples
      since_update_ = 0;
      Update();
    }
  }

  // Fundamental period in module-rate samples; 0 = unvoiced.
  float period() const { return period_; }

  // Age of the most recent pitch mark, in module-rate samples. Together
  // with the write head this locates epoch phase in the ring buffer.
  float epoch_age() const { return samples_since_epoch_; }

 private:
  static const int kBits = 512;
  static const int kWords = kBits / 32;

  inline int Popcount(uint32_t x) const {
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    return static_cast<int>((((x + (x >> 4)) & 0x0F0F0F0Fu) * 0x01010101u)
                            >> 24);
  }

  // Disagreement count between the ring and itself shifted by lag,
  // evaluated over the most recent (kBits - lag) bits.
  int Disagreement(int lag) const {
    int count = 0;
    const int span = kBits - lag;
    for (int i = 0; i < span; i += 32) {
      const uint32_t a = Extract32((write_ - span + i) & (kBits - 1));
      const uint32_t b = Extract32((write_ - span - lag + i) & (kBits - 1));
      uint32_t d = a ^ b;
      const int remaining = span - i;
      if (remaining < 32) d &= (1u << remaining) - 1u;
      count += Popcount(d);
    }
    return count;
  }

  inline uint32_t Extract32(int start) const {
    const int w = start >> 5;
    const int s = start & 31;
    uint32_t v = bits_[w] >> s;
    if (s) v |= bits_[(w + 1) & (kWords - 1)] << (32 - s);
    return v;
  }

  void Update() {
    // Lags 8..80 at 4 kHz = 500..50 Hz fundamentals.
    int best_lag = 0;
    float best_score = 0.30f;  // voicing threshold
    for (int lag = 8; lag <= 80; ++lag) {
      const int span = kBits - lag;
      const float agreement =
          1.0f - static_cast<float>(Disagreement(lag)) /
                     static_cast<float>(span);
      // Favor longer lags slightly: octave errors always agree at half
      // the true period, so demand a margin to prefer the fundamental.
      const float score = (agreement - 0.5f) * 2.0f;
      if (score > best_score * 1.05f) {
        best_score = score;
        best_lag = lag;
      }
    }
    // Octave guard: when a sub-multiple of the winning lag agrees
    // nearly as well, the long lag is a sub-octave alias (formant-heavy
    // voices phase-align at 2T) — prefer the fundamental.
    if (best_lag >= 16) {
      for (int div = 3; div >= 2; --div) {
        const int sub = best_lag / div;
        if (sub < 8) continue;
        int pick = 0;
        float pick_score = 0.0f;
        for (int cand = sub; cand <= sub + 1; ++cand) {
          const int span = kBits - cand;
          const float agreement =
              1.0f - static_cast<float>(Disagreement(cand)) /
                         static_cast<float>(span);
          const float score = (agreement - 0.5f) * 2.0f;
          if (score > pick_score) { pick_score = score; pick = cand; }
        }
        if (pick_score >= 0.78f * best_score) {
          best_lag = pick;
          best_score = pick_score;
        }
      }
    }
    // Median-of-3 across updates: measured, every octave-alias flip on
    // formant-heavy voices lasts exactly one update — none survive.
    raw_hist_[raw_idx_] = best_lag == 0
        ? 0.0f : static_cast<float>(best_lag) * 8.0f;
    raw_idx_ = (raw_idx_ + 1) % 3;
    const float a = raw_hist_[0];
    const float b = raw_hist_[1];
    const float c = raw_hist_[2];
    const float raw = a < b
        ? (b < c ? b : (a < c ? c : a))
        : (a < c ? a : (b < c ? c : b));
    if (raw <= 0.0f) {
      period_ = 0.0f;
      return;
    }
    if (period_ > 0.0f &&
        raw > period_ * 0.8f && raw < period_ * 1.25f) {
      period_ += 0.3f * (raw - period_);  // smooth small corrections
    } else {
      period_ = raw;  // jump on note changes
    }
  }

  uint32_t bits_[kWords];
  int write_;
  float accumulator_;
  int decimate_;
  float lp_;
  int since_update_;
  float period_;
  float raw_hist_[3];
  int raw_idx_;
  int previous_bit_;
  float samples_since_epoch_;
};

}  // namespace clouds

#endif  // CLOUDS_DSP_ARS_PITCH_H_
