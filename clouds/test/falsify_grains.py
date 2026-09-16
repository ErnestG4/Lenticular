#!/usr/bin/env python3
"""Grain-onset falsification for the ARS clouds firmware (calibrator zoo #2).

Verifies that the spacing statistics survive the whole shipped pipeline —
panel mapping (TEXTURE -> zone/character), scheduler, grain windows, output
stage — by recovering each zone's class from rendered audio onsets.

The measurement channel is the TEST build's scheduler tap (stderr lines
"ARS <sample>"), because acoustic onset detection is defeated by
overlapping smooth-windowed grains (measured: 40% recall, CV exploded).
Render through the PANEL path (texture drives the zone; the raw zone
argument is bypassed by the firmware map):

    ars_ab drone.raw out.wav 1 0.24 0.5 60 0 0 0 0.08 0 <texture> 0 -1 <loop> \
        2> onsets-<kind>.txt

    texture/loop: silk 0.20/0.5 · motif 0.55/0.5 · loose 0.90/0.98

then:  python3 falsify_grains.py onsets-*.txt

Checks:
  silk  (z1, char 0.5)  CV ~ 0.18 (chained-jitter theory 0.9*c*sqrt(1/6)),
                        Fano << 1 (hyperuniform; the 32-gap replay ring
                        makes long-range variance even lower)
  motif (z2)            <= 3 distinct gap values (three-distance theorem),
                        merge tolerance for window smear
  loose (z5, freerun)   CV against a paralyzable-dead-time Poisson
                        reference, D = detector minimum separation
                        (the BubbleTime measurement-channel lesson)
"""
import math
import random
import sys

MIN_SEPARATION_S = 0.008


def onsets(path):
    out = []
    for line in open(path):
        if line.startswith('ARS '):
            out.append(int(line.split()[1]))
    return out, 32000


def stats(ev):
    gaps = [b - a for a, b in zip(ev, ev[1:])]
    m = sum(gaps) / len(gaps)
    v = sum((g - m) ** 2 for g in gaps) / len(gaps)
    W = int(20 * m)
    counts = [sum(1 for e in ev if s <= e < s + W)
              for s in range(ev[0], ev[-1] - W, W)]
    cm = sum(counts) / len(counts)
    cvar = sum((c - cm) ** 2 for c in counts) / len(counts)
    uniq = []
    for g in sorted(gaps):
        if not uniq or g - uniq[-1] > int(0.004 * 32000):
            uniq.append(g)
    return m, math.sqrt(v) / m, (cvar / cm if cm else 0.0), len(uniq)


def poisson_reference(T, D, n=200000):
    random.seed(13)
    t, raw = 0.0, []
    for _ in range(n):
        t += random.expovariate(1.0) * T
        raw.append(t)
    out, gate_end = [], -1.0
    for e in raw:
        if e >= gate_end:
            out.append(e)
        gate_end = e + D
    gaps = [b - a for a, b in zip(out, out[1:])]
    m = sum(gaps) / len(gaps)
    v = sum((g - m) ** 2 for g in gaps) / len(gaps)
    return m / T, math.sqrt(v) / m


def main(paths):
    failures = 0
    for path in sorted(paths):
        kind = ('silk' if 'silk' in path else
                'motif' if 'motif' in path else 'loose')
        ev, sr = onsets(path)
        ev = ev[8:]
        if len(ev) < 60:
            print(f'{kind}: only {len(ev)} onsets — render too sparse/smeared')
            failures += 1
            continue
        m, cv, fano, distinct = stats(ev)
        if kind == 'silk':
            ok = 0.06 < cv < 0.30 and fano < 0.15
        elif kind == 'motif':
            ok = distinct <= 3
        else:
            # scheduler tap has no dead time beyond the 1-sample floor
            ok = abs(cv - 1.0) < 0.12
        verdict = 'PASS' if ok else 'FAIL'
        if not ok:
            failures += 1
        print(f'{kind:6s} n={len(ev):4d} meanT={m:7.1f} CV={cv:.3f} '
              f'Fano={fano:.3f} distinct={distinct} -> {verdict}')
    return failures


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
