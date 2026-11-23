Blobby Volley 2 — experimental Switch 2 / kaiHEN port

STATUS: SOURCE DRAFT ONLY. Not compiled or tested on hardware. No NRO is
included in this source package. Further compile/link/runtime fixes may be needed.

Based on danielknobe/blobbyvolley2 commit:
c28c5fa87872b7592f34f5f86196e93d127b6cf9
The included ounce installer is from ouncebrew/ounce-devkit commit:
5e957599bd498643d82e5470aec80da67a12685d (MIT; see ounce-LICENSE).
SDK packages are rolling versions; successful builds record build-packages.txt.

Build on a machine with Docker and access to devkitPro/ounce package servers:
  docker build --file switch2/Dockerfile --output type=local,dest=dist .
Run from the repository root. Alternatively upload this source to your own
GitHub repository and run the manual 'Experimental Switch 2 kaiHEN build'
workflow from Actions. No repository has been created or published for you.

After a SUCCESSFUL build, copy dist/switch/blobby2 to /switch/blobby2 on the
SD card. Keep the directory name: the port uses sdmc:/switch/blobby2/data.
Launch blobby.nro through kaiHEN's homebrew menu in a native Switch 2 game.
This package does not install or modify kaiHEN or console firmware.

First-run defaults: left human player versus right AI; left stick moves;
A jumps/selects; D-pad navigates; Plus goes back/pauses. These mappings are
inherited from the original Switch SDL port and still require hardware testing.
Configuration/replays are written below /switch/blobby2/user.
To reset preferences, back up and remove that user directory's config files.

Changes: ounce CMake toolchain integration; NRO packaging; explicit SDL/PhysFS
static libraries and OunceVK EGL; separate SD preference directory; applet exit
loop; socket initialization in release builds; SDL renderer error reporting;
controller and AI defaults. The game logic and assets are upstream originals.

Known unverified areas: compilation/link dependencies, GPU initialization,
PhysFS SD support, controller enumeration and hotplug, audio, suspend/resume,
network multiplayer. The immediate target is a local match against the AI.

Hardware smoke test after compilation:
1. Launch and check title/menu display, joystick navigation and audio.
2. Start a local match; move, jump, score, and confirm the AI plays.
3. Pause, return to menu, quit, and relaunch to check saved preferences.
4. Report firmware, kaiHEN version, build-packages.txt and any error text.

Local verification: git diff --check, XML parsing, patch applicability and
source-package inspection. Cross-compilation was blocked: no devkitPro,
Docker or installed WSL; package downloads returned HTTP 403; MSYS shell
initialization also failed in the execution environment.

Licensing: retain COPYING and AUTHORS. Blobby Volley 2 is GPL-2.0-or-later;
individual bundled dependencies retain their upstream notices. The complete
modified game source is included. Distribute corresponding source alongside
any resulting binary, with relevant toolchain/library license notices.

References:
https://github.com/danielknobe/blobbyvolley2
https://github.com/ouncebrew/ounce-devkit
https://github.com/ouncebrew/kaihen
