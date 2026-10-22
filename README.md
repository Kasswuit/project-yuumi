# Yuumi Pet

A small, hardware-independent virtual pet. The current milestone is a playable
command-line version; pet rules and save data live in the core so future desktop
and microcontroller interfaces can reuse them.

## Build

Open this folder in VS Code with the CMake Tools extension, select an installed
compiler kit, then build the `yuumi_pet` target. The `yuumi_pet_tests` target
checks core behavior and save/load handling.

## Play

Run the `yuumi_pet` executable from the project directory. Commands are:

- `feed` to reduce hunger
- `play` to increase happiness at the cost of energy and hunger
- `rest` to recover energy
- `save [file]` and `load [file]` to persist the pet (defaults to `yuumi.sav`)
- `help` and `quit`

Needs change while the game is running. The save file is versioned so its format
can evolve without tying the core to a display or hardware dependency.
