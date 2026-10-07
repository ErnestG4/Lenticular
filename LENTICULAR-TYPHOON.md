# Lenticular for Typhoon / Supercell — ALPHA

*The cloud that stands still while the air moves through it.*

Lenticular is a granular scheduler rebuilt around **spacing
statistics** — hyperuniform washes that provably cannot clump,
quasiperiodic motif scrubbing, Poisson, a rigid demoscene arp grid —
plus per-grain **chord harmonization**, **Reese twins**, **replayable
grain clouds**, and **VOICE**: a formant-preserving PSOLA
pitch-shifter/harmonizer. This build transplants the Lenticular engine
([ErnestG4/Lenticular](https://github.com/ErnestG4/Lenticular), proven
on original-architecture Clouds) onto the SuperParasites
Supercell/Typhoon platform.

## ⚠ Alpha status, honestly

- Typhoon takes Supercell-architecture firmware (builders have flashed
  patrickdowling's Supercell builds directly onto Typhoons) — this WAV
  is that build, with the Lenticular engine inside.
- **This build has not yet been verified on hardware.** It compiles
  and links against the proven SuperParasites platform code, and the
  engine itself is the same one shipping on Clouds/Cirrus — but nobody
  has flashed a Typhoon with it yet. You might be first. A bad flash
  is recoverable: reflash this WAV or stock SuperParasites.
- This is the **Supercell** build. Do not flash it into a Microcell or
  an original-architecture Clouds/Cirrus/Monsoon (and don't flash the
  Clouds Lenticular WAV into a Typhoon).
- Please report what you find — boots or doesn't, every knob that
  feels wrong — to the GitHub issues.

## Flashing

Standard audio update, as for SuperParasites: hold the boot combo your
module uses for firmware updates, play the WAV into the LEFT input at
full line level, wait. Reflash on any glitch.

## What you get

Four modes, cycled with the mode button exactly as in SuperParasites
(quality cycling and all other platform gestures unchanged):

1. **GRANULAR** — the Lenticular scheduler (below).
2. **STRETCH** — stock.
3. **LOOPING DELAY** — stock.
4. **VOICE** — formant-preserving PSOLA harmonizer (below).

**Dedicated knobs replace the blend pages** — this is where Typhoon
beats the original panel:

| Typhoon control | Lenticular function |
|---|---|
| BLEND / dry-wet pot | Dry/wet (stock) |
| **SPREAD pot + CV** | **TWINS** — every grain spawns a detuned twin (±0–15¢ each way), pairs spreading across the stereo field. High density + large SIZE = the Reese. In VOICE: the choir's stereo fan. |
| FEEDBACK pot + CV | Feedback, stock — beautiful with motif timing |
| **REVERB pot + CV** | **HARMONY** — the chord ladder: octave · fifth · sus4 · minor · m7 · m9 · m11 · 6/9 · M9 · M7 · major. CV over chord choice! (Reverb is gone from these modes — you have a rack for that.) |

## Granular mode

- **TEXTURE** — the rigidity ladder: CCW→9:00 **silk** (hyperuniform,
  smoothest windows — the cloud zone); 9:00→2:00 **motif**
  (quasiperiodic, at most three distinct spacings, knob walks
  Stern–Brocot to the golden ratio); 2:00→4:00 **loose** (Poisson, the
  stock feel); 4:00→CW **chip** (rigid grid cycling chord voices — the
  Follin arp; DENSITY becomes a musical arp clock ~2.5–57 notes/s;
  needs Harmony up).
- **DENSITY** — grain rate, full travel, never silent.
- **POSITION** — in motif/chip zones grains snap to your material's
  detected transients: POSITION means *which hit*.
- **TRIG** — replays the stored realization: the same cloud every
  strike. Clock it and the cloud riffs.
- **V/Oct is the riff engine** — in the chip zone each note change
  restarts the arp at the chord root, phrase-locked.
- **FREEZE** short press = freeze (stock). Long press = REVERSE —
  honored in stretch/looping; not yet in granular (alpha gap).

## VOICE mode (mode 4)

One wavelet per tracked glottal cycle, looped at the *target* period:
pitch moves by pulse spacing, formants never resample. Transpose your
voice or bass and it is still you.

- **PITCH / V-Oct** — formant-preserving transpose (up-shift caps ~+19 st).
- **TEXTURE** — phonation: CCW machine-perfect → healthy → patterned →
  vocal fry CW.
- **DENSITY** — wavelet width: narrow = buzzy robot, wide = soft.
- **SIZE** — choir looseness (±30¢ humanize per harmony voice).
- **HARMONY (reverb pot)** — all four chord tones at once: a choir of you.
- **SPREAD** — fans the choir in stereo.
- **FREEZE** — infinite vowel, still transposable; POSITION scrubs the
  stored cycles.
- Consonants pass untransposed on purpose: s's stay s's.

Measured on the Clouds build (same engine): transposition exact
−5…+12 st; formant band held within 1 dB where resampling smears 6 dB;
hyperuniform/chip/fry spacing statistics verified at the scheduler.

## Known alpha gaps

- Loop length / fray gesture from the Clouds build has no panel home
  here yet (the realization loops at its stock 32 events).
- REVERSE is inert in granular mode (works in stretch/looping).
- CAPTURE, WRITE/save, mutes, calibration: SuperParasites platform
  behavior, untouched but unverified against the new engine.

## Credits

Built on Émilie Gillet's Clouds (MIT), via the SuperParasites
Supercell platform (Patrick Dowling and contributors — drivers,
build system, and platform UI are theirs, unchanged). Statistics
engine descended from BubbleTime (Plaits Palette library).
By Combust, with Claude.
