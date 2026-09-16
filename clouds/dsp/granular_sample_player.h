// Copyright 2014 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Granular playback of audio stored in a buffer.

#ifndef CLOUDS_DSP_GRANULAR_SAMPLE_PLAYER_H_
#define CLOUDS_DSP_GRANULAR_SAMPLE_PLAYER_H_

#include "stmlib/stmlib.h"

#include <algorithm>

#include "stmlib/dsp/atan.h"
#include "stmlib/dsp/units.h"
#include "stmlib/utils/random.h"

#include "clouds/dsp/ars_chords.h"
#include "clouds/dsp/ars_scheduler.h"
#include "clouds/dsp/audio_buffer.h"
#include "clouds/dsp/frame.h"
#include "clouds/dsp/grain.h"
#include "clouds/dsp/parameters.h"

#include "clouds/resources.h"

namespace clouds {

const int32_t kMaxNumGrains = 64;

using namespace stmlib;

class GranularSamplePlayer {
 public:
  GranularSamplePlayer() { }
  ~GranularSamplePlayer() { }
  
  void Init(int32_t num_channels, int32_t max_num_grains) {
    max_num_grains_ = max_num_grains;
    num_midfi_grains_ = 3 * max_num_grains / 4;
    gain_normalization_ = 1.0f;
    for (int32_t i = 0; i < kMaxNumGrains; ++i) {
      grains_[i].Init();
    }
    num_grains_ = 0.0f;
    num_channels_ = num_channels;
    grain_size_hint_ = 1024.0f;
    ars_gaps_.Init(0xC10D5EEDu);
    ars_countdown_ = 1.0f;
    ars_transpose_ = 0.0f;
#ifndef ARS_LEAN
    ars_bar_ = 0;
    ars_step_ = 0;
    ars_step_countdown_ = 1.0f;
#endif
  }
  
  template<Resolution resolution>
  void Play(
      const AudioBuffer<resolution>* buffer,
      const Parameters& parameters,
      float* out, size_t size) {
    float overlap = parameters.granular.overlap;
    overlap = overlap * overlap * overlap;
    float target_num_grains = max_num_grains_ * overlap;
    float p = target_num_grains / static_cast<float>(grain_size_hint_);
    float space_between_grains = grain_size_hint_ / target_num_grains;
    // ARS ladder scheduling replaces both stock decision rules: gaps come
    // from the selected spacing statistics, scaled by the same mean spacing
    // the density knob implies — the ladder changes character, not density.
    const bool use_ars = parameters.ars_zone >= 0;
    if (use_ars) {
      p = -1.0f;
      grain_rate_phasor_ = -1000.0f;
      ars_gaps_.set_loop(parameters.ars_loop);
      if (parameters.trigger) {
        // Replay the stored realization: the same cloud every strike.
        ars_gaps_.Replay();
        ars_countdown_ = 1.0f;
#ifndef ARS_LEAN
        ars_step_ = 0;
        ars_step_countdown_ = 1.0f;
        ars_bar_ = 0;
#endif
      }
    } else if (parameters.granular.use_deterministic_seed) {
      p = -1.0f;
    } else {
      grain_rate_phasor_ = -1000.0f;
    }
    
    // Build a list of available grains.
    int32_t num_available_grains = FillAvailableGrainsList();
    
    // Try to schedule new grains.
    bool seed_trigger = parameters.trigger;
    for (size_t t = 0; t < size; ++t) {
      grain_rate_phasor_ += 1.0f;
      bool seed_probabilistic = Random::GetFloat() < p
          && target_num_grains > num_grains_;
      bool seed_deterministic = grain_rate_phasor_ >= space_between_grains;
      bool seed_ars = false;
#ifndef ARS_LEAN
      if (use_ars && parameters.ars_harmony == 2) {
        // Riff mode: a Steps source. Grains fire on a swung 8-step grid,
        // playing the selected riff transposed by the twelve-bar roots —
        // the zone statistics stand aside; this is a groove, not a process.
        ars_step_countdown_ -= 1.0f;
        if (ars_step_countdown_ <= 0.0f) {
          const float step = space_between_grains;
          ars_step_countdown_ += (ars_step_ & 1)
              ? 2.0f * step * (1.0f - ars::kRiffSwing)
              : 2.0f * step * ars::kRiffSwing;
          const int riff = static_cast<int>(
              parameters.ars_chord * (ars::kNumRiffs - 1) + 0.5f);
          const int16_t degree =
              ars::kRiffs[riff][ars_step_ % ars::kRiffSteps];
          ++ars_step_;
          if (ars_step_ % ars::kRiffSteps == 0) {
            ars_bar_ = static_cast<int16_t>((ars_bar_ + 1) % ars::kNumProgressionBars);
          }
          if (degree >= 0) {
            ars_transpose_ = static_cast<float>(
                ars::kTwelveBarRoots[ars_bar_] + degree) * 0.01f;
            seed_ars = true;
          }
        }
      } else if (use_ars) {
#else
      if (use_ars) {
#endif  // ARS_LEAN
        ars_countdown_ -= 1.0f;
        if (ars_countdown_ <= 0.0f) {
          // Advance the process whether or not a grain slot is free: a
          // missed event thins the realization, it must not warp it.
          const float gap = ars_gaps_.NextGap(
              parameters.ars_zone, parameters.ars_character,
              space_between_grains);
          ars_countdown_ += gap;
          seed_ars = true;

          // Chord clouds: the gap statistics choose the voice (short gaps
          // low, long gaps high — the gap melody, lifted to grains); the
          // cents come from the table. Progressions advance one bar per
          // gap-sum wrap, so bars breathe with the necklace.
          if (parameters.ars_harmony > 0) {
            const float gap_units = gap / space_between_grains;
            int voice = static_cast<int>(ars_gaps_.last_voice_u() * 3.999f);
            CONSTRAIN(voice, 0, 3);
            const bool metallic = parameters.ars_harmony == 1;
            const int n = metallic ? ars::kNumMetallicChords
                                   : ars::kNumHarmonicChords;
            float x = parameters.ars_chord * static_cast<float>(n - 1);
            int chord = static_cast<int>(x + 0.5f);
            CONSTRAIN(chord, 0, n - 1);
            const int16_t cents = metallic
                ? ars::kMetallicChords[chord][voice]
                : ars::kHarmonicChords[chord][voice];
            ars_transpose_ = static_cast<float>(cents) * 0.01f;
            (void) gap_units;
          } else {
            ars_transpose_ = 0.0f;
          }
        }
      } else {
        ars_transpose_ = 0.0f;
      }
      bool seed = seed_probabilistic || seed_deterministic || seed_ars ||
          seed_trigger;
      if (num_available_grains && seed) {
        --num_available_grains;
        int32_t index = available_grains_[num_available_grains];
        GrainQuality quality;
        if (num_available_grains < num_midfi_grains_) {
          quality = GRAIN_QUALITY_MEDIUM;
        } else {
          quality = GRAIN_QUALITY_HIGH;
        }

        // Reese twins: every grain is secretly two, detuned half the spread
        // each way around its chord tone. Long overlaps turn the pairs into
        // the classic beating growl; costs one extra grain slot per event.
        const bool twins = use_ars && parameters.ars_detune > 0.5f;
        const float base_transpose = ars_transpose_;
        if (twins) {
          ars_transpose_ = base_transpose + parameters.ars_detune * 0.005f;
        }
        Grain* g = &grains_[index];
        ScheduleGrain(
            g,
            parameters,
            t,
            buffer->size(),
            buffer->head() - size + t,
            quality);
        if (twins && num_available_grains) {
          --num_available_grains;
          const int32_t twin_index = available_grains_[num_available_grains];
          const GrainQuality twin_quality =
              num_available_grains < num_midfi_grains_
                  ? GRAIN_QUALITY_MEDIUM
                  : GRAIN_QUALITY_HIGH;
          ars_transpose_ = base_transpose - parameters.ars_detune * 0.005f;
          ScheduleGrain(
              &grains_[twin_index],
              parameters,
              t,
              buffer->size(),
              buffer->head() - size + t,
              twin_quality);
        }
        ars_transpose_ = base_transpose;
        grain_rate_phasor_ = 0.0f;
        seed_trigger = false;
      }
    }
    
    // Overlap grains.
    std::fill(&out[0], &out[size * 2], 0.0f);
    float* e = envelope_buffer_;
    for (int32_t i = 0; i < max_num_grains_; ++i) {
      Grain* g = &grains_[i];
      if (g->recommended_quality() == GRAIN_QUALITY_HIGH) {
        if (num_channels_ == 1) {
          g->OverlapAdd<1, GRAIN_QUALITY_HIGH>(buffer, out, e, size);
        } else {
          g->OverlapAdd<2, GRAIN_QUALITY_HIGH>(buffer, out, e, size);
        }
      } else if (g->recommended_quality() == GRAIN_QUALITY_MEDIUM) {
        if (num_channels_ == 1) {
          g->OverlapAdd<1, GRAIN_QUALITY_MEDIUM>(buffer, out, e, size);
        } else {
          g->OverlapAdd<2, GRAIN_QUALITY_MEDIUM>(buffer, out, e, size);
        }
      } else {
        if (num_channels_ == 1) {
          g->OverlapAdd<1, GRAIN_QUALITY_LOW>(buffer, out, e, size);
        } else {
          g->OverlapAdd<2, GRAIN_QUALITY_LOW>(buffer, out, e, size);
        }
      }
    }
    
    // Compute normalization factor.
    int32_t active_grains = max_num_grains_ - num_available_grains;
    SLOPE(num_grains_, static_cast<float>(active_grains), 0.9f, 0.2f);

    float gain_normalization = num_grains_ > 2.0f
        ? fast_rsqrt_carmack(num_grains_ - 1.0f)
        : 1.0f;  
    float window_gain = 1.0f + 2.0f * parameters.granular.window_shape;
    CONSTRAIN(window_gain, 1.0f, 2.0f);
    gain_normalization *= Crossfade(
        1.0f, window_gain, parameters.granular.overlap);

    // Apply gain normalization.
    for (size_t t = 0; t < size; ++t) {
      ONE_POLE(gain_normalization_, gain_normalization, 0.01f)
      *out++ *= gain_normalization_;
      *out++ *= gain_normalization_;
    }
  }
  
 private:
  int32_t FillAvailableGrainsList() {
    int32_t num_available_grains = 0;
    for (int32_t i = 0; i < max_num_grains_; ++i) {
      if (!grains_[i].active()) {
        available_grains_[num_available_grains] = i;
        ++num_available_grains;
      }
    }
    return num_available_grains;
  }
  
  void ScheduleGrain(
      Grain* grain,
      const Parameters& parameters,
      int32_t pre_delay,
      int32_t buffer_size,
      int32_t buffer_head,
      GrainQuality quality) {
    float position = parameters.position;
    // Chord clouds: the scheduler leaves this grain's chord-tone transpose
    // (semitones) in ars_transpose_; 0 when harmony is off.
    float pitch = parameters.pitch + ars_transpose_;
    float window_shape = parameters.granular.window_shape;
    float grain_size = Interpolate(lut_grain_size, parameters.size, 256.0f);
    float pitch_ratio = SemitonesToRatio(pitch);
    float inv_pitch_ratio = SemitonesToRatio(-pitch);
    float pan = 0.5f + parameters.stereo_spread * (Random::GetFloat() - 0.5f);
    float gain_l, gain_r;
    if (num_channels_ == 1) {
      gain_l = Interpolate(lut_sin, pan, 256.0f);
      gain_r = Interpolate(lut_sin + 256, pan, 256.0f);
    } else {
      if (pan < 0.5f) {
        gain_l = 1.0f;
        gain_r = 2.0f * pan;
      } else {
        gain_r = 1.0f;
        gain_l = 2.0f * (1.0f - pan);
      }
    }
    
    if (pitch_ratio > 1.0f) {
      // The grain's play-head moves faster than the buffer record-head.
      // we must make sure that the grain will not consume too much data.
      // In some situations, it might be necessary to reduce the size of the
      // grain.
      grain_size = std::min(grain_size, buffer_size * 0.25f * inv_pitch_ratio);
    }

    float eaten_by_play_head = grain_size * pitch_ratio;
    float eaten_by_recording_head = grain_size;

    float available = 0.0;
    available += static_cast<float>(buffer_size);
    available -= eaten_by_play_head;
    available -= eaten_by_recording_head;

    int32_t size = static_cast<int32_t>(grain_size) & ~1;
    int32_t start = buffer_head - static_cast<int32_t>(
        position * available + eaten_by_play_head);
    grain->Start(
        pre_delay,
        buffer_size,
        start,
        size,
        static_cast<uint32_t>(pitch_ratio * 65536.0f),
        window_shape,
        gain_l,
        gain_r,
        quality);
    ONE_POLE(grain_size_hint_, grain_size, 0.1f);
  }
  
  int32_t max_num_grains_;
  int32_t num_midfi_grains_;
  int32_t num_channels_;

  float num_grains_;
  float gain_normalization_;
  float grain_size_hint_;
  float grain_rate_phasor_;
  ArsGapGenerator ars_gaps_;
  float ars_countdown_;
  float ars_transpose_;
#ifndef ARS_LEAN
  int16_t ars_bar_;
  int16_t ars_step_;
  float ars_step_countdown_;
#endif
  
  Grain grains_[kMaxNumGrains];
  int32_t available_grains_[kMaxNumGrains];
  float envelope_buffer_[kMaxBlockSize];
  
  DISALLOW_COPY_AND_ASSIGN(GranularSamplePlayer);
};

}  // namespace clouds

#endif  // CLOUDS_DSP_GRANULAR_SAMPLE_PLAYER_H_
