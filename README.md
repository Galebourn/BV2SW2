# Blobby Volley 2 for Switch 2 / kaiHEN

Experimental native Switch 2 port of [Blobby Volley 2](https://github.com/danielknobe/blobbyvolley2).

**The initial build runs on Switch 2 hardware, as confirmed by its tester. The controller/menu update below is compiled and regression-tested; hardware confirmation of the update is pending. LAN and online play remain untested.**

## Local play, rumble and beach update — 25 November 2026

- Direct **Single Player** (human versus computer) and **Two Players** (same Switch) choices.
- Controller setup supports paired vertical Joy-Cons, Pro Controllers, and horizontal single Joy-Cons, including mixed setups. Press L+R on full controllers or SL+SR on singles to join. Join order assigns Player 1 then Player 2.
- Use **Split Joy-Cons** for detached pairs; **Paired / Pro** returns to paired mode. Start Match requires distinct joined controllers. A missing controller pauses local matches.
- The stick moves; the right face button jumps/selects (A on full controllers, Down arrow on a sideways left Joy-Con, X on a sideways right Joy-Con). Plus/Minus pauses.
- **Controllers → Rumble On/Off** is saved. **Test Rumble** tests joined controllers. Local human ball hits trigger short 65 ms pulses on the corresponding controller; paired controllers use both motors.
- Four new fields: **lagoon**, **sunset**, **moonlight**, and **jungle-island**, inspired by the supplied island image. Choose them under **Options → Misc → Background**. The original fields remain available.
- Spatial menu navigation, clear focus highlights, editing hints, touch buttons/lists and slider dragging remain available.
- Existing users should replace **both** `/switch/blobby2/blobby.nro` and `/switch/blobby2/data/backgrounds.zip`. Keep other data files and `user/`.

The new native controller and rumble implementation is compiled and tested with simulated controller states; physical Joy-Con/Pro combinations and vibration still need Switch 2 hardware confirmation. Rumble currently covers local matches. LAN and online play remain untested.

## Download and install

[Download the compiled package](https://github.com/Galebourn/BV2SW2/actions/runs/45717104114/artifacts/15071285246) (GitHub sign-in may be required).

Extract the ZIP and copy its `switch/blobby2` directory to `/switch/blobby2` on the SD card. Keep the `data` directory alongside `blobby.nro`; the NRO alone is insufficient. Launch it in kaiHEN's homebrew menu using a native Switch 2 game.

First-run defaults: one human player on the left versus AI on the right. Left stick moves, A jumps/selects, D-pad navigates, and Plus pauses/goes back. Controller mappings, graphics and audio still need hardware testing.

Preferences and replays are stored under `/switch/blobby2/user`, created on launch.

## Build and verification

- Successful source commit: `d4ddec1bdbf4b87c2e9a71c365cda62dbfffe010`.
- [Successful build](https://github.com/Galebourn/BV2SW2/actions/runs/45717104114).
- Controller tests passed for both sideways orientations, join order, separate player routing, rumble duration, disable/disconnect stopping and timer wrap.
- All four fields were rendered with game sprites and their packaged BMP dimensions verified.
- Native SDL interaction tests passed for spatial navigation, list/slider editing, Back/cancel, touch taps and slider dragging; rendered menu previews were inspected.
- ARM64 cross-compilation, linking, NRO packaging and NRO magic check passed.
- Package contains the game assets and installed-package version manifest.
- ZIP SHA-256: `d824cebb55009e515f739db322083230565d5c9f352d39d2fa9f05d154c44424`.

Build with Docker from the repository root:

```sh
docker build --file switch2/Dockerfile --output type=local,dest=dist .
```

The GitHub Actions workflow also builds pushes to main or manual runs. The initial import patch is historical; current source files include subsequent compilation fixes.

## Credits and license

Based on upstream commit `c28c5fa87872b7592f34f5f86196e93d127b6cf9`. Original game logic and assets belong to the upstream contributors. See COPYING and AUTHORS for GPL and attribution details. The ounce installer retains its MIT license in `switch2/ounce-LICENSE`.

Uses [ounce-devkit](https://github.com/ouncebrew/ounce-devkit) and libounce/OunceVK for kaiHEN. No firmware installer or console exploit is included.
