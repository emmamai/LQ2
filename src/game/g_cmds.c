/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * Game command processing.
 *
 * =======================================================================
 */

#include "header/local.h"
#include "monster/misc/player.h"

static char *
ClientTeam(edict_t *ent, char* value)
{
	char *p;

	value[0] = 0;

	if (!ent)
	{
		return value;
	}

	if (!ent->client)
	{
		return value;
	}

	strcpy(value, Info_ValueForKey(ent->client->pers.userinfo, "skin"));
	p = strchr(value, '/');

	if (!p)
	{
		return value;
	}

	if ((int)(dmflags->value) & DF_MODELTEAMS)
	{
		*p = 0;
		return value;
	}

	return ++p;
}

qboolean
OnSameTeam(edict_t *ent1, edict_t *ent2)
{
	char ent1Team[512];
	char ent2Team[512];

	if (!ent1 || !ent2)
	{
		return false;
	}

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	{
		return false;
	}

	ClientTeam(ent1, ent1Team);
	ClientTeam(ent2, ent2Team);

	if (ent1Team[0] != '\0' && strcmp(ent1Team, ent2Team) == 0)
	{
		return true;
	}

	return false;
}

void
SelectNextItem(edict_t *ent, int itflags)
{
	gclient_t *cl;
	int i, index;
	gitem_t *it;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (cl->chase_target)
	{
		ChaseNext(ent);
		return;
	}

	/* scan  for the next valid one */
	for (i = 1; i <= MAX_ITEMS; i++)
	{
		index = (cl->pers.selected_item + i) % MAX_ITEMS;

		if (!cl->pers.inventory[index])
		{
			continue;
		}

		it = &itemlist[index];

		if (!it->use)
		{
			continue;
		}

		if (!(it->flags & itflags))
		{
			continue;
		}

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void
ValidateSelectedItem(edict_t *ent)
{
	gclient_t *cl;

	if (!ent)
	{
		return;
	}

	cl = ent->client;

	if (cl->pers.inventory[cl->pers.selected_item])
	{
		return; /* valid */
	}

	SelectNextItem(ent, -1);
}

/*
 * Sets client to godmode
 */
void
Cmd_God_f(edict_t *ent)
{
	char *msg;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf( ent, PRINT_HIGH,
				"You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_GODMODE;

	if (!(ent->flags & FL_GODMODE))
	{
		msg = "godmode OFF\n";
	}
	else
	{
		msg = "godmode ON\n";
	}

	gi.cprintf(ent, PRINT_HIGH, msg);
}

/*
 * argv(0) noclip
 */
void
Cmd_Noclip_f(edict_t *ent)
{
	char *msg;

	if (!ent)
	{
		return;
	}

	if ((deathmatch->value || coop->value) && !sv_cheats->value)
	{
		gi.cprintf( ent, PRINT_HIGH,
				"You must run the server with '+set cheats 1' to enable this command.\n");
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

	gi.cprintf(ent, PRINT_HIGH, msg);
} 

void
Cmd_Score_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	ent->client->showinventory = false;
	ent->client->showhelp = false;

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboardMessage(ent, ent->enemy);
	gi.unicast(ent, true);
}

void
Cmd_Kill_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	if (((level.time - ent->client->respawn_time) < 5) ||
		(ent->client->resp.spectator))
	{
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die(ent, ent, ent, 100000, vec3_origin);
}

void
Cmd_PutAway_f(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showinventory = false;
}

int
PlayerSort(void const *a, void const *b)
{
	int anum, bnum;

	if (!a || !b)
	{
		return 0;
	}

	anum = *(int *)a;
	bnum = *(int *)b;

	anum = game.clients[anum].ps.stats[STAT_FRAGS];
	bnum = game.clients[bnum].ps.stats[STAT_FRAGS];

	if (anum < bnum)
	{
		return -1;
	}

	if (anum > bnum)
	{
		return 1;
	}

	return 0;
}

void
Cmd_Players_f(edict_t *ent)
{
	int i;
	int count;
	char small[64];
	char large[1280];
	int index[256];

	if (!ent)
	{
		return;
	}

	count = 0;

	for (i = 0; i < maxclients->value; i++)
	{
		if (game.clients[i].pers.connected)
		{
			index[count] = i;
			count++;
		}
	}

	/* sort by frags */
	qsort(index, count, sizeof(index[0]), PlayerSort);

	/* print information */
	large[0] = 0;

	for (i = 0; i < count; i++)
	{
		Com_sprintf(small, sizeof(small), "%3i %s\n",
				game.clients[index[i]].ps.stats[STAT_FRAGS],
				game.clients[index[i]].pers.netname);

		if (strlen(small) + strlen(large) > sizeof(large) - 100)
		{
			/* can't print all of them in one packet */
			strcat(large, "...\n");
			break;
		}

		strcat(large, small);
	}

	gi.cprintf(ent, PRINT_HIGH, "%s\n%i players\n", large, count);
}

void
Cmd_Wave_f(edict_t *ent)
{
	int i;

	if (!ent)
	{
		return;
	}

	i = (int)strtol(gi.argv(1), (char **)NULL, 10);

	/* can't wave when ducked */
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		return;
	}

	if (ent->client->anim_priority > ANIM_WAVE)
	{
		return;
	}

	ent->client->anim_priority = ANIM_WAVE;

	switch (i)
	{
		case 0:
			gi.cprintf(ent, PRINT_HIGH, "flipoff\n");
			ent->s.frame = FRAME_flip01 - 1;
			ent->client->anim_end = FRAME_flip12;
			break;
		case 1:
			gi.cprintf(ent, PRINT_HIGH, "salute\n");
			ent->s.frame = FRAME_salute01 - 1;
			ent->client->anim_end = FRAME_salute11;
			break;
		case 2:
			gi.cprintf(ent, PRINT_HIGH, "taunt\n");
			ent->s.frame = FRAME_taunt01 - 1;
			ent->client->anim_end = FRAME_taunt17;
			break;
		case 3:
			gi.cprintf(ent, PRINT_HIGH, "wave\n");
			ent->s.frame = FRAME_wave01 - 1;
			ent->client->anim_end = FRAME_wave11;
			break;
		case 4:
		default:
			gi.cprintf(ent, PRINT_HIGH, "point\n");
			ent->s.frame = FRAME_point01 - 1;
			ent->client->anim_end = FRAME_point12;
			break;
	}
}

void
Cmd_Say_f(edict_t *ent, qboolean team, qboolean arg0)
{
	int i, j;
	edict_t *other;
	char *p;
	char text[2048];
	gclient_t *cl;

	if (!ent)
	{
		return;
	}

	if ((gi.argc() < 2) && !arg0)
	{
		return;
	}

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	{
		team = false;
	}

	if (team)
	{
		Com_sprintf(text, sizeof(text), "(%s): ", ent->client->pers.netname);
	}
	else
	{
		Com_sprintf(text, sizeof(text), "%s: ", ent->client->pers.netname);
	}

	if (arg0)
	{
		strcat(text, gi.argv(0));
		strcat(text, " ");
		strcat(text, gi.args());
	}
	else
	{
		p = gi.args();

		if (*p == '"')
		{
			p++;
			p[strlen(p) - 1] = 0;
		}

		strcat(text, p);
	}

	/* don't let text be too long for malicious reasons */
	if (strlen(text) > 150)
	{
		text[150] = 0;
	}

	strcat(text, "\n");

	if (flood_msgs->value)
	{
		cl = ent->client;

		if (level.time < cl->flood_locktill)
		{
			gi.cprintf(ent, PRINT_HIGH, "You can't talk for %d more seconds\n",
					(int)(cl->flood_locktill - level.time));
			return;
		}

		i = cl->flood_whenhead - flood_msgs->value + 1;

		if (i < 0)
		{
			i = (sizeof(cl->flood_when) / sizeof(cl->flood_when[0])) + i;
		}

		if (cl->flood_when[i] &&
			(level.time - cl->flood_when[i] < flood_persecond->value))
		{
			cl->flood_locktill = level.time + flood_waitdelay->value;
			gi.cprintf(ent, PRINT_CHAT,
					"Flood protection:  You can't talk for %d seconds.\n",
					(int)flood_waitdelay->value);
			return;
		}

		cl->flood_whenhead = (cl->flood_whenhead + 1) %
							 (sizeof(cl->flood_when) / sizeof(cl->flood_when[0]));
		cl->flood_when[cl->flood_whenhead] = level.time;
	}

	if (dedicated->value)
	{
		gi.cprintf(NULL, PRINT_CHAT, "%s", text);
	}

	for (j = 1; j <= game.maxclients; j++)
	{
		other = &g_edicts[j];

		if (!other->inuse)
		{
			continue;
		}

		if (!other->client)
		{
			continue;
		}

		if (team)
		{
			if (!OnSameTeam(ent, other))
			{
				continue;
			}
		}

		gi.cprintf(other, PRINT_CHAT, "%s", text);
	}
}

void
Cmd_PlayerList_f(edict_t *ent)
{
	int i;
	char st[80];
	char text[1400];
	edict_t *e2;

	if (!ent)
	{
		return;
	}

	/* connect time, ping, score, name */
	*text = 0;

	for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++)
	{
		if (!e2->inuse)
		{
			continue;
		}

		Com_sprintf(st, sizeof(st), "%02d:%02d %4d %3d %s%s\n",
				(level.framenum - e2->client->resp.enterframe) / 600,
				((level.framenum - e2->client->resp.enterframe) % 600) / 10,
				e2->client->ping,
				e2->client->resp.score,
				e2->client->pers.netname,
				e2->client->resp.spectator ? " (spectator)" : "");

		if (strlen(text) + strlen(st) > sizeof(text) - 50)
		{
			strcpy(text + strlen(text), "And more...\n");
			gi.cprintf(ent, PRINT_HIGH, "%s", text);
			return;
		}

		strcat(text, st);
	}

	gi.cprintf(ent, PRINT_HIGH, "%s", text);
}

void
ClientCommand(edict_t *ent)
{
	char *cmd;

	if (!ent)
	{
		return;
	}

	if (!ent->client)
	{
		return; /* not fully in game yet */
	}

	cmd = gi.argv(0);

	if (Q_stricmp(cmd, "players") == 0)
	{
		Cmd_Players_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "say") == 0)
	{
		Cmd_Say_f(ent, false, false);
		return;
	}

	if (Q_stricmp(cmd, "say_team") == 0)
	{
		Cmd_Say_f(ent, true, false);
		return;
	}

	if (Q_stricmp(cmd, "score") == 0)
	{
		Cmd_Score_f(ent);
		return;
	}

	if (Q_stricmp(cmd, "help") == 0)
	{
		Cmd_Score_f(ent);
		return;
	}

	if (level.intermissiontime)
	{
		return;
	}

	if (Q_stricmp(cmd, "god") == 0)
	{
		Cmd_God_f(ent);
	}
	else if (Q_stricmp(cmd, "noclip") == 0)
	{
		Cmd_Noclip_f(ent);
	}
	else if (Q_stricmp(cmd, "kill") == 0)
	{
		Cmd_Kill_f(ent);
	}
	else if (Q_stricmp(cmd, "putaway") == 0)
	{
		Cmd_PutAway_f(ent);
	}
	else if (Q_stricmp(cmd, "wave") == 0)
	{
		Cmd_Wave_f(ent);
	}
	else if (Q_stricmp(cmd, "playerlist") == 0)
	{
		Cmd_PlayerList_f(ent);
	}
	else /* anything that doesn't match a command will be a chat */
	{
		Cmd_Say_f(ent, false, true);
	}
}
