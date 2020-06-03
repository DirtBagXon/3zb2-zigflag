# ZigFlag - Custom 3rd Zigock Bot II for Quake II

This is a custom port of the 3rd Zigock Bot II to Quake II - Yamagi Quake II is recommended.  \
All warnings (up to GCC9) and unused variables have been addressed in the original source. \
The code also has handpicked backport fixes, enhancements and features applied from various \
sources: `tastyspleen`, `yquake2`, `OpenTDM`, `OpenFFA` and custom.

This was modified for my own use and driven by nostalgia for the Quake II servers of the 90's. \
There are many heavily modified versions of the Quake II engine, this mod tries to keep the look and feel of \
the original game deathmatch, but allows a multiplayer experience with some of the best bots for the Quake II\
engine. I couldn't locate any `Capture and Hold` servers on [q2servers](http://q2servers.com), so this offered the opportunity to rekindle \
a firm favourite via the `zigmode` modification.

Tip of the hat to `Ponpoko`, original mod author and bot creator.

On global linux installs, **e.g.** `/usr/share/games`, you may need to specify `basepath` - See details below.

Bot chaining routes are supplied, further routes can be (re)created via the mod command `chedit` (See `CONFIG.txt`)

### ZigMode ZigFlag (Capture and Hold)

The premise is simple: **Get the flag and keep it** - *plays on standard Deathmatch maps*.

The original `zigmode` was released belated, buggy and only half implemented, I attempted to make this feature a little \
more refined, just for fun. I was trying to keep the look and feel of the original deathmatch, but with a few bells and whistles.

* Simple HUD enhancements.
* Optional OpenTDM gameplay fixes.
* Optional Flag respawn feature.
* Optional Flagholder frag bonus.
* Optional Flag draws health from holder.
* Optional respawn protection.
* Spawn bots at a distance.
* Autospawn bots at level start.
* Visual/audio notifications to Flagholder.
* HUD Identification of player in the crosshair.
* Flagholder dogtag displayed on scoreboard.

..and many bugfixes was the final outcome of playing around with the code.

The mod also supports skin and model teams with appropriate bonuses and penalties on Flag possession and `FRIENDLY_FIRE`.

The changes subtly alter the game dynamics and improve on the original zigmode game element, IMHO. \
The original gameplay, with bugfixes, can still be enabled by disabling the new elements via cvars.

`Capture and Hold` plays best on smaller level maps with a couple of bots throw in.

### Pak file (Flag model) and Route Chaining files

`ZigMode` requires the included small `.pak` file, for the flag model, and route chaining files for the maps. \
Many popular maps are included, further route chaining `.chn` files can be created via the mod `chedit` function.

#### Example config file for ZigFlag:

```
exec addbot.cfg
set zigmode 1
set zigspawn 1
set zigkiller 1
set ctf 0
set aimfix 1
set spawnbotfar 1
set killerflag 1
set fixflaws 1
set playerid 1
set playerid_alt 0
set botlist default
set autospawn 3
set vwep 1
set maxclients 16
set respawn_protection 1
//set basepath "/usr/share/games/quake2"   <- Check requirement in Windows
set dmflags 16384
set fraglimit 30
set timelimit 10
set maplist q2dmx
map q2dm1
```

See `CONFIG.txt` for further details.

## Windows Installation

A Windows `game.dll` file has been provided in the `windows` subdirectory. Paths for config, maplist and chaining files have \
been made unixoid in the main source. `git grep BASEPATH` will provide concerned locations for future compilations.  I have \
also provided a small patch file to switch these paths with ease. This is also included in the `windows` subdirectory.

Install the `.dll` in the *3zb2* folder:

    copy .\windows\gamex86.dll .\3zb2\game.dll
    copy .\windows\gamex86.dll .\3zb2\gamex86.dll

Move the *3zb2* directory in the main Quake II installation folder, alongside *baseq2*.

    move 3zb2 C:\Program Files\Quake2\

Adjust the `zig.cfg` (see above) in the *3zb2* directory if required, then create a `.bat` file using the below arguments as a minimum:

    quake2.exe +set game 3zb2 +set deathmatch 1 +exec zig.cfg

**Note:** `basepath` directive may **NOT** be required in Windows Quake II configuration files.

## Linux Installation

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

    aimfix 1

Fix noted Quake 2 gameplay flaws (opentdm), enable `1` (default) or disable `0` via:

    fixflaws 1

Flag takes health from a subdued holder, anti-camping, enable `1` or disable `0` (default):

    killerflag 1

**Stay-in-the-fray** to avoid penalty.

Identify player in the crosshair, enable `1` or disable `0` (default) \
Additional cvar for alternate positioning of the player_id label: `playerid_alt`:

    playerid 1
    playerid_alt 1

`Capture and Hold (ZigFlag)` mode for Deathmatch/Team games:

    zigmode 1
    zigspawn 1
    zigkiller 1

## Errata

Specify a `basedir` to inform the mod of the system `baseq2` directory location. \
On system level installs this may be needed to ensure the mod can find`3zbconfig.cfg,` \
`3zbmaps.lst` and `chaining` files. The basedir can be specified in config via:

    set basepath "/usr/share/games/quake2"

Use on the following errors:

    3ZB CFG: file not found: ./3zb2/3zbconfig.cfg
    Chaining: file 3zb2/chdtm/q2dm1.chn not found.

The mod has a random issues using `gamemap`, resulting in a lockup or segfault. Bot tracing in \
`SV_RunThink()` seems to be the related area, but I have yet to track down. Use the `map` command which \
causes a full level reset to overcome the issue. Note: **Q2Pro** attempts to enforce the use of \
`gamemap`, use `sv_allow_map 1` in Q2Pro to overcome this:

    map q2dm1


## 獄

![captureandhold](https://raw.githubusercontent.com/DirtBagXon/3zb2/master/screenshot.png)
