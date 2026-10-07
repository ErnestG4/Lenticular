# Lenticular

*Alternative firmware for Mutable Instruments Clouds — the cloud that
stands still while the air moves through it.*

A granular scheduler rebuilt around **spacing statistics**: hyperuniform
washes that cannot clump, quasiperiodic motif scrubbing, Poisson, and a
rigid chip-arp grid — plus per-grain chord harmonization, Reese twins,
and replayable grain clouds. Full manual: [clouds/ARS-MANUAL.md](clouds/ARS-MANUAL.md).

**Current release: v0.8** — grab the flashable WAV from the
[Releases page](https://github.com/ErnestG4/Lenticular/releases) and
play the firmware into your module (see Flashing below). v0.8 adds
**VOICE mode**: a formant-preserving TD-PSOLA pitch-shifter/harmonizer
in the spectral slot — transpose your voice or bass and it is still
you; the rigidity ladder becomes phonation, from machine-smooth to
vocal fry.

## Flashing

Standard Clouds audio update: hold the blend button on power-up, play
the release WAV into the LEFT input at full line level, wait ~2
minutes. If it doesn't boot afterwards, flash again — the audio
transfer is fragile and one glitch corrupts it.

## Building

```sh
git submodule update --init stmlib stm_audio_bootloader
docker build -f Dockerfile.plaits-builder -t lenticular-builder .
docker run --rm -v $PWD:/workspace:ro -v $PWD/build:/out -w /workspace \
  lenticular-builder sh -c \
  'make -f clouds/makefile BUILD_ROOT=/out/ PROJECT_CONFIGURATION=-DARS_LEAN wav'
```

Flash the resulting `clouds.wav` with the standard Clouds audio update
(hold blend button at power-on, play WAV into the left input).

## Credits

Built on [Émilie Gillet's Clouds](https://github.com/pichenettes/eurorack)
(MIT), from Lyle Mills' eurorack tree. Statistics engine descended from
BubbleTime (Plaits Palette library). By Combust, with Claude.
