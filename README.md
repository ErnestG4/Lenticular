# Lenticular

*Alternative firmware for Mutable Instruments Clouds — the cloud that
stands still while the air moves through it.*

A granular scheduler rebuilt around **spacing statistics**: hyperuniform
washes that cannot clump, quasiperiodic motif scrubbing, Poisson, and a
rigid chip-arp grid — plus per-grain chord harmonization, Reese twins,
and replayable grain clouds. Full manual: [clouds/ARS-MANUAL.md](clouds/ARS-MANUAL.md).

**Status: v0.7 pre-release, under hardware testing.** Flashable WAV
releases will appear once the current build clears.

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
