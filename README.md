# Custom 3rd Zigock Bot II for Yamagi Quake II

This is a custom port of the 3rd Zigock Bot II to Linux Yamagi Quake II. \
All warnings (up to gcc8) and unused variables have been fixed in the original source. \
The code has many backport fixes, enhancements and features applied (custom, tsmod & yquake2).

BASE_DIR is defined in `src/header/local.h`, for Yamagi Quake II use in Debian:

    /usr/share/games/quake2

For mod installation in:

    /usr/share/games/quake2/3zb2

Bot chaining routes are supplied, further routes can be (re)created via the mod `chedit` (See `CONFIG.txt`)

Custom features have been added to the `zigmode` Capture and Hold element of the game. Including enhancements \
to the HUD, a flag return function and sound feedback additions which subtly alter the game dynamics and add \
stability over the original release. It plays best on smaller level maps with a few bots.

Example config file for ZigFlag:

```
set zigmode 1
set zigspawn 1
set aimfix 1
set botlist default
set autospawn 5
set vwep 1
set maxclients 16
set dmflags 16384
set fraglimit 40
set timelimit 15
map q2dm1
```

See `CONFIG.txt` for further details.

## Installation

* Copy the *3zb2/* dir to your Quake II directory.
* Build the `game.so` by calling `make` and copy it to the *3zb2/* dir.

```
sudo cp -R 3zb2 /usr/share/games/quake2/3zb2

make 
===> Building game.so
make release/game.so
make[1]: Entering directory '/home/user/3zb2'
===> CC src/bot/bot.c
===> CC src/bot/fire.c
===> CC src/bot/func.c
===> CC src/bot/za.c
===> CC src/g_chase.c
===> CC src/g_cmds.c
===> CC src/g_combat.c
===> CC src/g_ctf.c
===> CC src/g_func.c
===> CC src/g_items.c
===> CC src/g_main.c
===> CC src/g_misc.c
===> CC src/g_monster.c
===> CC src/g_phys.c
===> CC src/g_save.c
===> CC src/g_spawn.c
===> CC src/g_svcmds.c
===> CC src/g_target.c
===> CC src/g_trigger.c
===> CC src/g_utils.c
===> CC src/g_weapon.c
===> CC src/monster/move.c
===> CC src/player/client.c
===> CC src/player/hud.c
===> CC src/player/menu.c
===> CC src/player/trail.c
===> CC src/player/view.c
===> CC src/player/weapon.c
===> CC src/shared/shared.c
===> LD release/game.so
make[1]: Leaving directory '/home/user/3zb2'

sudo cp release/game.so /usr/share/games/quake2/3zb2
```

## Running bots

To start `$` bots automatically, append:

    +set autospawn $

### Deathmatch

    /usr/lib/yamagi-quake2/quake2 -datadir /usr/share/games/quake2/ +set game 3zb2 +set deathmatch 1 +set autospawn 5 +exec game.cfg

### CTF (Capture the Flag)

Copy **CTF** `.pak` files to *3zb2/*

    sudo cp /usr/share/games/quake2/ctf/*.pak /usr/share/games/quake2/3zb2

**Single process**:

    /usr/lib/yamagi-quake2/quake2 -datadir /usr/share/games/quake2/ +set vid_gamma 1.400000 +set game 3zb2 +set deathmatch 1 +exec ctf.cfg 

**Separate server and client**:

    /usr/lib/yamagi-quake2/q2ded -datadir /usr/share/games/quake2/ +set game 3zb2 +set ip 127.0.0.1 +set port 27910 +set autospawn 5 +exec ctfserver.cfg

    /usr/lib/yamagi-quake2/quake2 -datadir /usr/share/games/quake2/ +set vid_gamma 1.400000 +game 3zb2 +exec ctfplayer.cfg +connect 127.0.0.1:27910

**Default grapple:** `bind MWHEELDOWN +hook`

## Bot commands (See full details in CONFIG.txt)

By default use: `KP_PLUS`, `KP_MINUS` & `KP_ENTER` for bot control

`Console`

Spawn `$` bots via:

    sv spb $

Remove `$` bots via:

    sv rmb $

## Other features (See full details in CONFIG.txt)

Improved aim, enable `1` (default) or disable `0` via:

    aimfix $

`Capture and Hold (ZigFlag)` mode for Deathmatch/Team games:

    zigmode $
    zigspawn $

