# poke
a small X11 program to display notifications

> [!NOTE]
> This program is a work in progress, and it's really only meant for myself.

## Usage
```poke "Some title" "Description" "" "SomethingElse"```

There is no predefined formatting or text rendering rules. `poke` essentially goes through its `argv` values and renders them in the same order as they were passed.
Furthermore, there's no limit on the amount of values that can be passed to `poke`, but keep in mind that only the first `MAXLINES` lines will be rendered (This can be set in poke.h).

While it can be used for simple and localized notifications, you can also combine it with [tiramisu](https://github.com/Sweets/tiramisu) to get regular desktop notifications, similar to what you would see in a full-fledged desktop environment.

## Examples
To alert if a command has failed:
```somecommand || poke "Something bad happened"```

To get desktop notifications with _tiramisu_:
```
tiramisu -s -o "#source#summary#body" | while read -r tiramisu
do
        source="${tiramisu%%*}"
        summary="$(echo $tiramisu | cut -f 2 -d '')"
        body="${tiramisu##*}"

        poke "${source}: ${summary}" "" "${body}"
done
```

## Building
This program was only built and tested on FreeBSD, so you may need to deal with your distributions' or OS's nitty-gritties in order to compile it.
For starters, you need at least the following libraries:
- `libx11`
- `libxft`
- `freetype2`

Once those libraries are installed, you can follow with the classical:
1. ```make```
2. ```make install```

## Footnotes
AI was not used at all. If there's a bug, it's my fault.
