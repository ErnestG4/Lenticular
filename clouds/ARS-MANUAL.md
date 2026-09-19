# Lenticular — alternative firmware for Mutable Instruments Clouds

*Named for the cloud that stands still while the air moves through it.*

A granular scheduler rebuilt around **spacing statistics**. Every granular
engine ever shipped schedules grains at one of two points: the metronome or
the dice. This firmware owns the space between — hyperuniform streams that
never clump, quasiperiodic streams that scrub motivically, Poisson and
beyond — plus per-grain **chord harmonization**, **Reese twins**, and
**replayable grain clouds**. It turned out, somewhat by surprise, to be a
top-shelf **vocal processor**. Sing into it.

Built on Émilie Gillet's Clouds (MIT). Statistics engine descended from
BubbleTime (Plaits Palette library). By Combust, with Claude.

## Flashing

Standard Clouds audio update: hold the blend button on power-up, play the
firmware WAV into the left input at full line level, wait ~2 minutes.
If it fails to boot afterwards, just flash again — the audio transfer is
fragile and a single glitch corrupts it; the build is fine.

## What changed, what didn't

Only **granular mode** (mode 1) is rebuilt. Stretch, looping delay, and
spectral modes are stock, including their reverb. FREEZE, POSITION, SIZE,
PITCH, IN GAIN behave exactly as stock everywhere.

## Granular mode panel

| Control | Function |
|---|---|
| **POSITION** | Buffer position. In the **motif and chip zones** grains snap to the nearest detected transient of your material — POSITION means *which hit*, not which tape-spot. Silk and loose stay free (washes must not lump). |
| **SIZE** | Grain size, stock. |
| **PITCH** | Global transpose, stock (chord tones stack on top of it). |
| **DENSITY** | Grain rate, full knob travel. No dead zone at noon; fully CCW is *very sparse* (~1 grain per 2 s floor), never silent. |
| **TEXTURE** | **Timing personality** — the rigidity ladder: |
| | · CCW→9 o'clock: **silk** — hyperuniform, with the smoothest grain windows: this is the *cloud* zone, washes that cannot clump. Knob sets jitter depth. |
| | · 9 o'clock→2 o'clock: **motif** — quasiperiodic (three-distance). Grains fall in patterns of at most three distinct spacings; the knob walks the Stern-Brocot path from a rigid 1/2 lock out to the golden ratio. Melodic scrubbing. |
| | · 2 o'clock→4 o'clock: **loose** — Poisson. The classic random cloud; the stock-firmware feel lives here. |
| | · 4 o'clock→CW: **chip** — a rigid grid cycling the chord voices in strict order: the Follin/demoscene arpeggio. DENSITY is a musical arp clock, ~2.5 to ~57 notes/s (log): 9 o'clock ≈ a 5/s arp, 3 o'clock ≈ the classic 32/s run, the very top fuses into the spectral chord-cloud illusion. Needs Harmony up to speak. |
| **FREEZE** | Stock. Freeze + TEXTURE sweep is a tour of one buffer through four universes. |
| **hold MODE + DENSITY** | **Loop/fray** (hidden, with pickup): CCW tightens the replayed realization to an 8-gap riff, noon = stock 32, CW frays it with fresh draws toward full freerun. Saved across power. |
| **TRIG in** | **Replay**: restarts the stored grain realization from the top — the *same* cloud every strike, phrase for phrase. Clock it and the cloud riffs. |

## Blend pages (blend button cycles, knob sets)

| Page | LED | Function |
|---|---|---|
| 1 | — | **Dry/wet**, stock. |
| 2 | — | **Twins** — every grain spawns a twin, detuned ±0–15 cents each way (30¢ total spread at full), and the pair spreads across the stereo field as you turn. At high density + large SIZE this is the Reese: continuous beating growl from any input. Costs half the polyphony at full effect; worth it. |
| 3 | — | **Feedback**, stock — the "echo" personality. Pairs beautifully with motif timing. |
| 4 | — | **Harmony** *(was reverb — you have a rack for that)*: CCW = off; turning up walks a chord ladder — octave, fifth, sus4, minor, m7, m9, m11, 6/9, M9, M7, major. Each grain is transposed to a **chord tone chosen by the timing statistics**: short gaps play low voices, long gaps high ones. Silk arpeggiates evenly; motif plays three-note figures; loose sprays free harmony. |

## Recipes

- **The vocal processor** (the headline): voice in, full wet, DENSITY 2
  o'clock, TEXTURE in the motif zone, Harmony at m7, a breath of Feedback.
  Speak or sing; it answers in harmonized fragments of you.
- **Silk wall**: sustained input, DENSITY high, TEXTURE fully CCW, Harmony
  off. A texture with no lumps — impossible on stock.
- **Reese anything**: bass-ish input, SIZE past 3 o'clock, DENSITY high,
  page 2 to taste. Works on things that have no business being a Reese.
- **Chord cloud** (the classic, harmonized): FREEZE something, SIZE past
  3 o'clock, DENSITY high, TEXTURE fully CCW (silk), Harmony at a chord,
  full wet. Grains at every chord tone overlap into one sustained billow.
- **Chord drone**: any drone, Harmony at a major-family chord, TEXTURE
  silk, PITCH down an octave. A choir out of a sine.
- **Replay riffing**: clock TRIG at bar rate; the cloud becomes a
  repeating phrase. Change TEXTURE/Harmony between strikes — same rhythm
  bones, new flesh.
- **Feedback motif echo**: motif zone + page 3 around 0.5 — the grains'
  quasiperiodic pattern prints into the feedback path and compounds.
- **Chop shop**: drums or riffs in, motif zone, DENSITY ~noon — the
  firmware re-orders your own hits into quasiperiodic figures. Chip zone
  instead: your hits become the arp material, phrase-locked to V/Oct.
- **The beeper returns**: TEXTURE fully CW (chip), Harmony at m7 or m11,
  DENSITY around 9 o'clock for a slow demoscene arp of whatever you feed
  it — then sweep DENSITY up and hear the arp fuse into a chord cloud,
  exactly the trick the ZX beeper drivers played on the ear.

## Notes, honestly

- The grain realization loops (hold MODE + DENSITY sets length/fray);
  TRIG replays it from the top. A reseed gesture exists in the engine
  but has no panel home yet — future version.
- There is deliberately no internal riff engine: **V/Oct is the riff
  engine.** Sequence the pitch input — chords and the chip arp transpose
  relatively, and in the chip zone a note change restarts the arp at the
  chord root, phrase-locked, the way the beeper drivers did.
- The statistics are the verified BubbleTime family: hyperuniform means
  *provably* anti-clumping, not "smoothed random."
- Reverb is gone from granular mode only. Stretch/looping/spectral keep it.

## Version

v0.7 — **named Lenticular** · onset-aware POSITION (motif/chip snap to
your transients) + loop/fray on hold-MODE+DENSITY (saved).
v0.6 — CHIP zone: the Follin arp comes to Clouds (TEXTURE top).
v0.5 — continuous-phase Reese (frozen = exact).
v0.4 — silk billows: grain windows follow the timing personality
(silk smoothest, motif articulate), overlap ceiling restored to stock reach.
v0.3 — first sounding release (2026-09-15). v0.1 booted silent (boot-time
ADC ramp poisoned the scheduler; fixed), v0.2 didn't boot (the author
briefly believed CCM RAM was vacant; Émilie's audio buffer disagreed).
