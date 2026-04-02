# 3ZB2 Configuration Guide

## File Structure

| File | Description |
|------|-------------|
| `3zb2/3zbconfig.cfg` | Configuration file for bot information and server messages |
| `3zb2/3zbmaps.lst` | Map list file |
| `3zb2/chdtm/*.chn` | Deathmatch route chain files |
| `3zb2/chctf/*.chf` | Capture The Flag route chain files |

Route chain files (`.chn`/`.chf`) contain object and level navigation data. See [Creating Route Chains](#creating-route-chains) for details.

## Notes

- **Map Navigation**: Place route chain files (`.chn`) in `3zb2/chdtm/` for proper bot navigation. For example, `q2dm1` requires `3zb2/chdtm/q2dm1.chn`.
- **Mission Pack 1 Support**: To play on Reckoning maps, copy `xatrix/pak0.pak` to `3zb2/`.
- **CTF Support**: Copy `ctf/pak*.pak` to `3zb2/`. Rename existing pak files to avoid conflicts (e.g., `pakX.pak` where `X` is a new number).

## Console Variables

### Requires Restart

| Variable | Description | Default |
|----------|-------------|---------|
| `vwep` | Visible weapons | `1` (on) |
| `maplist` | Map list section name | `"default"` |
| `autospawn` | Number of bots to auto-spawn | `0` |
| `autobot` | Manage bots based on real client count | `0` |
| `chedit` | Chaining (route editing) mode | `0` (off) |
| `zigmode` | Capture and Hold (ZigFlag) mode | `0` (off) |
| `zigspawn` | Return ZigFlag to spawn after 60s | `1` (on) |
| `zigkiller` | Bonus frag for killing flag holder | `1` (on) |
| `zigrapple` | Add CTF grapple to non-CTF games | `0` (off) |
| `zigintro` | Spectator mode on game start | `0` (off) |
| `botlist` | Bot list section name | `"default"` |
| `ctf` | Capture The Flag mode | `0` (off) |
| `tdm` | Team Deathmatch mode | `0` (off) |
| `aimfix` | More accurate bot aiming | `1` (on) |
| `combathud` | Extra HUD information | `1` (on) |
| `respawn_protection` | Brief immunity on respawn | `0` (off) |
| `spawnbotfar` | Spawn bots at farthest points | `0` (off) |
| `killerflag` | Flag drains holder health | `1` (on) |
| `fixflaws` | Fix known Quake 2 gameplay flaws | `1` (on) |
| `playerid` | Identify player in crosshair | `0` (off) |
| `weaponswap` | Auto-switch to better weapon on pickup | `0` (off) |

### Live-Updated

| Variable | Description | Default |
|----------|-------------|---------|
| `spectator` | Spectator mode | `0` (off) |

## Server Commands

| Command | Description |
|---------|-------------|
| `sv spb <n>` | Spawn `n` bots |
| `sv rspb <n>` | Spawn `n` random bots from list |
| `sv rmb <n>` | Remove `n` bots |
| `sv dsp <n>` | Debug spawn: bot runs `n` pods from old position |
| `sv savechain` | Save chaining route file |

## Client Commands

| Command | Description |
|---------|-------------|
| `cmd undo <n>` | Move back `n` pod positions |

## Creating Route Chains

### Prerequisites

- A writable route chain file in the correct directory. Create an empty file if starting fresh:
  ```bash
  touch 3zb2/chdtm/q2dm8.chn
  touch 3zb2/chctf/q2ctf3.chf
  ```

### Steps

1. Enable chaining mode:
   ```
   chedit 1
   ```
2. Load the map:
   ```
   map q2dm8
   ```
3. Roam the map. If using elevators with buttons, verify bot navigation works.
4. Debug spawn tests (optional):
   ```
   sv dsp 20
   ```
   - Bot is removed if it reaches the current pod successfully.
   - Bot is removed if it fails to trace the route.
5. Undo pod positions if needed:
   ```
   cmd undo 20
   ```
6. Save the route:
   ```
   sv savechain
   ```

### Tips

- Avoid rocket jumps and grenade jumps in routes.
- Deaths (lava, slime, etc.) auto-correct routes to safe paths.
- Manually remove stuck bots: `sv rmb 1`

## Map List

Define map rotation patterns in `3zbmaps.lst`.

### Format

```ini
[section_name]
mapname1
mapname2
mapname3
```

### Usage

1. Create a section in `3zbmaps.lst`:
   ```ini
   [list1]
   q2dm1
   q2dm2
   q2dm3
   ```
2. Set the section name:
   ```
   maplist list1
   ```
3. Start a match on any map in the list.

## Configuration (`3zbconfig.cfg`)

### Message Sections

| Section | Mode |
|---------|------|
| `[MessDeathMatch]` | Deathmatch |
| `[MessChainDM]` | Deathmatch chaining |
| `[MessCTF]` | CTF |
| `[MessChainCTF]` | CTF chaining |

### Bot List Section

`[BotList]` defines available bots (max 128). Format:

```
\\netname\model\skin\param0-15\team\autospawnflag
```

| Field | Description | Values |
|-------|-------------|--------|
| `netname` | Bot name | String |
| `model` | Model name | String |
| `skin` | Skin name | String |
| `param0` | Walking mode | `0` = always run, `1` = walk |
| `param1` | Aiming skill | `0`–`9` (higher = harder) |
| `param2` | Item pickup frequency | `0`–`9` (higher = more aggressive) |
| `param3` | Character type | `0`–`9` (high = offensive, low = defensive) |
| `param4` | Combat skill | `0`–`9` (higher = better) |
| `param5` | Rocket jump | `0` = off, `1` = on |
| `param6` | Reaction skill | `0`–`9` (higher = faster) |
| `param7` | Vertical view range | `10`–`180` degrees |
| `param8` | Horizontal view range | `10`–`180` degrees |
| `param9` | Primary weapon | `0`–`13` |
| `param10` | Secondary weapon | `0`–`13` |
| `param11` | Dodging (depends on combat skill) | `0` = off, `1` = on |
| `param12` | Estimation (requires param13) | `0` = off, `1` = on |
| `param13` | Enemy noise detection | `0` = off, `1` = on |
| `param14` | Cannot see through water | `0` = off, `1` = on |
| `param15` | Teamwork | `0`–`9` (higher = better) |
| `team` | Team (CTF only) | `R` or `B` |
| `autospawnflag` | Auto-spawn this bot | `0` or `1` |

### Weapon IDs

| ID | Weapon |
|----|--------|
| 1 | Blaster |
| 2 | Shotgun |
| 3 | Super Shotgun |
| 4 | Machine Gun |
| 5 | Chaingun |
| 6 | Hand Grenade |
| 7 | Grenade Launcher |
| 8 | Rocket Launcher |
| 9 | Hyperblaster |
| 10 | Railgun |
| 11 | BFG10K |
| 12 | Plasma Gun |
| 13 | Ion Ripper |

### Custom Bot List

1. Add a new section in `3zbconfig.cfg`:
   ```ini
   [MyBotList]
   ```
2. Set the section name:
   ```
   botlist mybotlist
   ```

## Autospawn

Automatically spawn bots on match start.

1. Set the number of bots:
   ```
   autospawn 4
   ```
2. Start a match. Bots spawn automatically.

**Note**: Bots must exist in `[BotList]` and have `autospawnflag` set to `1`.

## Teamplay and Capture the Hold

### Teamplay Deathmatch Flags

| Flag | Value |
|------|-------|
| `DF_SKINTEAMS` | `64` |
| `DF_MODELTEAMS` | `128` |
| `DF_NO_FRIENDLY_FIRE` | `256` |

### Capture and Hold (ZigFlag)

1. Enable the mode:
   ```
   zigmode 1
   ```
2. Optionally enable flag return:
   ```
   zigspawn 1
   ```

**Rules**:
- Works in Deathmatch and Teamplay only.
- The Zigock Flag spawns on the map.
- The holder gains 1 bonus frag every 10 seconds.
- In Teamplay, teammates of the holder also earn points.
