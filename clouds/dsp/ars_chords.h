// Copyright 2026 Combust.
// SPDX-License-Identifier: MIT
//
// Harmony data for ARS chord clouds. Cents tables are plain data — the same
// format as Plaits Palette chord tables — consumed per grain at schedule
// time: the gap statistics select the voice, the table supplies the cents.

#ifndef CLOUDS_DSP_ARS_CHORDS_H_
#define CLOUDS_DSP_ARS_CHORDS_H_

#include "stmlib/stmlib.h"

namespace clouds {
namespace ars {

const int kChordNumVoices = 4;

// The Metallic Means table (Combust, Plaits Palette library): Stern-Brocot
// mediants from the 1/2 lock out to noble phi, then folded metallic stacks.
const int kNumMetallicChords = 11;
const int16_t kMetallicChords[kNumMetallicChords][kChordNumVoices] = {
  { 0,   2, 1198, 1200 },  // Octaves
  { 0, 600, 1200, 1800 },  // Brocot 1/2
  { 0, 800, 1600, 2400 },  // Mediant 2/3
  { 0, 720, 1440, 2160 },  // Mediant 3/5
  { 0, 750, 1500, 2250 },  // Mediant 5/8
  { 0, 738, 1477, 2215 },  // Mediant 8/13
  { 0, 833, 1666, 2499 },  // Golden 833
  { 0, 868, 1737, 2605 },  // Bronze (folded)
  { 0, 326,  652,  978 },  // Silver (folded)
  { 0, 487,  974, 1460 },  // Plastic
  { 0, 487,  833, 1526 },  // Alloy
};

// The classic Plaits chord set (original catalog cents) — actual harmonic
// intervals, knob order octave → minor family → major family.
const int kNumHarmonicChords = 11;
const int16_t kHarmonicChords[kNumHarmonicChords][kChordNumVoices] = {
  { 0,   1, 1199, 1200 },  // Octave
  { 0, 700,  701, 1200 },  // Fifth
  { 0, 500,  700, 1200 },  // Sus4
  { 0, 300,  700, 1200 },  // Minor
  { 0, 300,  700, 1000 },  // Minor 7th
  { 0, 300, 1000, 1400 },  // Minor 9th
  { 0, 300, 1000, 1700 },  // Minor 11th
  { 0, 200,  900, 1600 },  // 6/9
  { 0, 400, 1100, 1400 },  // Major 9th
  { 0, 400,  700, 1100 },  // Major 7th
  { 0, 400,  700, 1200 },  // Major
};

// Just-intonation dominant seventh, for the progression demo.
const int16_t kJustDom7[kChordNumVoices] = { 0, 386, 702, 969 };

// Twelve-bar blues, roots in just cents against the drone key:
// I I I I | IV IV I I | V IV I V.
const int kNumProgressionBars = 12;
const int16_t kTwelveBarRoots[kNumProgressionBars] = {
  0, 0, 0, 0, 498, 498, 0, 0, 702, 498, 0, 702 };

// Bar length for progression advance, in mean-gap units: the necklace's
// cumulative gap sum wraps here, so bars breathe with the statistics.
const float kBarGapUnits = 8.0f;

// Riff mode: the progression's grains fire on a swung 8-step grid — a Steps
// source, not a point process ("decomposed twelve-bar", not circus music).
// Cents against the bar root, just intonation; -1 = rest.
const int kRiffSteps = 8;
const int kNumRiffs = 4;
const int16_t kRiffs[kNumRiffs][kRiffSteps] = {
  {   0, 386, 702, 884, 969, 884, 702, 386 },  // boogie-woogie walk
  {   0,  -1,  -1, 316, 498,  -1,   0,  -1 },  // green-onions stabs
  {   0,   0, 969, 702, 498, 316,   0,  -1 },  // baby-left-me walkdown
  {   0,   0, 316, 386,   0,  -1,  -1,  -1 },  // stop-time (I'm a Man)
};
const float kRiffSwing = 0.58f;  // long-short step pairing

}  // namespace ars
}  // namespace clouds

#endif  // CLOUDS_DSP_ARS_CHORDS_H_
