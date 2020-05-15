# Custom 3rd Zigock Bot II for Linux Quake II

This is a custom port of the 3rd Zigock Bot II to Linux Quake II clients or servers. \
All warnings (up to GCC9) and unused variables have been addressed from the original source. \
The code has backport fixes, enhancements and features applied from various sources: tsmod, \
yquake2 and custom.

This was modified for my own use and driven by nostalgia for the Quake II servers of old. \
There are many heavily modified versions of Quake II games, this mod keeps the look and feel of \
the original game, but allowed the Quake II multiplayer experience sadly lacking in the present day. \
I couldn't find any `Capture and Hold` servers running on [q2servers](http://q2servers.com) so this offered the ability to \
return a firm favourite via the `zigmode` function.

Tip of the hat to `Ponpoko`, original mod author and bot creator.

On global linux installs, **e.g.** `/usr/share/games`, you may need to specify `basepath` - See details below.

Bot chaining routes are supplied, further routes can be (re)created via the mod command `chedit` (See `CONFIG.txt`)

### ZigMode ZigFlag (Capture and Hold)

The premise is simple: **Get the flag and keep it** - *plays on standard Deathmatch maps*.

The original `zigmode` was released belated, buggy and only half implemented, I attempted to make this popular feature \
a little more refined, for fun. Whilst keeping the look and feel of the original game, essentially extending deathmatch. \
A few **simple** HUD enhancements, an optional flag return feature, optional flagholder frag bonus, flagholder on scoreboard, \
added visual/audio notifications and many bugfixes was the final outcome.

The changes subtly alter the game dynamics and improve on the original zigmode game element, IMHO. \
The original gameplay can still be enabled by disabling the new elements via cvars.

I don't have a Windows development environment to compile a `.dll`, but paths for config, maplist and chaining files have \
been made unixcentric. `git grep BASEPATH` will provide concerned locations. I am happy to include a `.dll` if anyone \
stumbles across this project and can compile.

`ZigMode` requires the included `.pak` file and a route chaining file for the map, many popular maps are \
included, further `.chn` can be created via the mod `chedit` function.

`Capture and Hold` plays best on smaller level maps with a couple of bots throw in.

Example config file for ZigFlag:

```
set zigmode 1
set zigspawn 1
set zigkiller 1
set ctf 0
set aimfix 1
set spawnbotfar 1
set botlist default
set autospawn 3
set vwep 1
set maxclients 16
set basepath "/usr/share/games/quake2"
set dmflags 16384
set fraglimit 30
set timelimit 10
set maplist q2dmx
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

### Spawn bots at the farthest point

    spawnbotfar $

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
    zigkiller $

## Errata

Specify a `basedir` to inform the mod of the system `baseq2` directory location. \
On system level installs this may be needed to ensure the mod can find`3zbconfig.cfg,` \
`3zbmaps.lst` and `chaining` files. The basedir can be specified in config via:

    set basepath "/usr/share/games/quake2"

Use on the following errors:

    3ZB CFG: file not found: ./3zb2/3zbconfig.cfg
    Chaining: file 3zb2/chdtm/q2dm1.chn not found.

