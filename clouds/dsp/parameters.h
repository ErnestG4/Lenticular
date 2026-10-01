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
// Parameters of the granular effect.

#ifndef CLOUDS_DSP_PARAMETERS_H_
#define CLOUDS_DSP_PARAMETERS_H_

#include "stmlib/stmlib.h"

namespace clouds {

struct Parameters {
  float position;
  float size;
  float pitch;
  float density;
  float texture;
  float dry_wet;
  float stereo_spread;
  float feedback;
  float reverb;
  
  bool freeze;
  bool trigger;
  bool gate;
  
  struct Granular {
    float overlap;
    float window_shape;
    float stereo_spread;
    bool use_deterministic_seed;
  } granular;

  // ARS rigidity-ladder grain scheduling. Zone -1 = stock scheduler
  // (deterministic/probabilistic per use_deterministic_seed); zones 0..6
  // draw inter-onset gaps from the corresponding spacing statistics, with
  // ars_character as the in-zone parameter.
  int8_t ars_zone;
  float ars_character;

  // Chord clouds: 0 = off (grains at knob pitch), 1 = Metallic Means table,
  // 2 = twelve-bar progression advanced on gap-sum wrap downbeats,
  // 3 = classic harmonic chord table. ars_chord selects the chord in table
  // modes. Voice per grain comes from the gap statistics.
  int8_t ars_harmony;
  float ars_chord;

  // Reese: alternating grains detune +/- half this many cents around their
  // chord tone (or around knob pitch with harmony off). Long overlapping
  // grains turn the pairs into the classic beating growl.
  float ars_detune;

  // Realization identity: 0 CCW tight 8-gap riff .. 0.5 stock 64 .. 1 full
  // freerun (fray). TRIG replays the stored realization from the top.
  float ars_loop;
  bool ars_psola;    // VOICE mode: player runs the TD-PSOLA scheduler
  float ars_period;  // tracked fundamental period, samples; 0 = unvoiced
  
  struct Spectral {
    float quantization;
    float refresh_rate;
    float phase_randomization;
    float warp;
  } spectral;
};

}  // namespace clouds

#endif  // CLOUDS_DSP_PARAMETERS_H_
