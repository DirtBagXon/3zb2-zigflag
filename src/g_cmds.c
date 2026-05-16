#include "header/local.h"
#include "header/player.h"
#include "header/bot.h"

// ---------------------------------------------------------------------------
// Cached stats snapshot: saved at intermission so 'stats <name>' still works
// after bots have disconnected.
// ---------------------------------------------------------------------------
#define MAX_CACHED_STATS 64

typedef struct {
	char		name[32];
	fragstat_t	frags[FRAG_TOTAL];
	int		damage_given;
	int		damage_recvd;
} cached_stat_entry_t;

static cached_stat_entry_t cached_stats[MAX_CACHED_STATS];
static int	num_cached_stats = 0;

void SaveStatsSnapshot(void)
{
	int i, j;
	num_cached_stats = 0;
	for (i = 1; i <= game.maxclients && num_cached_stats < MAX_CACHED_STATS; i++)
	{
		edict_t *cl = &g_edicts[i];
		if (!cl->inuse || !cl->client) continue;
		if (!cl->client->pers.netname[0]) continue;	// skip empty slots
		cached_stat_entry_t *e = &cached_stats[num_cached_stats++];
		strncpy(e->name, cl->client->pers.netname, sizeof(e->name) - 1);
		e->name[sizeof(e->name) - 1] = '\0';
		for (j = 0; j < FRAG_TOTAL; j++) e->frags[j] = cl->client->resp.frags[j];
		e->damage_given = cl->client->resp.damage_given;
		e->damage_recvd = cl->client->resp.damage_recvd;
		e->frags[FLAG_EVENT].possession = cl->client->resp.possession;
		e->frags[FLAG_EVENT].assassin = cl->client->resp.assassin;

	}
}

void ClearStatsCache(void)
{
	num_cached_stats = 0;
}

char *ClientTeam (edict_t *ent)
{
	char *p;
	static char value[512];
	const char *skin;

	value[0] = '\0';

	if (!ent || !ent->client)
		return value;

	skin = Info_ValueForKey(ent->client->pers.userinfo, "skin");
	if (!skin)
		return value;

	snprintf(value, sizeof(value), "%s", skin);

	p = strchr(value, '/');
	if (!p)
		return value;

	if ((int)(dmflags->value) & DF_MODELTEAMS)
	{
		*p = '\0';
		return value;
	}

	return p + 1;
}

qboolean OnSameTeam (edict_t *ent1, edict_t *ent2)
{
	char	ent1Team [512];
	char	ent2Team [512];

	if (ctf->value && ent1->client && ent2->client)
	{
		if (ent1->client->resp.ctf_team >= CTF_TEAM1 &&
			ent1->client->resp.ctf_team == ent2->client->resp.ctf_team)
			return qtrue;
	}

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
		return qfalse;

	strlcpy(ent1Team, ClientTeam (ent1), sizeof(ent1Team));
	strlcpy(ent2Team, ClientTeam (ent2), sizeof(ent2Team));

	if (strcmp(ent1Team, ent2Team) == 0)
		return qtrue;

	return qfalse;
}


void SelectNextItem (edict_t *ent, int itflags)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;

	cl = ent->client;

//ZOID
	if (cl->menu) {
		PMenu_Next(ent);
		return;
	} else if (cl->chase_target) {
		ChaseNext(ent);
		return;
	}
//ZOID

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (cl->pers.selected_item + i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (!(it->flags & itflags))
			continue;

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void SelectPrevItem (edict_t *ent, int itflags)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;

	cl = ent->client;

//ZOID
	if (cl->menu) {
		PMenu_Prev(ent);
		return;
	} else if (cl->chase_target) {
		ChasePrev(ent);
		return;
	}
//ZOID

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (cl->pers.selected_item + MAX_ITEMS - i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (!(it->flags & itflags))
			continue;

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void ValidateSelectedItem (edict_t *ent)
{
	gclient_t	*cl;

	cl = ent->client;

	if (cl->pers.inventory[cl->pers.selected_item])
		return;		// valid

	SelectNextItem (ent, -1);
}


//=================================================================================

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f (edict_t *ent)
{
	char		*name;
	gitem_t		*it;
	int			index;
	int			i;
	qboolean	give_all;
	edict_t		*it_ent;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	name = gi.args();

	if (Q_stricmp(name, "all") == 0)
		give_all = qtrue;
	else
		give_all = qfalse;

	if (give_all || Q_stricmp(gi.argv(1), "health") == 0)
	{
		if (gi.argc() == 3) {
			ent->health = atoi(gi.argv(2));
			ent->health = ent->health < 1 ? 1 : ent->health;
		}
		else
		{
			ent->health = ent->max_health;
		}

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		for (i=0 ; i<game.num_items ; i++)
		{
			it = itemlist + i;
			if (!it->pickup)
				continue;
			if (!(it->flags & IT_WEAPON))
				continue;
			ent->client->pers.inventory[i] += 1;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0)
	{
		for (i=0 ; i<game.num_items ; i++)
		{
			it = itemlist + i;
			if (!it->pickup)
				continue;
			if (!(it->flags & IT_AMMO))
				continue;
			Add_Ammo (ent, it, 1000);
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "armor") == 0)
	{
		gitem_armor_t	*info;

		it = FindItem("Jacket Armor");
		ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

		it = FindItem("Combat Armor");
		ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

		it = FindItem("Body Armor");
		info = (gitem_armor_t *)it->info;
		ent->client->pers.inventory[ITEM_INDEX(it)] = info->max_count;

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "Power Shield") == 0)
	{
		it = FindItem("Power Shield");
		it_ent = G_Spawn();
		it_ent->classname = it->classname;
		SpawnItem (it_ent, it);
		Touch_Item (it_ent, ent, NULL, NULL);
		if (it_ent->inuse)
			G_FreeEdict(it_ent);

		if (!give_all)
			return;
	}

	if (give_all)
	{
		for (i=0 ; i<game.num_items ; i++)
		{
			it = itemlist + i;
			if (!it->pickup)
				continue;
			if (it->flags & (IT_ARMOR|IT_WEAPON|IT_AMMO))
				continue;
			ent->client->pers.inventory[i] = 1;
		}
		return;
	}

	it = FindItem (name);
	if (!it)
	{
		name = gi.argv(1);
		it = FindItem (name);
		if (!it)
		{
			gi.dprintf ("unknown item\n");
			return;
		}
	}

	if (!it->pickup)
	{
		gi.dprintf ("non-pickup item\n");
		return;
	}

	index = ITEM_INDEX(it);

	if (it->flags & IT_AMMO)
	{
		if (gi.argc() == 3)
			ent->client->pers.inventory[index] = atoi(gi.argv(2));
		else
			ent->client->pers.inventory[index] += it->quantity;
	}
	else
	{
		it_ent = G_Spawn();
		it_ent->classname = it->classname;
		SpawnItem (it_ent, it);
		if (it_ent->inuse)
		{
			Touch_Item (it_ent, ent, NULL, NULL);
			if (it_ent->inuse) G_FreeEdict(it_ent);
		}
	}
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (edict_t *ent)
{
	char	*msg;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f (edict_t *ent)
{
	char	*msg;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f (edict_t *ent)
{
	char	*msg;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (ent->movetype == MOVETYPE_NOCLIP)
	{
		ent->movetype = MOVETYPE_WALK;
		msg = "noclip OFF\n";
	}
	else
	{
		ent->movetype = MOVETYPE_NOCLIP;
		msg = "noclip ON\n";
	}

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Use_f

Use an inventory item
==================
*/
void Cmd_Use_f (edict_t *ent)
{
	int			index;
	gitem_t		*it;
	char		*s;

	s = gi.args();
	it = FindItem (s);
	if (!it)
	{
		gi.cprintf (ent, PRINT_HIGH, "unknown item: %s\n", s);
		return;
	}
	if (!it->use)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	index = ITEM_INDEX(it);
	if (!ent->client->pers.inventory[index])
	{
		// RAFAEL
		if (strcmp (it->pickup_name, "HyperBlaster") == 0)
		{
			it = Fdi_BOOMER;//FindItem ("Ionripper");
			index = ITEM_INDEX (it);
			if (!ent->client->pers.inventory[index])
			{
				gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
				return;
			}
		}
		// RAFAEL
		else if (strcmp (it->pickup_name, "Railgun") == 0)
		{
			it = Fdi_PHALANX;//FindItem ("Phalanx");
			index = ITEM_INDEX (it);
			if (!ent->client->pers.inventory[index])
			{
				gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
				return;
			}
		}
		else
		{
			gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
			return;
		}
	}

	it->use (ent, it);
}


/*
==================
Cmd_Drop_f

Drop an inventory item
==================
*/
void Cmd_Drop_f (edict_t *ent)
{
	int			index;
	gitem_t		*it;
	char		*s;

//ZOID--special case for tech powerups
	if (Q_stricmp(gi.args(), "tech") == 0 && (it = CTFWhat_Tech(ent)) != NULL) {
		it->drop (ent, it);
		return;
	}
//ZOID

	s = gi.args();
	it = FindItem (s);
	if (!it)
	{
		gi.cprintf (ent, PRINT_HIGH, "unknown item: %s\n", s);
		return;
	}
	if (instagib && instagib->value && !Q_stricmp(it->pickup_name, "Slugs"))
	{
		gi.cprintf(ent, PRINT_HIGH, "Can't drop slugs in instagib mode\n");
		return;
	}
	if (!it->drop)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	index = ITEM_INDEX(it);
	if (!ent->client->pers.inventory[index])
	{
		// RAFAEL
		if (strcmp (it->pickup_name, "HyperBlaster") == 0)
		{
			it = Fdi_BOOMER;//FindItem ("Ionripper");
			index = ITEM_INDEX (it);
			if (!ent->client->pers.inventory[index])
			{
				gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
				return;
			}
		}
		// RAFAEL
		else if (strcmp (it->pickup_name, "Railgun") == 0)
		{
			it = Fdi_PHALANX;//FindItem ("Phalanx");
			index = ITEM_INDEX (it);
			if (!ent->client->pers.inventory[index])
			{
				gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
				return;
			}
		}
		else
		{
			gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
			return;
		}
	}

	it->drop (ent, it);
}


/*
=================
Cmd_Inven_f
=================
*/
void Cmd_Inven_f (edict_t *ent)
{
	int			i;
	gclient_t	*cl;

	if(ent->svflags & SVF_MONSTER) return;

	cl = ent->client;

	cl->showscores = qfalse;
	cl->showhelp = qfalse;

//ZOID
	if (ent->client->menu) {
		PMenu_Close(ent);
		ent->client->update_chase = qtrue;
		return;
	}
//ZOID

	if (cl->showinventory)
	{
		cl->showinventory = qfalse;
		return;
	}

//ZOID
	if (ctf->value && cl->resp.ctf_team == CTF_NOTEAM) {
		CTFOpenJoinMenu(ent);
		return;
	}
//ZOID

	cl->showinventory = qtrue;

	gi.WriteByte (svc_inventory);
	for (i=0 ; i<MAX_ITEMS ; i++)
	{
		gi.WriteShort (cl->pers.inventory[i]);
	}
	gi.unicast (ent, qtrue);
}

/*
=================
Cmd_InvUse_f
=================
*/
void Cmd_InvUse_f (edict_t *ent)
{
	gitem_t		*it;

	if (ent->client->resp.spectator && !ent->client->pers.joined) {
		ent->client->pers.joined = qtrue;
		spectator_respawn(ent);
		return;
	}

//ZOID
	if (ent->client->menu) {
		PMenu_Select(ent);
		return;
	}
//ZOID

	ValidateSelectedItem (ent);

	if (ent->client->pers.selected_item == -1)
	{
		gi.cprintf (ent, PRINT_HIGH, "No item to use.\n");
		return;
	}

	it = &itemlist[ent->client->pers.selected_item];
	if (!it->use)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	it->use (ent, it);
}

//ZOID
/*
=================
Cmd_LastWeap_f
=================
*/
void Cmd_LastWeap_f (edict_t *ent)
{
	gclient_t	*cl;

	cl = ent->client;

	if (!cl->pers.weapon || !cl->pers.lastweapon)
		return;

	cl->pers.lastweapon->use (ent, cl->pers.lastweapon);
}
//ZOID


/*
=================
Cmd_WeapPrev_f
=================
*/
void Cmd_WeapPrev_f (edict_t *ent)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;
	int			selected_weapon;

	cl = ent->client;

	if (!cl->pers.weapon)
		return;

	selected_weapon = ITEM_INDEX(cl->pers.weapon);

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (selected_weapon + i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (! (it->flags & IT_WEAPON) )
			continue;
		it->use (ent, it);
		if (cl->pers.weapon == it)
			return;	// successful
	}
}

/*
=================
Cmd_WeapNext_f
=================
*/
void Cmd_WeapNext_f (edict_t *ent)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;
	int			selected_weapon;

	cl = ent->client;

	if (!cl->pers.weapon)
		return;

	selected_weapon = ITEM_INDEX(cl->pers.weapon);

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (selected_weapon + MAX_ITEMS - i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (! (it->flags & IT_WEAPON) )
			continue;
		it->use (ent, it);
		if (cl->pers.weapon == it)
			return;	// successful
	}
}

/*
=================
Cmd_WeapLast_f
=================
*/
void Cmd_WeapLast_f (edict_t *ent)
{
	gclient_t	*cl;
	int			index;
	gitem_t		*it;

	cl = ent->client;

	if (!cl->pers.weapon || !cl->pers.lastweapon)
		return;

	index = ITEM_INDEX(cl->pers.lastweapon);
	if (!cl->pers.inventory[index])
		return;
	it = &itemlist[index];
	if (!it->use)
		return;
	if (! (it->flags & IT_WEAPON) )
		return;
	it->use (ent, it);
}

/*
=================
Cmd_InvDrop_f
=================
*/
void Cmd_InvDrop_f (edict_t *ent)
{
	gitem_t		*it;

	ValidateSelectedItem (ent);

	if (ent->client->pers.selected_item == -1)
	{
		gi.cprintf (ent, PRINT_HIGH, "No item to drop.\n");
		return;
	}

	it = &itemlist[ent->client->pers.selected_item];
	if (!it->drop)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	it->drop (ent, it);
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f (edict_t *ent)
{
//ZOID
	if (ent->solid == SOLID_NOT)
		return;
//ZOID

	if((level.time - ent->client->respawn_time) < 5)
		return;
	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die (ent, ent, ent, 100000, vec3_origin);
	// don't even bother waiting for death frames
	ent->deadflag = DEAD_DEAD;
	respawn (ent);
}

/*
=================
Cmd_PutAway_f
=================
*/
void Cmd_PutAway_f (edict_t *ent)
{
	ent->client->showscores = qfalse;
	ent->client->showhelp = qfalse;
	ent->client->showinventory = qfalse;
//ZOID
	if (ent->client->menu)
		PMenu_Close(ent);
	ent->client->update_chase = qtrue;
//ZOID
}


int PlayerSort (void const *a, void const *b)
{
	int		anum, bnum;

	anum = *(int *)a;
	bnum = *(int *)b;

	anum = game.clients[anum].ps.stats[STAT_FRAGS];
	bnum = game.clients[bnum].ps.stats[STAT_FRAGS];

	if (anum < bnum)
		return -1;
	if (anum > bnum)
		return 1;
	return 0;
}

/*
=================
Cmd_Players_f
=================
*/
void Cmd_Players_f (edict_t *ent)
{
	int		i;
	int		count;
	char	small[64];
	char	large[1280];
	int		index[256];

	count = 0;
	for (i = 0 ; i < maxclients->value ; i++)
		if (game.clients[i].pers.connected)
		{
			index[count] = i;
			count++;
		}

	// sort by frags
	qsort (index, count, sizeof(index[0]), PlayerSort);

	// print information
	large[0] = 0;

	for (i = 0 ; i < count ; i++)
	{
		Com_sprintf (small, sizeof(small), "%3i %s\n",
			game.clients[index[i]].ps.stats[STAT_FRAGS],
			game.clients[index[i]].pers.netname);
		if (strlen (small) + strlen(large) > sizeof(large) - 100 )
		{	// can't print all of them in one packet
			strcat (large, "...\n");
			break;
		}
		strcat (large, small);
	}

	gi.cprintf (ent, PRINT_HIGH, "%s\n%i players\n", large, count);
}

/*
=================
Cmd_Wave_f
=================
*/
void Cmd_Wave_f (edict_t *ent)
{
	int		i;

	i = atoi (gi.argv(1));

	// can't wave when ducked
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
		return;

	if (ent->client->anim_priority > ANIM_WAVE)
		return;

	ent->client->anim_priority = ANIM_WAVE;

	switch (i)
	{
	case 0:
		gi.cprintf (ent, PRINT_HIGH, "flipoff\n");
		ent->s.frame = FRAME_flip01-1;
		ent->client->anim_end = FRAME_flip12;
		break;
	case 1:
		gi.cprintf (ent, PRINT_HIGH, "salute\n");
		ent->s.frame = FRAME_salute01-1;
		ent->client->anim_end = FRAME_salute11;
		break;
	case 2:
		gi.cprintf (ent, PRINT_HIGH, "taunt\n");
		ent->s.frame = FRAME_taunt01-1;
		ent->client->anim_end = FRAME_taunt17;
		break;
	case 3:
		gi.cprintf (ent, PRINT_HIGH, "wave\n");
		ent->s.frame = FRAME_wave01-1;
		ent->client->anim_end = FRAME_wave11;
		break;
	case 4:
	default:
		gi.cprintf (ent, PRINT_HIGH, "point\n");
		ent->s.frame = FRAME_point01-1;
		ent->client->anim_end = FRAME_point12;
		break;
	}
}

/*
==================
Cmd_Say_f
==================
*/
void Cmd_Say_f (edict_t *ent, qboolean team, qboolean arg0)
{
	int		j;
	edict_t	*other;
	char	*p;
	char	text[2048];

	if (gi.argc () < 2 && !arg0)
		return;

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
		team = qfalse;

	if (team)
		Com_sprintf (text, sizeof(text), "(%s): ", ent->client->pers.netname);
	else
		Com_sprintf (text, sizeof(text), "%s: ", ent->client->pers.netname);

	if (arg0)
	{
		strcat (text, gi.argv(0));
		strcat (text, " ");
		strcat (text, gi.args());
	}
	else
	{
		p = gi.args();

		if (*p == '"')
		{
			p++;
			p[strlen(p)-1] = 0;
		}
		strcat(text, p);
	}

	// don't let text be too long for malicious reasons
	if (strlen(text) > 150)
		text[150] = 0;

	strcat(text, "\n");

	if (dedicated->value)
		gi.cprintf(NULL, PRINT_CHAT, "%s", text);

	for (j = 1; j <= game.maxclients; j++)
	{
		other = &g_edicts[j];
		if (!other->inuse)
			continue;
		if (!other->client)
			continue;
		if (team)
		{
			if (!OnSameTeam(ent, other))
				continue;
		}
		if (other->svflags & SVF_MONSTER) continue;

		gi.cprintf(other, PRINT_CHAT, "%s", text);
	}
}
//スナイパー用ZoomIn Out
void Cmd_ZoomIn(edict_t *ent)
{
	if( ent->client->zc.autozoom )
	{
		gi.cprintf(ent,PRINT_HIGH,"autozoom has been selected.\n");
		return;
	}

//	if(	ent->client->pers.weapon != FindItem("Railgun")) return;

	if( ent->client->zc.aiming != 1 && ent->client->zc.aiming != 3) return;

	if(ent->client->zc.distance < 15 || ent->client->zc.distance > 90)
	{
		ent->client->zc.distance = 90;
		ent->client->ps.fov = 90;
	}

	if(ent->client->zc.distance > 15)
	{
		gi.sound (ent, CHAN_AUTO, gi.soundindex("3zb/zoom.wav"), 1, ATTN_NORM, 0);
		if(ent->client->zc.distance == 90 ) ent->client->zc.distance = 65;
		else if(ent->client->zc.distance == 65 ) ent->client->zc.distance = 40;
		else ent->client->zc.distance = 15;
		ent->client->ps.fov = ent->client->zc.distance;
	}
}
void Cmd_ZoomOut(edict_t *ent)
{
	if( ent->client->zc.autozoom )
	{
		gi.cprintf(ent,PRINT_HIGH,"autozoom has been selected.\n");
		return;
	}

//	if(	ent->client->pers.weapon != FindItem("Railgun")) return;

	if(ent->client->zc.aiming != 1 && ent->client->zc.aiming != 3) return;

	if(ent->client->zc.distance < 15 || ent->client->zc.distance > 90)
	{
		ent->client->zc.distance = 90;
		ent->client->ps.fov = 90;
	}

	if(ent->client->zc.distance < 90)
	{
		gi.sound (ent, CHAN_AUTO, gi.soundindex("3zb/zoom.wav"), 1, ATTN_NORM, 0);
		if(ent->client->zc.distance == 15 ) ent->client->zc.distance = 40;
		else if(ent->client->zc.distance == 40 ) ent->client->zc.distance = 65;
		else ent->client->zc.distance = 90;
		ent->client->ps.fov = ent->client->zc.distance;
	}
}

void Cmd_AutoZoom(edict_t *ent)
{
	if( ent->client->zc.autozoom )
	{
		gi.cprintf(ent,PRINT_HIGH,"autozoom off.\n");
		ent->client->zc.autozoom = qfalse;
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"autozoom on.\n");
		ent->client->zc.autozoom = qtrue;
	}
}

//chain の undo
void UndoChain(edict_t *ent ,int step)
{
	int	count,i;
	trace_t	rs_trace;

	if(step < 2) count = 2;
	else count = step;

	if(chedit->value && !ent->deadflag && ent == &g_edicts[1])
	{
		for(i = CurrentIndex - 1;i > 0 ;i--)
		{
			if(Route[i].state == GRS_NORMAL)
			{
				rs_trace = gi.trace(Route[i].Pt,ent->mins,ent->maxs,Route[i].Pt,ent,MASK_BOTSOLID);

				if(--count <= 0 && !rs_trace.allsolid && !rs_trace.startsolid) break;
			}
		}

		gi.cprintf(ent,PRINT_HIGH,"backed %i %i steps.\n",CurrentIndex - i,step);
		CurrentIndex = i;
		VectorCopy(Route[CurrentIndex].Pt,ent->s.origin);
		VectorCopy(Route[CurrentIndex].Pt,ent->s.old_origin);

		memset(&Route[CurrentIndex],0,sizeof(route_t));
		if(CurrentIndex > 0) Route[CurrentIndex].index = Route[CurrentIndex - 1].index + 1;
	}
}

/*
=================
Cmd_Stats_f
=================
*/
void Cmd_Stats_f(edict_t *ent, qboolean check_other)
{
	static const char *weapon_names[] = {
		"Unknown", "Blaster", "Shotgun", "Super Shotgun", "Machinegun",
		"Chaingun", "Grenades", "Grenade Launcher", "Rocket Launcher",
		"HyperBlaster", "Railgun", "BFG10K"
	};
	int i, sum_atts = 0, sum_hits = 0, sum_kills = 0, sum_deaths = 0, sum_suicides = 0;
	int acc, sum_acc;
	fragstat_t *frags;
	edict_t *target = ent;

	if (check_other && gi.argc() > 1)
	{
		char *name = gi.argv(1);
		edict_t *found = NULL;

		// Exact match
		for (i = 1; i <= game.maxclients; i++)
		{
			edict_t *cl_ent = &g_edicts[i];
			if (!cl_ent->inuse || !cl_ent->client) continue;
			if (Q_stricmp(cl_ent->client->pers.netname, name) == 0)
			{
				found = cl_ent;
				break;
			}
		}

		// Partial match
		if (!found)
		{
			for (i = 1; i <= game.maxclients; i++)
			{
				edict_t *cl_ent = &g_edicts[i];
				if (!cl_ent->inuse || !cl_ent->client) continue;
				if (Q_strncasecmp(cl_ent->client->pers.netname, name, strlen(name)) == 0)
				{
					found = cl_ent;
					break;
				}
			}
		}

		if (!found)
		{
			// Not currently connected -- check snapshot cache from last match
			for (i = 0; i < num_cached_stats; i++)
			{
				if (Q_stricmp(cached_stats[i].name, name) == 0)
				{
					// Print directly from cache and return
					cached_stat_entry_t *e = &cached_stats[i];
					gi.cprintf(ent, PRINT_HIGH, "\nAccuracy stats for %s (last match):\n", e->name);
					gi.cprintf(ent, PRINT_HIGH, "Weapon             Acc   Hit/Atts   Kills Deaths Suic\n");
					gi.cprintf(ent, PRINT_HIGH, "-----------------------------------------------------\n");
					int s_atts = 0, s_hits = 0, s_kills = 0, s_deaths = 0, s_suicides = 0, s_acc;
					for (i = FRAG_BLASTER; i <= FRAG_BFG; i++)
					{
						if (e->frags[i].atts == 0 && e->frags[i].kills == 0 && e->frags[i].deaths == 0) continue;
						int ac = e->frags[i].atts > 0 ? (int)(((float)e->frags[i].hits * 100.0f / (float)e->frags[i].atts) + 0.5f) : 0;
						if (ac > 100) ac = 100;
						gi.cprintf(ent, PRINT_HIGH, "%-16s %3d%% %5d/%-5d  %4d  %4d  %3d\n",
							weapon_names[i], ac, e->frags[i].hits, e->frags[i].atts,
							e->frags[i].kills, e->frags[i].deaths, e->frags[i].suicides);
						s_atts += e->frags[i].atts; s_hits += e->frags[i].hits;
						s_kills += e->frags[i].kills; s_deaths += e->frags[i].deaths;
						s_suicides += e->frags[i].suicides;
					}
					s_acc = s_atts > 0 ? (int)(((float)s_hits * 100.0f / (float)s_atts) + 0.5f) : 0;
					if (s_acc > 100) s_acc = 100;
					gi.cprintf(ent, PRINT_HIGH, "-----------------------------------------------------\n");
					gi.cprintf(ent, PRINT_HIGH, "Total:           %3d%% %5d/%-5d  %4d  %4d  %3d\n",
						s_acc, s_hits, s_atts, s_kills, s_deaths, s_suicides);
					if (e->damage_given > 0 || e->damage_recvd > 0)
						gi.cprintf(ent, PRINT_HIGH, "\nDamage Given: %d  Received: %d\n", e->damage_given, e->damage_recvd);
					return;
				}
			}
			gi.cprintf(ent, PRINT_HIGH, "Player '%s' not found (not in game and no cached stats).\n", name);
			return;
		}
		target = found;
	}

	frags = target->client->resp.frags;

	gi.cprintf(ent, PRINT_HIGH, "\nAccuracy stats for %s:\n", target->client->pers.netname);
	gi.cprintf(ent, PRINT_HIGH, "Weapon             Acc   Hit/Atts   Kills Deaths Suic\n");
	gi.cprintf(ent, PRINT_HIGH, "-----------------------------------------------------\n");

	for (i = FRAG_BLASTER; i <= FRAG_BFG; i++)
	{
		if (frags[i].atts == 0 && frags[i].kills == 0 && frags[i].deaths == 0)
			continue;

		acc = frags[i].atts > 0 ? (int)(((float)frags[i].hits * 100.0f / (float)frags[i].atts) + 0.5f) : 0;
		if (acc > 100) acc = 100;

		gi.cprintf(ent, PRINT_HIGH, "%-16s %3d%% %5d/%-5d  %4d  %4d  %3d\n",
			weapon_names[i], acc, frags[i].hits, frags[i].atts,
			frags[i].kills, frags[i].deaths, frags[i].suicides);

		sum_atts += frags[i].atts;
		sum_hits += frags[i].hits;
		sum_kills += frags[i].kills;
		sum_deaths += frags[i].deaths;
		sum_suicides += frags[i].suicides;
	}

	sum_acc = sum_atts > 0 ? (int)(((float)sum_hits * 100.0f / (float)sum_atts) + 0.5f) : 0;
	if (sum_acc > 100) sum_acc = 100;

	gi.cprintf(ent, PRINT_HIGH, "-----------------------------------------------------\n");
	gi.cprintf(ent, PRINT_HIGH, "Total:           %3d%% %5d/%-5d  %4d  %4d  %3d\n",
		sum_acc, sum_hits, sum_atts, sum_kills, sum_deaths, sum_suicides);

	if (target->client->resp.damage_given > 0 || target->client->resp.damage_recvd > 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "\nDamage Given: %d  Received: %d\n",
			target->client->resp.damage_given, target->client->resp.damage_recvd);
	}
	CPRepeat(10, 2);
}

/*
=================
Cmd_StatsAll_f
=================
*/
void Cmd_StatsAll_f(edict_t *ent)
{
	int i, j, x;
	char header[64], rowfmt[64];

	// Helper macro: compute accuracy % from fragstat
#define ACC(f, w) ((f)[w].atts > 0 ? (int)(((float)(f)[w].hits * 100.0f / (float)(f)[w].atts) + 0.5f) : 0)
#define CAPV(v)   ((v) > 100 ? 100 : (v))

	Com_sprintf(header, sizeof(header), "\n%-9s %2s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s%s%s\n",
		"Name", "ki", "RA%", "CH%", "RL%", "MG%", "SG%", "SS%", "HB%", "GR%", "GL%", "BL%",
		zigmode->value ? " FAS" : "",
		zigmode->value ? " FBN" : "");

	Com_sprintf(rowfmt, sizeof(rowfmt), "%%-9s %%2d %%3d %%3d %%3d %%3d %%3d %%3d %%3d %%3d %%3d %%3d%s%s\n",
		zigmode->value ? " %3d" : "",
		zigmode->value ? " %3d" : "");

	gi.cprintf(ent, PRINT_HIGH, "%s", header);
	CPRepeat(45, zigmode->value ? 60 : 52);

	if (level.intermissiontime && num_cached_stats > 0)
	{
		// Sort a local index array by kills (descending)
		int order[MAX_CACHED_STATS];
		for (i = 0; i < num_cached_stats; i++) order[i] = i;
		for (i = 0; i < num_cached_stats - 1; i++)
		{
			for (j = i + 1; j < num_cached_stats; j++)
			{
				int k1 = 0, k2 = 0;
				for (x = 0; x < FRAG_TOTAL; x++) k1 += cached_stats[order[i]].frags[x].kills;
				for (x = 0; x < FRAG_TOTAL; x++) k2 += cached_stats[order[j]].frags[x].kills;
				if (k1 < k2) { int tmp = order[i]; order[i] = order[j]; order[j] = tmp; }
			}
		}

		for (i = 0; i < num_cached_stats; i++)
		{
			cached_stat_entry_t *e = &cached_stats[order[i]];
			fragstat_t *f = e->frags;
			int total_kills = 0;
			for (x = 0; x < FRAG_TOTAL; x++) total_kills += f[x].kills;

			char name[10];
			strncpy(name, e->name, 9); name[9] = '\0';

			if (zigmode->value)
				gi.cprintf(ent, PRINT_HIGH, "%-9s %2d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
					name, total_kills,
					CAPV(ACC(f, FRAG_RAILGUN)), CAPV(ACC(f, FRAG_CHAINGUN)),
					CAPV(ACC(f, FRAG_ROCKETLAUNCHER)), CAPV(ACC(f, FRAG_MACHINEGUN)),
					CAPV(ACC(f, FRAG_SHOTGUN)), CAPV(ACC(f, FRAG_SUPERSHOTGUN)),
					CAPV(ACC(f, FRAG_HYPERBLASTER)), CAPV(ACC(f, FRAG_GRENADES)),
					CAPV(ACC(f, FRAG_GRENADELAUNCHER)), CAPV(ACC(f, FRAG_BLASTER)),
					f[FLAG_EVENT].assassin, f[FLAG_EVENT].possession);
			else
				gi.cprintf(ent, PRINT_HIGH, "%-9s %2d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
					name, total_kills,
					CAPV(ACC(f, FRAG_RAILGUN)), CAPV(ACC(f, FRAG_CHAINGUN)),
					CAPV(ACC(f, FRAG_ROCKETLAUNCHER)), CAPV(ACC(f, FRAG_MACHINEGUN)),
					CAPV(ACC(f, FRAG_SHOTGUN)), CAPV(ACC(f, FRAG_SUPERSHOTGUN)),
					CAPV(ACC(f, FRAG_HYPERBLASTER)), CAPV(ACC(f, FRAG_GRENADES)),
					CAPV(ACC(f, FRAG_GRENADELAUNCHER)), CAPV(ACC(f, FRAG_BLASTER)));
		}
		CPRepeat(10, 2);
	}
	else
	{
		// Live match: read directly from connected clients
		edict_t *players[MAX_CLIENTS];
		int num_players = 0;
		for (i = 1; i <= game.maxclients; i++)
		{
			if (g_edicts[i].inuse && g_edicts[i].client && g_edicts[i].client->pers.netname[0])
				players[num_players++] = &g_edicts[i];
		}
		for (i = 0; i < num_players - 1; i++)
		{
			for (j = i + 1; j < num_players; j++)
			{
				int k1 = 0, k2 = 0;
				for (x = 0; x < FRAG_TOTAL; x++) k1 += players[i]->client->resp.frags[x].kills;
				for (x = 0; x < FRAG_TOTAL; x++) k2 += players[j]->client->resp.frags[x].kills;
				if (k1 < k2) { edict_t *tmp = players[i]; players[i] = players[j]; players[j] = tmp; }
			}
		}
		for (i = 0; i < num_players; i++)
		{
			edict_t *p = players[i];
			fragstat_t *f = p->client->resp.frags;
			int total_kills = 0;
			for (x = 0; x < FRAG_TOTAL; x++) total_kills += f[x].kills;

			char name[10];
			strncpy(name, p->client->pers.netname, 9); name[9] = '\0';

			if (zigmode->value)
				gi.cprintf(ent, PRINT_HIGH, "%-9s %2d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
					name, total_kills,
					CAPV(ACC(f, FRAG_RAILGUN)), CAPV(ACC(f, FRAG_CHAINGUN)),
					CAPV(ACC(f, FRAG_ROCKETLAUNCHER)), CAPV(ACC(f, FRAG_MACHINEGUN)),
					CAPV(ACC(f, FRAG_SHOTGUN)), CAPV(ACC(f, FRAG_SUPERSHOTGUN)),
					CAPV(ACC(f, FRAG_HYPERBLASTER)), CAPV(ACC(f, FRAG_GRENADES)),
					CAPV(ACC(f, FRAG_GRENADELAUNCHER)), CAPV(ACC(f, FRAG_BLASTER)),
					p->client->resp.assassin, p->client->resp.possession);
			else
				gi.cprintf(ent, PRINT_HIGH, "%-9s %2d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
					name, total_kills,
					CAPV(ACC(f, FRAG_RAILGUN)), CAPV(ACC(f, FRAG_CHAINGUN)),
					CAPV(ACC(f, FRAG_ROCKETLAUNCHER)), CAPV(ACC(f, FRAG_MACHINEGUN)),
					CAPV(ACC(f, FRAG_SHOTGUN)), CAPV(ACC(f, FRAG_SUPERSHOTGUN)),
					CAPV(ACC(f, FRAG_HYPERBLASTER)), CAPV(ACC(f, FRAG_GRENADES)),
					CAPV(ACC(f, FRAG_GRENADELAUNCHER)), CAPV(ACC(f, FRAG_BLASTER)));
		}
		CPRepeat(10, 2);
	}

#undef ACC
#undef CAPV
}



/*
=================
ClientCommand
=================
*/
void ClientCommand (edict_t *ent)
{
	char	*cmd;

	if (!ent->client)
		return;		// not fully in game yet

	cmd = gi.argv(0);

	if (Q_stricmp (cmd, "players") == 0)
	{
		Cmd_Players_f (ent);
		return;
	}
	if (Q_stricmp (cmd, "say") == 0)
	{
		Cmd_Say_f (ent, qfalse, qfalse);
		return;
	}
	if (Q_stricmp (cmd, "say_team") == 0)
	{
		Cmd_Say_f (ent, qtrue, qfalse);
		return;
	}
	if (Q_stricmp (cmd, "score") == 0)
	{
		Cmd_Score_f (ent);
		return;
	}
	if (Q_stricmp (cmd, "help") == 0)
	{
		Cmd_Help_f (ent);
		return;
	}
	if (Q_stricmp (cmd, "stats") == 0 || Q_stricmp (cmd, "accuracy") == 0)
	{
		Cmd_Stats_f (ent, qtrue);
		return;
	}
	if (Q_stricmp (cmd, "stats-all") == 0)
	{
		Cmd_StatsAll_f (ent);
		return;
	}

	if (Q_stricmp (cmd, "store") == 0)
	{
		Cmd_Store_f(ent, qtrue);
		return;
	}

	if (Q_stricmp (cmd, "recall") == 0)
	{
		Cmd_Recall_f(ent);
		return;
	}

	if (level.intermissiontime)
		return;

	if (Q_stricmp (cmd, "use") == 0)
		Cmd_Use_f (ent);
	else if (Q_stricmp (cmd, "drop") == 0)
		Cmd_Drop_f (ent);
	else if (Q_stricmp (cmd, "give") == 0)
		Cmd_Give_f (ent);
	else if (Q_stricmp (cmd, "god") == 0)
		Cmd_God_f (ent);
	else if (Q_stricmp (cmd, "notarget") == 0)
		Cmd_Notarget_f (ent);
	else if (Q_stricmp (cmd, "noclip") == 0)
		Cmd_Noclip_f (ent);
	else if (Q_stricmp (cmd, "inven") == 0)
		Cmd_Inven_f (ent);
	else if (Q_stricmp (cmd, "invnext") == 0)
		SelectNextItem (ent, -1);
	else if (Q_stricmp (cmd, "invprev") == 0)
		SelectPrevItem (ent, -1);
	else if (Q_stricmp (cmd, "invnextw") == 0)
		SelectNextItem (ent, IT_WEAPON);
	else if (Q_stricmp (cmd, "invprevw") == 0)
		SelectPrevItem (ent, IT_WEAPON);
	else if (Q_stricmp (cmd, "invnextp") == 0)
		SelectNextItem (ent, IT_POWERUP);
	else if (Q_stricmp (cmd, "invprevp") == 0)
		SelectPrevItem (ent, IT_POWERUP);
	else if (Q_stricmp (cmd, "invuse") == 0)
		Cmd_InvUse_f (ent);
	else if (Q_stricmp (cmd, "invdrop") == 0)
		Cmd_InvDrop_f (ent);
	else if (Q_stricmp (cmd, "weapprev") == 0)
		Cmd_WeapPrev_f (ent);
	else if (Q_stricmp (cmd, "weapnext") == 0)
		Cmd_WeapNext_f (ent);
	else if (Q_stricmp (cmd, "weaplast") == 0)
		Cmd_WeapLast_f (ent);
	else if (Q_stricmp (cmd, "kill") == 0)
		Cmd_Kill_f (ent);
	else if (Q_stricmp (cmd, "putaway") == 0)
		Cmd_PutAway_f (ent);
	else if (Q_stricmp (cmd, "wave") == 0)
		Cmd_Wave_f (ent);
	else if (Q_stricmp (cmd, "zoomin") == 0)		//zoom
		Cmd_ZoomIn(ent);
	else if (Q_stricmp (cmd, "zoomout") == 0)
		Cmd_ZoomOut(ent);
	else if (Q_stricmp (cmd, "autozoom") == 0)
		Cmd_AutoZoom(ent);
	else if (Q_stricmp (cmd, "air") == 0)
		Cmd_AirStrike(ent);
	else if (Q_stricmp (cmd, "undo") == 0)
	{
		if(gi.argc() <= 1) UndoChain(ent,1);
		else UndoChain (ent,atoi(gi.argv(1)));
	}
//ZOID
	else if (Q_stricmp (cmd, "team") == 0)
	{
		CTFTeam_f (ent);
	} else if (Q_stricmp(cmd, "id") == 0) {
		CTFID_f (ent);
	}
//ZOID
	else	// anything that doesn't match a command will be a chat
		Cmd_Say_f (ent, qfalse, qtrue);
}

void Cmd_Store_f (edict_t *ent, qboolean verbose)
{
	if (!ent->client)
		return;

	if (!sv_cheats->value) {
		if (verbose)
			gi.cprintf(ent, PRINT_HIGH, "Not allowed\n");
		return;
	}

	VectorCopy(ent->s.origin, ent->client->pers.stored_origin);
	VectorCopy(ent->client->ps.viewangles, ent->client->pers.stored_angles);
	ent->client->pers.stored_frame = level.framenum;
}

void Cmd_Recall_f (edict_t *ent)
{
	if (!ent->client)
		return;

	if (ent->client->pers.stored_frame == 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "No valid position stored.\n");
		return;
	}

	VectorCopy(ent->client->pers.stored_origin, ent->s.origin);
	VectorCopy(ent->client->pers.stored_origin, ent->s.old_origin);
	VectorCopy(ent->client->pers.stored_origin, ent->client->ps.pmove.origin);

	VectorCopy(ent->client->pers.stored_angles, ent->client->ps.viewangles);
	VectorCopy(ent->client->pers.stored_angles, ent->client->v_angle);
	VectorCopy(ent->client->pers.stored_angles, ent->s.angles);

	for (int i=0 ; i<3 ; i++)
		ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(ent->client->v_angle[i] - ent->client->resp.cmd_angles[i]);

	VectorCopy(vec3_origin, ent->velocity);
	VectorCopy(vec3_origin, ent->client->ps.pmove.velocity);

	gi.linkentity(ent);
}
