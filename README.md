# Blobby Volley 2 for Switch 2 / kaiHEN

Experimental native Switch 2 port of [Blobby Volley 2](https://github.com/danielknobe/blobbyvolley2).

**The initial build runs on Switch 2 hardware, as confirmed by its tester. The menu update below is compiled and regression-tested; hardware confirmation of the update is pending. LAN and online play remain untested.**

## Menu update — 24 November 2026

- Left-stick menus choose the dominant axis instead of letting horizontal input override a vertical tilt.
- Dead-zone hysteresis and a 350 ms initial / 140 ms subsequent repeat delay reduce accidental steps.
- Selected buttons have a gold outline and dark background; sliders and text/list widgets have a matching focus outline.
- Gameplay stick handling is unchanged. Regression tests cover diagonals, cardinal directions, dead zones, repeat timing and tick wraparound.
- Existing users only need to replace `/switch/blobby2/blobby.nro`; keep `data/` and `user/`.

## Download and install

[Download the compiled package](https://github.com/Galebourn/BV2SW2/actions/runs/45497233311/artifacts/14970395413) (GitHub sign-in may be required).

Extract the ZIP and copy its `switch/blobby2` directory to `/switch/blobby2` on the SD card. Keep the `data` directory alongside `blobby.nro`; the NRO alone is insufficient. Launch it in kaiHEN's homebrew menu using a native Switch 2 game.

First-run defaults: one human player on the left versus AI on the right. Left stick moves, A jumps/selects, D-pad navigates, and Plus pauses/goes back. Controller mappings, graphics and audio still need hardware testing.

Preferences and replays are stored under `/switch/blobby2/user`, created on launch.

## Build and verification

- Successful source commit: `fe3a039ab4f41c01a45eedbb4b3f91b00180eed7`.
- [Successful build](https://github.com/Galebourn/BV2SW2/actions/runs/45497233311).
- ARM64 cross-compilation, linking, NRO packaging and NRO magic check passed.
- Package contains the game assets and installed-package version manifest.
- ZIP SHA-256: `af1a04505c5caddf633445706a035828c6f40219d661a5d36e7a1f0099e72071`.

Build with Docker from the repository root:

```sh
docker build --file switch2/Dockerfile --output type=local,dest=dist .
```

The GitHub Actions workflow also builds pushes to main or manual runs. The initial import patch is historical; current source files include subsequent compilation fixes.

## Credits and license

Based on upstream commit `c28c5fa87872b7592f34f5f86196e93d127b6cf9`. Original game logic and assets belong to the upstream contributors. See COPYING and AUTHORS for GPL and attribution details. The ounce installer retains its MIT license in `switch2/ounce-LICENSE`.

Uses [ounce-devkit](https://github.com/ouncebrew/ounce-devkit) and libounce/OunceVK for kaiHEN. No firmware installer or console exploit is included.
