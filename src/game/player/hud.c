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
 * HUD, scoreboard, and intermission stuff.
 *
 * =======================================================================
 */

#include "../header/local.h"

void MoveClientToIntermission( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->client->showscores = true;

	VectorCopy( level.intermission_origin, ent->s.origin );
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0] * 8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1] * 8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2] * 8;
	VectorCopy( level.intermission_angle, ent->client->ps.viewangles );
	ent->client->ps.pmove.pm_type = PM_FREEZE;
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	/* clean up powerup info */
	ent->client->invincible_framenum = 0;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	gi.linkentity( ent );

	/* add the layout */
	DeathmatchScoreboardMessage( ent, NULL );
	gi.unicast( ent, true );
}

void BeginIntermission( edict_t *targ ) {
	int i;
	edict_t *ent, *client;

	if ( !targ ) {
		return;
	}

	if ( level.intermissiontime ) {
		return; /* already activated */
	}

	game.autosaved = false;

	/* respawn any dead clients */
	for ( i = 0; i < maxclients->value; i++ ) {
		client = g_edicts + 1 + i;

		if ( !client->inuse ) {
			continue;
		}

		if ( client->health <= 0 ) {
			respawn( client );
		}
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if ( !strstr( level.changemap, "*" ) ) {
		level.exitintermission = 1; /* go immediately to the next level */
		return;
	}

	level.exitintermission = 0;

	/* find an intermission spot */
	ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );

	if ( !ent ) {
		/* the map creator forgot to put in an intermission point... */
		ent = G_Find( NULL, FOFS( classname ), "info_player_start" );

		if ( !ent ) {
			ent = G_Find( NULL, FOFS( classname ), "info_player_deathmatch" );
		}
	} else {
		/* chose one of four spots */
		i = randk() & 3;

		while ( i-- ) {
			ent = G_Find( ent, FOFS( classname ), "info_player_intermission" );

			if ( !ent ) { /* wrap around the list */
				ent = G_Find( ent, FOFS( classname ), "info_player_intermission" );
			}
		}
	}

	VectorCopy( ent->s.origin, level.intermission_origin );
	VectorCopy( ent->s.angles, level.intermission_angle );

	/* In fact1 the intermission collides
	   with an area portal, resulting in
	   clutterings */
	if ( !Q_stricmp( level.mapname, "fact1" ) ) {
		level.intermission_origin[0] = 1037.0;
		level.intermission_origin[1] = 1100.0;
		level.intermission_origin[2] = 222.0;
	}

	/* move all clients to the intermission point */
	for ( i = 0; i < maxclients->value; i++ ) {
		client = g_edicts + 1 + i;

		if ( !client->inuse ) {
			continue;
		}

		MoveClientToIntermission( client );
	}
}

void DeathmatchScoreboardMessage( edict_t *ent, edict_t *killer ) {
	char entry[1024];
	char string[1400];
	int stringlength;
	int i, j, k;
	int sorted[MAX_CLIENTS];
	int sortedscores[MAX_CLIENTS];
	int score, total;
	int x, y;
	gclient_t *cl;
	edict_t *cl_ent;
	char *tag;

	if ( !ent ) { /* killer can be NULL */
		return;
	}

	/* sort the clients by score */
	total = 0;

	for ( i = 0; i < game.maxclients; i++ ) {
		cl_ent = g_edicts + 1 + i;

		if ( !cl_ent->inuse  ) {
			continue;
		}

		score = game.clients[i].resp.score;

		for ( j = 0; j < total; j++ ) {
			if ( score > sortedscores[j] ) {
				break;
			}
		}

		for ( k = total; k > j; k-- ) {
			sorted[k] = sorted[k - 1];
			sortedscores[k] = sortedscores[k - 1];
		}

		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	/* print level name and exit rules */
	string[0] = 0;

	stringlength = strlen( string );

	/* add the clients in sorted order */
	if ( total > 12 ) {
		total = 12;
	}

	for ( i = 0; i < total; i++ ) {
		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		x = ( i >= 6 ) ? 160 : 0;
		y = 32 + 32 * ( i % 6 );

		/* add a dogtag */
		if ( cl_ent == ent ) {
			tag = "tag1";
		} else if ( cl_ent == killer ) {
			tag = "tag2";
		} else {
			tag = NULL;
		}

		if ( tag ) {
			Com_sprintf( entry, sizeof( entry ),
			             "xv %i yv %i picn %s ", x + 32, y, tag );
			j = strlen( entry );

			if ( stringlength + j > 1024 ) {
				break;
			}

			strcpy( string + stringlength, entry );
			stringlength += j;
		}

		/* send the layout */
		Com_sprintf( entry, sizeof( entry ),
		             "client %i %i %i %i %i %i ",
		             x, y, sorted[i], cl->resp.score, cl->ping,
		             ( level.framenum - cl->resp.enterframe ) / 600 );
		j = strlen( entry );

		if ( stringlength + j > 1024 ) {
			break;
		}

		strcpy( string + stringlength, entry );
		stringlength += j;
	}

	gi.WriteByte( svc_layout );
	gi.WriteString( string );
}

/* ======================================================================= */

void G_SetStats( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	/* timers */
	if ( ent->client->invincible_framenum > level.framenum ) {
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex(
		            "p_invulnerability" );
		ent->client->ps.stats[STAT_TIMER] =
		    ( ent->client->invincible_framenum - level.framenum ) / 10;
	} else {
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}

	/* layouts */
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if ( ( ent->client->pers.health <= 0 ) || level.intermissiontime ||
	        ent->client->showscores ) {
		ent->client->ps.stats[STAT_LAYOUTS] |= 1;
	}

	/* frags */
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;
}