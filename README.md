# Yuumi Pet

A small, hardware-independent virtual pet. The current milestone is a playable
command-line version; pet rules and save data live in the core so future desktop
and microcontroller interfaces can reuse them.

## Build

Open this folder in VS Code with the CMake Tools extension, select an installed
compiler kit, then build the `yuumi_pet` target. The `yuumi_pet_tests` target
checks core behavior and save/load handling.

## Play

Run the `yuumi_pet` executable from the project directory. It draws a live,
fixed-layout screen (animation, stat bars, message line) that updates in place.
Keys:

- `f` opens a food menu; press its number to feed
- `p` to play (costs energy and hunger)
- `r` to rest
- `s` / `l` to save / load the pet (`yuumi.sav`)
- `q` or Ctrl+C to quit

Needs change in real time. Pet rules live in the core, `ui/Screen` is a fixed
character grid (the part an LCD driver would replace), and `platform/Terminal`
handles raw keys and the alternate screen.
