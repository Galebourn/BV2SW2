Blobby Volley 2 for Switch 2 / kaiHEN
Status: compiled successfully, hardware testing pending.

Download:
https://github.com/Galebourn/BV2SW2/actions/runs/45263542944/artifacts/14861418443

Copy the extracted switch/blobby2 folder to /switch/blobby2 on the SD card.
Keep data/ next to blobby.nro. Launch through kaiHEN's native-game homebrew menu.
Left stick: move. A: jump/select. D-pad: menu. Plus: pause/back.
First-run default is human versus AI. Controller mappings need hardware testing.
Preferences and replays are created under /switch/blobby2/user.

Source commit: 8e6f706362b5e920b20c2c96494345277efcc7e4
Build: https://github.com/Galebourn/BV2SW2/actions/runs/45263542944
Checks passed: cross-compile, link, NRO generation, NRO header, asset presence.
ZIP SHA-256: a0fa678999d3f7a6cbbb6fb72ead00a01f9b309dd10b385a188715be95d60fea

Build from repository root:
docker build --file switch2/Dockerfile --output type=local,dest=dist .

The historical switch2-port.patch is only the initial bootstrap patch.
Current source files include the subsequent input-settings compilation fix.
See the repository README, COPYING, AUTHORS, and switch2/ounce-LICENSE.
