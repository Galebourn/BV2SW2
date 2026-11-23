# Blobby Volley 2 for Switch 2 / kaiHEN

Experimental native Switch 2 port of [Blobby Volley 2](https://github.com/danielknobe/blobbyvolley2).

**Compiled successfully; not yet tested on Switch 2 hardware.**

## Download and install

[Download the compiled package](https://github.com/Galebourn/BV2SW2/actions/runs/45263542944/artifacts/14861418443) (GitHub sign-in may be required).

Extract the ZIP and copy its `switch/blobby2` directory to `/switch/blobby2` on the SD card. Keep the `data` directory alongside `blobby.nro`; the NRO alone is insufficient. Launch it in kaiHEN's homebrew menu using a native Switch 2 game.

First-run defaults: one human player on the left versus AI on the right. Left stick moves, A jumps/selects, D-pad navigates, and Plus pauses/goes back. Controller mappings, graphics and audio still need hardware testing.

Preferences and replays are stored under `/switch/blobby2/user`, created on launch.

## Build and verification

- Successful source commit: `8e6f706362b5e920b20c2c96494345277efcc7e4`.
- [Successful build](https://github.com/Galebourn/BV2SW2/actions/runs/45263542944).
- ARM64 cross-compilation, linking, NRO packaging and NRO magic check passed.
- Package contains the game assets and installed-package version manifest.
- ZIP SHA-256: `a0fa678999d3f7a6cbbb6fb72ead00a01f9b309dd10b385a188715be95d60fea`.

Build with Docker from the repository root:

```sh
docker build --file switch2/Dockerfile --output type=local,dest=dist .
```

The GitHub Actions workflow also builds pushes to main or manual runs. The initial import patch is historical; current source files include subsequent compilation fixes.

## Credits and license

Based on upstream commit `c28c5fa87872b7592f34f5f86196e93d127b6cf9`. Original game logic and assets belong to the upstream contributors. See COPYING and AUTHORS for GPL and attribution details. The ounce installer retains its MIT license in `switch2/ounce-LICENSE`.

Uses [ounce-devkit](https://github.com/ouncebrew/ounce-devkit) and libounce/OunceVK for kaiHEN. No firmware installer or console exploit is included.
