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
#include "player/player.h"

/*
 * Sets client to godmode
 */
void Cmd_God_f( edict_t *ent ) {
	char *msg;

	if ( !ent ) {
		return;
	}

	if ( !sv_cheats->value ) {
		gi.cprintf( ent, PRINT_HIGH,
		            "You must run the server with '+set cheats 1' to enable this command.\n" );
		return;
	}

	ent->flags ^= FL_GODMODE;

	if ( !( ent->flags & FL_GODMODE ) ) {
		msg = "godmode OFF\n";
	} else {
		msg = "godmode ON\n";
	}

	gi.cprintf( ent, PRINT_HIGH, msg );
}

/*
 * argv(0) noclip
 */
void Cmd_Noclip_f( edict_t *ent ) {
	char *msg;

	if ( !ent ) {
		return;
	}

	if ( !sv_cheats->value ) {
		gi.cprintf( ent, PRINT_HIGH,
		            "You must run the server with '+set cheats 1' to enable this command.\n" );
		return;
	}

	if ( ent->movetype == MOVETYPE_NOCLIP ) {
		ent->movetype = MOVETYPE_WALK;
		msg = "noclip OFF\n";
	} else {
		ent->movetype = MOVETYPE_NOCLIP;
		msg = "noclip ON\n";
	}

	gi.cprintf( ent, PRINT_HIGH, msg );
}

void Cmd_Score_f( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	if ( ent->client->showscores ) {
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboardMessage( ent, ent->enemy );
	gi.unicast( ent, true );
}

void Cmd_Kill_f( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	if ( ( level.time - ent->client->respawn_time ) < 5 ) {
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die( ent, ent, ent, 100000, vec3_origin );
}

int PlayerSort( void const *a, void const *b ) {
	int anum, bnum;

	if ( !a || !b ) {
		return 0;
	}

	anum = *( int * )a;
	bnum = *( int * )b;

	anum = game.clients[anum].ps.stats[STAT_FRAGS];
	bnum = game.clients[bnum].ps.stats[STAT_FRAGS];

	if ( anum < bnum ) {
		return -1;
	}

	if ( anum > bnum ) {
		return 1;
	}

	return 0;
}

void Cmd_Players_f( edict_t *ent ) {
	int i;
	int count;
	char small[64];
	char large[1280];
	int index[256];

	if ( !ent ) {
		return;
	}

	count = 0;

	for ( i = 0; i < maxclients->value; i++ ) {
		if ( game.clients[i].pers.connected ) {
			index[count] = i;
			count++;
		}
	}

	/* sort by frags */
	qsort( index, count, sizeof( index[0] ), PlayerSort );

	/* print information */
	large[0] = 0;

	for ( i = 0; i < count; i++ ) {
		Com_sprintf( small, sizeof( small ), "%3i %s\n",
		             game.clients[index[i]].ps.stats[STAT_FRAGS],
		             game.clients[index[i]].pers.netname );

		if ( strlen( small ) + strlen( large ) > sizeof( large ) - 100 ) {
			/* can't print all of them in one packet */
			strcat( large, "...\n" );
			break;
		}

		strcat( large, small );
	}

	gi.cprintf( ent, PRINT_HIGH, "%s\n%i players\n", large, count );
}

void Cmd_Say_f( edict_t *ent, qboolean arg0 ) {
	int i;
	edict_t *other;
	char *p;
	char text[2048];

	if ( !ent ) {
		return;
	}

	if ( ( gi.argc() < 2 ) && !arg0 ) {
		return;
	}

	Com_sprintf( text, sizeof( text ), "%s: ", ent->client->pers.netname );

	if ( arg0 ) {
		strcat( text, gi.argv( 0 ) );
		strcat( text, " " );
		strcat( text, gi.args() );
	} else {
		p = gi.args();

		if ( *p == '"' ) {
			p++;
			p[strlen( p ) - 1] = 0;
		}

		strcat( text, p );
	}

	/* don't let text be too long for malicious reasons */
	if ( strlen( text ) > 150 ) {
		text[150] = 0;
	}

	strcat( text, "\n" );

	if ( dedicated->value ) {
		gi.cprintf( NULL, PRINT_CHAT, "%s", text );
	}

	for ( i = 1; i <= game.maxclients; i++ ) {
		other = &g_edicts[i];

		if ( !other->inuse ) {
			continue;
		}

		if ( !other->client ) {
			continue;
		}

		gi.cprintf( other, PRINT_CHAT, "%s", text );
	}
}

void Cmd_PlayerList_f( edict_t *ent ) {
	int i;
	char st[80];
	char text[1400];
	edict_t *e2;

	if ( !ent ) {
		return;
	}

	/* connect time, ping, score, name */
	*text = 0;

	for ( i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++ ) {
		if ( !e2->inuse ) {
			continue;
		}

		Com_sprintf( st, sizeof( st ), "%02d:%02d %4d %3d %s%s\n",
		             ( level.framenum - e2->client->resp.enterframe ) / 600,
		             ( ( level.framenum - e2->client->resp.enterframe ) % 600 ) / 10,
		             e2->client->ping,
		             e2->client->resp.score,
		             e2->client->pers.netname,
		             "" );

		if ( strlen( text ) + strlen( st ) > sizeof( text ) - 50 ) {
			strcpy( text + strlen( text ), "And more...\n" );
			gi.cprintf( ent, PRINT_HIGH, "%s", text );
			return;
		}

		strcat( text, st );
	}

	gi.cprintf( ent, PRINT_HIGH, "%s", text );
}

void ClientCommand( edict_t *ent ) {
	char *cmd;

	if ( !ent ) {
		return;
	}

	if ( !ent->client ) {
		return; /* not fully in game yet */
	}

	cmd = gi.argv( 0 );

	if ( Q_stricmp( cmd, "players" ) == 0 ) {
		Cmd_Players_f( ent );
		return;
	} else if ( Q_stricmp( cmd, "say" ) == 0 ) {
		Cmd_Say_f( ent, false );
		return;
	} else if ( Q_stricmp( cmd, "score" ) == 0 ) {
		Cmd_Score_f( ent );
		return;
	} else if ( Q_stricmp( cmd, "help" ) == 0 ) {
		Cmd_Score_f( ent );
		return;
	} else if ( Q_stricmp( cmd, "inven" ) == 0 ) {
		Cmd_Score_f( ent );
		return;
	}

	if ( level.intermissiontime ) {
		return;
	}

	if ( Q_stricmp( cmd, "god" ) == 0 ) {
		Cmd_God_f( ent );
	} else if ( Q_stricmp( cmd, "noclip" ) == 0 ) {
		Cmd_Noclip_f( ent );
	} else if ( Q_stricmp( cmd, "kill" ) == 0 ) {
		Cmd_Kill_f( ent );
	} else if ( Q_stricmp( cmd, "playerlist" ) == 0 ) {
		Cmd_PlayerList_f( ent );
	} else { /* anything that doesn't match a command will be a chat */
		//Cmd_Say_f( ent, true );
		gi.cprintf( ent, PRINT_HIGH, "Unknown command %s\n", cmd );
	}
}

void ServerCommand( void ) {
	gi.cprintf( NULL, PRINT_HIGH, "Server commands unsupported\n" );
}
