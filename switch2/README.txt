Blobby Volley 2 - local play, rumble and beach update

Existing installation:
Copy the update ZIP's switch folder onto the SD card and merge folders.
Replace /switch/blobby2/blobby.nro and /switch/blobby2/data/backgrounds.zip.
Keep your other data files and /switch/blobby2/user (settings and replays).

Single Player: human on the left against the configured computer opponent.
Two Players: two humans sharing this Switch. Join order assigns left then right.
Player names, colors and computer difficulty remain in Options.

Controllers:
- A pair of vertical Joy-Cons or a Pro Controller: press L+R to join.
- A single horizontal Joy-Con: detach it, choose Split Joy-Cons if needed,
  then press SL+SR on each controller.
- Paired / Pro restores paired mode. Press L+R on each pair/Pro to join.
- Mixed single Joy-Con and full-controller setups are accepted.
- Stick moves. The right face button jumps/selects: A on full controllers,
  the Down arrow on a sideways left Joy-Con, X on a sideways right Joy-Con.
- Plus or Minus pauses. The bottom face button goes back in menus.
- Two-player Start Match appears only when two distinct controllers are joined.
- A missing controller pauses local matches until reconnected.

Rumble:
Controllers -> Rumble On/Off (saved), with a Test Rumble button.
Join a controller before testing. Short 65 ms pulses occur on local human
ball hits, routed to that player's controller. Disabling rumble stops pulses.
System vibration settings and controller hardware still apply.

New backgrounds:
Options -> Misc -> Background. Choose lagoon, sunset, moonlight or jungle-island.
The original two fields remain available. Physics is unchanged.
Jungle Island uses the supplied island picture as inspiration.
Artwork was produced with the built-in image_gen tool; prompts are included
in the separate artwork download and data/backgrounds/ARTWORK.txt in source.

Validation:
Navigation, controller orientation, per-player assignment, rumble timing,
disconnect and tick wrap tests; native SDL menu/touch and settings tests;
actual game renders of all four fields; Switch 2 build and NRO/asset checks.
The initial game was confirmed running on Switch 2 by the user.
This new controller/rumble revision needs hardware confirmation.
LAN and online play remain untested; rumble is currently for local matches.

Source: d4ddec1bdbf4b87c2e9a71c365cda62dbfffe010
Build: https://github.com/Galebourn/BV2SW2/actions/runs/45717104114

Build from the repository root:
docker build --file switch2/Dockerfile --output type=local,dest=dist .

See COPYING, AUTHORS and switch2/ounce-LICENSE for credits and licensing.

Full package:
https://github.com/Galebourn/BV2SW2/actions/runs/45717104114/artifacts/15071285246
ZIP SHA256: d824cebb55009e515f739db322083230565d5c9f352d39d2fa9f05d154c44424
