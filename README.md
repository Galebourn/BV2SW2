# Blobby Volley 2 for Switch 2 / kaiHEN

Experimental native Switch 2 port of [Blobby Volley 2](https://github.com/danielknobe/blobbyvolley2).

**The initial build runs on Switch 2 hardware, as confirmed by its tester. The menu update below is compiled and regression-tested; hardware confirmation of the update is pending. LAN and online play remain untested.**

## Menu update — 25 November 2026

- Directional navigation follows the visible layout, including Left/Right for Yes/No dialogs.
- Main menu uses two columns; player settings use readable panels while keeping the original rainbow font.
- A enters editing for lists, sliders and text. Up/Down changes list rows; Left/Right adjusts sliders. A or B finishes editing. B returns or cancels.
- Touch supports tapping buttons and list rows, and dragging sliders.
- Gold focus outlines, selected-row highlights, dark backgrounds and context-sensitive hints clarify selection.
- Dominant-axis stick handling and controlled repeat remain in place. Gameplay controls are unchanged.
- Existing users only need to replace `/switch/blobby2/blobby.nro`; keep `data/` and `user/`.

## Download and install

[Download the compiled package](https://github.com/Galebourn/BV2SW2/actions/runs/45713417353/artifacts/15068302791) (GitHub sign-in may be required).

Extract the ZIP and copy its `switch/blobby2` directory to `/switch/blobby2` on the SD card. Keep the `data` directory alongside `blobby.nro`; the NRO alone is insufficient. Launch it in kaiHEN's homebrew menu using a native Switch 2 game.

First-run defaults: one human player on the left versus AI on the right. Left stick moves, A jumps/selects, D-pad navigates, and Plus pauses/goes back. Controller mappings, graphics and audio still need hardware testing.

Preferences and replays are stored under `/switch/blobby2/user`, created on launch.

## Build and verification

- Successful source commit: `ad422cc4412698b3a8520876be89faba2484fad3`.
- [Successful build](https://github.com/Galebourn/BV2SW2/actions/runs/45713417353).
- Native SDL interaction tests passed for spatial navigation, list/slider editing, Back/cancel, touch taps and slider dragging; rendered menu previews were inspected.
- ARM64 cross-compilation, linking, NRO packaging and NRO magic check passed.
- Package contains the game assets and installed-package version manifest.
- ZIP SHA-256: `ffb11e431a26f65982187e8ac2cf94d6ea25f41a9e35c28f97aec1f1e1d5db20`.

Build with Docker from the repository root:

```sh
docker build --file switch2/Dockerfile --output type=local,dest=dist .
```

The GitHub Actions workflow also builds pushes to main or manual runs. The initial import patch is historical; current source files include subsequent compilation fixes.

## Credits and license

Based on upstream commit `c28c5fa87872b7592f34f5f86196e93d127b6cf9`. Original game logic and assets belong to the upstream contributors. See COPYING and AUTHORS for GPL and attribution details. The ounce installer retains its MIT license in `switch2/ounce-LICENSE`.

Uses [ounce-devkit](https://github.com/ouncebrew/ounce-devkit) and libounce/OunceVK for kaiHEN. No firmware installer or console exploit is included.
