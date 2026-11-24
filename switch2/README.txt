Blobby Volley 2 for Switch 2 / kaiHEN
Status: compiled successfully, hardware testing pending.

Download:
https://github.com/Galebourn/BV2SW2/actions/runs/45497233311/artifacts/14970395413

Copy the extracted switch/blobby2 folder to /switch/blobby2 on the SD card.
Keep data/ next to blobby.nro. Launch through kaiHEN's native-game homebrew menu.
Left stick: move. A: jump/select. D-pad: menu. Plus: pause/back.
First-run default is human versus AI. Controller mappings need hardware testing.
Preferences and replays are created under /switch/blobby2/user.

Source commit: fe3a039ab4f41c01a45eedbb4b3f91b00180eed7
Build: https://github.com/Galebourn/BV2SW2/actions/runs/45497233311
Checks passed: cross-compile, link, NRO generation, NRO header, asset presence.
ZIP SHA-256: af1a04505c5caddf633445706a035828c6f40219d661a5d36e7a1f0099e72071

Build from repository root:
docker build --file switch2/Dockerfile --output type=local,dest=dist .

The historical switch2-port.patch is only the initial bootstrap patch.
Current source files include the subsequent input-settings compilation fix.
See the repository README, COPYING, AUTHORS, and switch2/ounce-LICENSE.

Menu update 2026-11-24: dominant-axis stick navigation, controlled repeat,
gold focus outline and dark button background. Navigation regression tests pass.
The initial build runs on the tester's Switch 2. This update needs hardware
confirmation; LAN and online play are untested.
Existing installations: replace blobby.nro only; keep data/ and user/.
