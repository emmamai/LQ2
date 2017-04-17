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
 * Entity spawning.
 *
 * =======================================================================
 */

#include "header/local.h"

typedef struct {
	char *name;
	void ( *spawn )( edict_t *ent );
} spawn_t;

void SP_info_player_start( edict_t *ent );
void SP_info_player_deathmatch( edict_t *ent );
void SP_info_player_intermission( edict_t *ent );

void SP_func_plat( edict_t *ent );
void SP_func_rotating( edict_t *ent );
void SP_func_button( edict_t *ent );
void SP_func_door( edict_t *ent );
void SP_func_door_secret( edict_t *ent );
void SP_func_door_rotating( edict_t *ent );
void SP_func_water( edict_t *ent );
void SP_func_train( edict_t *ent );
void SP_func_conveyor( edict_t *self );
void SP_func_wall( edict_t *self );
void SP_func_object( edict_t *self );
void SP_func_timer( edict_t *self );
void SP_func_areaportal( edict_t *ent );
void SP_func_clock( edict_t *ent );
void SP_func_killbox( edict_t *ent );

void SP_trigger_always( edict_t *ent );
void SP_trigger_once( edict_t *ent );
void SP_trigger_multiple( edict_t *ent );
void SP_trigger_relay( edict_t *ent );
void SP_trigger_push( edict_t *ent );
void SP_trigger_hurt( edict_t *ent );
void SP_trigger_counter( edict_t *ent );
void SP_trigger_elevator( edict_t *ent );
void SP_trigger_gravity( edict_t *ent );

void SP_target_temp_entity( edict_t *ent );
void SP_target_speaker( edict_t *ent );
void SP_target_explosion( edict_t *ent );
void SP_target_changelevel( edict_t *ent );
void SP_target_secret( edict_t *ent );
void SP_target_goal( edict_t *ent );
void SP_target_splash( edict_t *ent );
void SP_target_crosslevel_trigger( edict_t *ent );
void SP_target_crosslevel_target( edict_t *ent );
void SP_target_character( edict_t *ent );
void SP_target_string( edict_t *ent );

void SP_worldspawn( edict_t *ent );
void SP_viewthing( edict_t *ent );

void SP_light( edict_t *self );
void SP_light_mine1( edict_t *ent );
void SP_light_mine2( edict_t *ent );
void SP_info_null( edict_t *self );
void SP_info_notnull( edict_t *self );
void SP_path_corner( edict_t *self );

void SP_misc_banner( edict_t *self );
void SP_misc_teleporter( edict_t *self );
void SP_misc_teleporter_dest( edict_t *self );

spawn_t spawns[] = {
	{"info_player_start", SP_info_player_start},
	{"info_player_deathmatch", SP_info_player_deathmatch},
	{"info_player_intermission", SP_info_player_intermission},

	{"func_plat", SP_func_plat},
	{"func_button", SP_func_button},
	{"func_door", SP_func_door},
	{"func_door_secret", SP_func_door_secret},
	{"func_door_rotating", SP_func_door_rotating},
	{"func_rotating", SP_func_rotating},
	{"func_train", SP_func_train},
	{"func_water", SP_func_water},
	{"func_conveyor", SP_func_conveyor},
	{"func_areaportal", SP_func_areaportal},
	{"func_clock", SP_func_clock},
	{"func_wall", SP_func_wall},
	{"func_object", SP_func_object},
	{"func_timer", SP_func_timer},
	{"func_killbox", SP_func_killbox},

	{"trigger_always", SP_trigger_always},
	{"trigger_once", SP_trigger_once},
	{"trigger_multiple", SP_trigger_multiple},
	{"trigger_relay", SP_trigger_relay},
	{"trigger_push", SP_trigger_push},
	{"trigger_hurt", SP_trigger_hurt},
	{"trigger_counter", SP_trigger_counter},
	{"trigger_elevator", SP_trigger_elevator},
	{"trigger_gravity", SP_trigger_gravity},

	{"target_temp_entity", SP_target_temp_entity},
	{"target_speaker", SP_target_speaker},
	{"target_explosion", SP_target_explosion},
	{"target_changelevel", SP_target_changelevel},
	{"target_splash", SP_target_splash},
	{"target_crosslevel_trigger", SP_target_crosslevel_trigger},
	{"target_crosslevel_target", SP_target_crosslevel_target},
	{"target_character", SP_target_character},
	{"target_string", SP_target_string},

	{"worldspawn", SP_worldspawn},
	{"viewthing", SP_viewthing},

	{"light_mine1", SP_light_mine1},
	{"light_mine2", SP_light_mine2},
	{"info_null", SP_info_null},
	{"func_group", SP_info_null},
	{"info_notnull", SP_info_notnull},
	{"path_corner", SP_path_corner},

	{"misc_banner", SP_misc_banner},
	{"misc_teleporter", SP_misc_teleporter},
	{"misc_teleporter_dest", SP_misc_teleporter_dest},

	{NULL, NULL}
};

/*
 * Finds the spawn function for
 * the entity and calls it
 */
void ED_CallSpawn( edict_t *ent ) {
	spawn_t *s;

	if ( !ent ) {
		return;
	}

	if ( !ent->classname ) {
		gi.dprintf( "ED_CallSpawn: NULL classname\n" );
		G_FreeEdict( ent );
		return;
	}

	/* check normal spawn functions */
	for ( s = spawns; s->name; s++ ) {
		if ( !strcmp( s->name, ent->classname ) ) {
			/* found it */
			s->spawn( ent );
			return;
		}
	}

	gi.dprintf( "%s doesn't have a spawn function\n", ent->classname );
}

char *ED_NewString( const char *string ) {
	char *newb, *new_p;
	int i, l;

	if ( !string ) {
		return NULL;
	}

	l = strlen( string ) + 1;

	newb = gi.TagMalloc( l, TAG_LEVEL );

	new_p = newb;

	for ( i = 0; i < l; i++ ) {
		if ( ( string[i] == '\\' ) && ( i < l - 1 ) ) {
			i++;

			if ( string[i] == 'n' ) {
				*new_p++ = '\n';
			} else {
				*new_p++ = '\\';
			}
		} else {
			*new_p++ = string[i];
		}
	}

	return newb;
}

/*
 * Takes a key/value pair and sets
 * the binary values in an edict
 */
void ED_ParseField( const char *key, const char *value, edict_t *ent ) {
	field_t *f;
	byte *b;
	float v;
	vec3_t vec;

	if ( !key || !value ) {
		return;
	}

	for ( f = fields; f->name; f++ ) {
		if ( !( f->flags & FFL_NOSPAWN ) && !Q_strcasecmp( f->name, ( char * )key ) ) {
			/* found it */
			if ( f->flags & FFL_SPAWNTEMP ) {
				b = ( byte * )&st;
			} else {
				b = ( byte * )ent;
			}

			switch ( f->type ) {
			case F_LSTRING:
				*( char ** )( b + f->ofs ) = ED_NewString( value );
				break;
			case F_VECTOR:
				sscanf( value, "%f %f %f", &vec[0], &vec[1], &vec[2] );
				( ( float * )( b + f->ofs ) )[0] = vec[0];
				( ( float * )( b + f->ofs ) )[1] = vec[1];
				( ( float * )( b + f->ofs ) )[2] = vec[2];
				break;
			case F_INT:
				*( int * )( b + f->ofs ) = ( int )strtol( value, ( char ** )NULL, 10 );
				break;
			case F_FLOAT:
				*( float * )( b + f->ofs ) = ( float )strtod( value, ( char ** )NULL );
				break;
			case F_ANGLEHACK:
				v = ( float )strtod( value, ( char ** )NULL );
				( ( float * )( b + f->ofs ) )[0] = 0;
				( ( float * )( b + f->ofs ) )[1] = v;
				( ( float * )( b + f->ofs ) )[2] = 0;
				break;
			case F_IGNORE:
				break;
			default:
				break;
			}

			return;
		}
	}

	gi.dprintf( "%s is not a field\n", key );
}

/*
 * Parses an edict out of the given string,
 * returning the new position ed should be
 * a properly initialized empty edict.
 */
char * ED_ParseEdict( char *data, edict_t *ent ) {
	qboolean init;
	char keyname[256];
	const char *com_token;

	if ( !ent ) {
		return NULL;
	}

	init = false;
	memset( &st, 0, sizeof( st ) );

	/* go through all the dictionary pairs */
	while ( 1 ) {
		/* parse key */
		com_token = COM_Parse( &data );

		if ( com_token[0] == '}' ) {
			break;
		}

		if ( !data ) {
			gi.error( "ED_ParseEntity: EOF without closing brace" );
		}

		Q_strlcpy( keyname, com_token, sizeof( keyname ) );

		/* parse value */
		com_token = COM_Parse( &data );

		if ( !data ) {
			gi.error( "ED_ParseEntity: EOF without closing brace" );
		}

		if ( com_token[0] == '}' ) {
			gi.error( "ED_ParseEntity: closing brace without data" );
		}

		init = true;

		/* keynames with a leading underscore are
		   used for utility comments, and are
		   immediately discarded by quake */
		if ( keyname[0] == '_' ) {
			continue;
		}

		ED_ParseField( keyname, com_token, ent );
	}

	if ( !init ) {
		memset( ent, 0, sizeof( *ent ) );
	}

	return data;
}

/*
 * Creates a server's entity / program execution context by
 * parsing textual entity definitions out of an ent file.
 */
void SpawnEntities( const char *mapname, char *entities, const char *spawnpoint ) {
	edict_t *ent;
	int inhibit;
	const char *com_token;
	int i;

	if ( !mapname || !entities || !spawnpoint ) {
		return;
	}

	SaveClientData();

	gi.FreeTags( TAG_LEVEL );

	memset( &level, 0, sizeof( level ) );
	memset( g_edicts, 0, game.maxentities * sizeof( g_edicts[0] ) );

	Q_strlcpy( level.mapname, mapname, sizeof( level.mapname ) );

	/* set client fields on player ents */
	for ( i = 0; i < game.maxclients; i++ ) {
		g_edicts[i + 1].client = game.clients + i;
	}

	ent = NULL;
	inhibit = 0;

	/* parse ents */
	while ( 1 ) {
		/* parse the opening brace */
		com_token = COM_Parse( &entities );

		if ( !entities ) {
			break;
		}

		if ( com_token[0] != '{' ) {
			gi.error( "ED_LoadFromFile: found %s when expecting {", com_token );
		}

		if ( !ent ) {
			ent = g_edicts;
		} else {
			ent = G_Spawn();
		}

		entities = ED_ParseEdict( entities, ent );

		ED_CallSpawn( ent );
	}

	gi.dprintf( "%i entities inhibited.\n", inhibit );

}

/* =================================================================== */

char *dm_statusbar =
    "yb	-24 "

    /* timer */
    "if 9 "
    "	xv	246 "
    "	num	2	10 "
    "	xv	296 "
    "	pic	9 "
    "endif "

    /*  frags */
    "xr	-50 "
    "yt 2 "
    "num 2 14 "

    /* spectator */
    "if 17 "
    "xv 0 "
    "yb -58 "
    "string2 \"SPECTATOR MODE\" "
    "endif "

    /* chase camera */
    "if 16 "
    "xv 0 "
    "yb -68 "
    "string \"Chasing\" "
    "xv 64 "
    "stat_string 16 "
    "endif "
    ;

/*QUAKED worldspawn (0 0 0) ?
 *
 * Only used for the world.
 *  "sky"		environment map name
 *  "skyaxis"	vector axis for rotating sky
 *  "skyrotate"	speed of rotation in degrees/second
 *  "sounds"	music cd track number
 *  "gravity"	800 is default gravity
 *  "message"	text to print at user logon
 */
void SP_worldspawn( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	ent->inuse = true; /* since the world doesn't use G_Spawn() */
	ent->s.modelindex = 1; /* world model is always index 1 */

	/* --------------- */

	if ( st.nextmap ) {
		strcpy( level.nextmap, st.nextmap );
	}

	/* make some data visible to the server */
	if ( ent->message && ent->message[0] ) {
		gi.configstring( CS_NAME, ent->message );
		Q_strlcpy( level.level_name, ent->message, sizeof( level.level_name ) );
	} else {
		Q_strlcpy( level.level_name, level.mapname, sizeof( level.level_name ) );
	}

	if ( st.sky && st.sky[0] ) {
		gi.configstring( CS_SKY, st.sky );
	} else {
		gi.configstring( CS_SKY, "unit1_" );
	}

	gi.configstring( CS_SKYROTATE, va( "%f", st.skyrotate ) );

	gi.configstring( CS_SKYAXIS, va( "%f %f %f",
	                                 st.skyaxis[0], st.skyaxis[1], st.skyaxis[2] ) );

	gi.configstring( CS_CDTRACK, va( "%i", ent->sounds ) );

	gi.configstring( CS_MAXCLIENTS, va( "%i", ( int )( maxclients->value ) ) );

	gi.configstring( CS_STATUSBAR, dm_statusbar );



	/* --------------- */

	if ( !st.gravity ) {
		gi.cvar_set( "sv_gravity", "800" );
	} else {
		gi.cvar_set( "sv_gravity", st.gravity );
	}

	snd_fry = gi.soundindex( "player/fry.wav" ); /* standing in lava / slime */

	gi.soundindex( "player/lava1.wav" );
	gi.soundindex( "player/lava2.wav" );

	/* sexed sounds */
	gi.soundindex( "*death1.wav" );
	gi.soundindex( "*death2.wav" );
	gi.soundindex( "*death3.wav" );
	gi.soundindex( "*death4.wav" );
	gi.soundindex( "*fall1.wav" );
	gi.soundindex( "*fall2.wav" );
	gi.soundindex( "*gurp1.wav" ); /* drowning damage */
	gi.soundindex( "*gurp2.wav" );
	gi.soundindex( "*jump1.wav" ); /* player jump */
	gi.soundindex( "*pain25_1.wav" );
	gi.soundindex( "*pain25_2.wav" );
	gi.soundindex( "*pain50_1.wav" );
	gi.soundindex( "*pain50_2.wav" );
	gi.soundindex( "*pain75_1.wav" );
	gi.soundindex( "*pain75_2.wav" );
	gi.soundindex( "*pain100_1.wav" );
	gi.soundindex( "*pain100_2.wav" );

	/* sexed models: THIS ORDER MUST MATCH THE DEFINES IN g_local.h
	   you can add more, max 19 */
	gi.modelindex( "#w_shotgun.md2" );

	/* ------------------- */

	gi.soundindex( "player/gasp1.wav" ); /* gasping for air */
	gi.soundindex( "player/gasp2.wav" ); /* head breaking surface, not gasping */

	gi.soundindex( "player/watr_in.wav" ); /* feet hitting water */
	gi.soundindex( "player/watr_out.wav" ); /* feet leaving water */

	gi.soundindex( "player/watr_un.wav" ); /* head going underwater */

	gi.soundindex( "player/u_breath1.wav" );
	gi.soundindex( "player/u_breath2.wav" );

	gi.soundindex( "world/land.wav" ); /* landing thud */
	gi.soundindex( "misc/h2ohit1.wav" ); /* landing splash */

	gi.soundindex( "items/protect.wav" );
	gi.soundindex( "items/protect4.wav" );

	gi.soundindex( "infantry/inflies1.wav" );

	/* Setup light animation tables. 'a'
	   is total darkness, 'z' is doublebright. */

	/* 0 normal */
	gi.configstring( CS_LIGHTS + 0, "m" );

	/* 1 FLICKER (first variety) */
	gi.configstring( CS_LIGHTS + 1, "mmnmmommommnonmmonqnmmo" );

	/* 2 SLOW STRONG PULSE */
	gi.configstring( CS_LIGHTS + 2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba" );

	/* 3 CANDLE (first variety) */
	gi.configstring( CS_LIGHTS + 3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg" );

	/* 4 FAST STROBE */
	gi.configstring( CS_LIGHTS + 4, "mamamamamama" );

	/* 5 GENTLE PULSE 1 */
	gi.configstring( CS_LIGHTS + 5, "jklmnopqrstuvwxyzyxwvutsrqponmlkj" );

	/* 6 FLICKER (second variety) */
	gi.configstring( CS_LIGHTS + 6, "nmonqnmomnmomomno" );

	/* 7 CANDLE (second variety) */
	gi.configstring( CS_LIGHTS + 7, "mmmaaaabcdefgmmmmaaaammmaamm" );

	/* 8 CANDLE (third variety) */
	gi.configstring( CS_LIGHTS + 8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa" );

	/* 9 SLOW STROBE (fourth variety) */
	gi.configstring( CS_LIGHTS + 9, "aaaaaaaazzzzzzzz" );

	/* 10 FLUORESCENT FLICKER */
	gi.configstring( CS_LIGHTS + 10, "mmamammmmammamamaaamammma" );

	/* 11 SLOW PULSE NOT FADE TO BLACK */
	gi.configstring( CS_LIGHTS + 11, "abcdefghijklmnopqrrqponmlkjihgfedcba" );

	/* styles 32-62 are assigned by the light program for switchable lights */

	/* 63 testing */
	gi.configstring( CS_LIGHTS + 63, "a" );
}
