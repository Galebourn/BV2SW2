Blobby Volley 2 for Switch 2 / kaiHEN
Modern menus update, 25 November 2026.
Initial version confirmed running on Switch 2; this revision needs hardware testing.
LAN and online play remain untested.

Download:
https://github.com/Galebourn/BV2SW2/actions/runs/45713417353/artifacts/15068302791

Copy switch/blobby2 to /switch/blobby2 on your SD card.
Existing users: replace blobby.nro only and keep data/ and user/.
Launch through kaiHEN's native-game homebrew menu.

Menus: stick/D-pad follows visible layout. A selects or starts editing.
Editing lists: Up/Down. Editing sliders: Left/Right. A/B finishes editing.
B returns/cancels. Touch taps buttons/list rows and drags sliders.
Gameplay: left stick moves, A jumps, Plus pauses.
Preferences/replays: /switch/blobby2/user.

Source commit: ad422cc4412698b3a8520876be89faba2484fad3
Build: https://github.com/Galebourn/BV2SW2/actions/runs/45713417353
Passed: navigation and native SDL interaction tests, cross-compile, NRO generation.
ZIP SHA-256: ffb11e431a26f65982187e8ac2cf94d6ea25f41a9e35c28f97aec1f1e1d5db20

Build:
docker build --file switch2/Dockerfile --output type=local,dest=dist .

See README.md, COPYING, AUTHORS and switch2/ounce-LICENSE.
