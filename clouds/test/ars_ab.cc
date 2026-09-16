// Copyright 2026 Combust.
// SPDX-License-Identifier: MIT
//
// A/B harness: the same input audio granulated by the stock scheduler
// (deterministic or probabilistic) and by each ARS rigidity-ladder zone.
//
//   ars_ab <input.raw> <output.wav> <zone> <density> [character] [seconds]
//
// zone: -2 stock-deterministic, -1 stock-probabilistic, 0..6 ladder zones.
// input: raw s16le stereo @ 32 kHz, looped as needed.

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "clouds/dsp/granular_processor.h"
#include "clouds/resources.h"

using namespace clouds;

const size_t kSampleRate = 32000;
const size_t kBlockSize = 32;

static void WriteWavHeader(FILE* fp, int num_samples, int num_channels) {
  uint32_t l;
  uint16_t s;
  fwrite("RIFF", 4, 1, fp);
  l = 36 + num_samples * 2 * num_channels;
  fwrite(&l, 4, 1, fp);
  fwrite("WAVE", 4, 1, fp);
  fwrite("fmt ", 4, 1, fp);
  l = 16;
  fwrite(&l, 4, 1, fp);
  s = 1;
  fwrite(&s, 2, 1, fp);
  s = num_channels;
  fwrite(&s, 2, 1, fp);
  l = kSampleRate;
  fwrite(&l, 4, 1, fp);
  l = static_cast<uint32_t>(kSampleRate) * 2 * num_channels;
  fwrite(&l, 4, 1, fp);
  s = 2 * num_channels;
  fwrite(&s, 2, 1, fp);
  s = 16;
  fwrite(&s, 2, 1, fp);
  fwrite("data", 4, 1, fp);
  l = num_samples * 2 * num_channels;
  fwrite(&l, 4, 1, fp);
}

int main(int argc, char** argv) {
  if (argc < 5) {
    fprintf(stderr, "usage: %s in.raw out.wav zone density [char] [secs]\n",
            argv[0]);
    return 1;
  }
  const char* in_path = argv[1];
  const char* out_path = argv[2];
  const int zone = atoi(argv[3]);
  const float density = atof(argv[4]);
  const float character = argc > 5 ? atof(argv[5]) : 0.5f;
  const size_t seconds = argc > 6 ? atoi(argv[6]) : 12;
  const int harmony = argc > 7 ? atoi(argv[7]) : 0;
  const float chord = argc > 8 ? atof(argv[8]) : 0.0f;
  const float detune = argc > 9 ? atof(argv[9]) : 0.0f;
  const float size_knob = argc > 10 ? atof(argv[10]) : 0.5f;
  const float feedback = argc > 11 ? atof(argv[11]) : 0.0f;
  const float texture = argc > 12 ? atof(argv[12]) : 0.5f;
  const int trig_secs = argc > 13 ? atoi(argv[13]) : 0;

  FILE* fp_in = fopen(in_path, "rb");
  if (!fp_in) {
    fprintf(stderr, "cannot open %s\n", in_path);
    return 1;
  }
  fseek(fp_in, 0, SEEK_END);
  const long in_bytes = ftell(fp_in);
  fseek(fp_in, 0, SEEK_SET);
  std::vector<ShortFrame> input_audio(in_bytes / sizeof(ShortFrame));
  if (fread(input_audio.data(), 1, in_bytes, fp_in) !=
      static_cast<size_t>(in_bytes)) {
    fprintf(stderr, "short read\n");
    return 1;
  }
  fclose(fp_in);

  static uint8_t large_buffer[118784];
  static uint8_t small_buffer[65536];
  GranularProcessor processor;
  processor.Init(&large_buffer[0], sizeof(large_buffer),
                 &small_buffer[0], sizeof(small_buffer));
  processor.set_num_channels(2);
  processor.set_low_fidelity(false);
  processor.set_playback_mode(PLAYBACK_MODE_GRANULAR);
  processor.Prepare();

  FILE* fp_out = fopen(out_path, "wb");
  size_t remaining = kSampleRate * seconds;
  WriteWavHeader(fp_out, remaining, 2);

  Parameters* p = processor.mutable_parameters();
  size_t read_pos = 0;
  size_t sample_clock = 0;
  while (remaining >= kBlockSize) {
    p->gate = false;
    p->trigger = trig_secs > 0 &&
        (sample_clock % (kSampleRate * trig_secs)) < kBlockSize;
    sample_clock += kBlockSize;
    p->freeze = false;
    p->position = 0.2f;
    p->size = size_knob;
    p->pitch = 0.0f;
    p->density = zone == -2 ? (density < 0.47f ? density : 0.47f)
                            : (density > 0.53f ? density : 0.53f);
    if (zone == -2) {
      // stock deterministic wants density < 0.5; mirror the overlap amount
      p->density = 1.0f - density;
    } else {
      p->density = density;
    }
    p->texture = texture;
    p->feedback = feedback;
    p->dry_wet = 1.0f;
    p->reverb = 0.0f;
    p->stereo_spread = 0.3f;
    p->ars_zone = zone >= 0 ? zone : -1;
    p->ars_character = character;
    p->ars_harmony = harmony;
    p->ars_chord = chord;
    p->ars_detune = detune;

    ShortFrame in[kBlockSize];
    ShortFrame out[kBlockSize];
    for (size_t i = 0; i < kBlockSize; ++i) {
      in[i] = input_audio[read_pos];
      read_pos = (read_pos + 1) % input_audio.size();
    }
    processor.Process(in, out, kBlockSize);
    processor.Prepare();
    fwrite(out, sizeof(ShortFrame), kBlockSize, fp_out);
    remaining -= kBlockSize;
  }
  fclose(fp_out);
  printf("rendered %s zone=%d density=%.2f character=%.2f\n",
         out_path, zone, density, character);
  return 0;
}
