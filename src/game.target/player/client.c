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
 * Interface between client <-> game and client calculations.
 *
 * =======================================================================
 */

#include "../header/local.h"
#include "player.h"

void ClientUserinfoChanged( edict_t *ent, char *userinfo );
void SP_misc_teleporter_dest( edict_t *ent );

/*
 * QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
 * The normal starting point for a level.
 */
void SP_info_player_start( edict_t *self ) {
}

/*
 * QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32)
 * potential spawning position for deathmatch games
 */
void SP_info_player_deathmatch( edict_t *self ) {
	if ( !self ) {
		return;
	}

	SP_misc_teleporter_dest( self );
}

/*
 * QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
 * The deathmatch intermission point will be at one of these
 * Use 'angles' instead of 'angle', so you can set pitch or
 * roll as well as yaw.  'pitch yaw roll'
 */
void SP_info_player_intermission( void ) {
}

/* ======================================================================= */

void
player_pain( edict_t *self, edict_t *other,float kick, int damage ) {
}

qboolean IsFemale( edict_t *ent ) {
	char *info;

	if ( !ent ) {
		return false;
	}

	if ( !ent->client ) {
		return false;
	}

	info = Info_ValueForKey( ent->client->pers.userinfo, "gender" );

	if ( strstr( info, "crakhor" ) ) {
		return true;
	}

	if ( ( info[0] == 'f' ) || ( info[0] == 'F' ) ) {
		return true;
	}

	return false;
}

qboolean IsNeutral( edict_t *ent ) {
	char *info;

	if ( !ent ) {
		return false;
	}

	if ( !ent->client ) {
		return false;
	}

	info = Info_ValueForKey( ent->client->pers.userinfo, "gender" );

	if ( strstr( info, "crakhor" ) ) {
		return false;
	}

	if ( ( info[0] != 'f' ) && ( info[0] != 'F' ) && ( info[0] != 'm' ) && ( info[0] != 'M' ) ) {
		return true;
	}

	return false;
}

void ClientObituary( edict_t *self, edict_t *inflictor /* unused */, edict_t *attacker ) {
	int mod;
	char *message;
	char *message2;
	qboolean ff;

	if ( !self || !inflictor ) {
		return;
	}

	ff = meansOfDeath & MOD_FRIENDLY_FIRE;
	mod = meansOfDeath & ~MOD_FRIENDLY_FIRE;
	message = NULL;
	message2 = "";

	switch ( mod ) {
	case MOD_SUICIDE:
		message = "suicides";
		break;
	case MOD_FALLING:
		message = "cratered";
		break;
	case MOD_CRUSH:
		message = "was squished";
		break;
	case MOD_WATER:
		message = "sank like a rock";
		break;
	case MOD_SLIME:
		message = "melted";
		break;
	case MOD_LAVA:
		message = "does a back flip into the lava";
		break;
	case MOD_EXPLOSIVE:
	case MOD_BARREL:
		message = "blew up";
		break;
	case MOD_EXIT:
		message = "found a way out";
		break;
	case MOD_TARGET_LASER:
		message = "saw the light";
		break;
	case MOD_TARGET_BLASTER:
		message = "got blasted";
		break;
	case MOD_BOMB:
	case MOD_SPLASH:
	case MOD_TRIGGER_HURT:
		message = "was in the wrong place";
		break;
	}

	if ( attacker == self ) {
		if ( IsNeutral( self ) ) {
			message = "killed itself";
		} else if ( IsFemale( self ) ) {
			message = "killed herself";
		} else {
			message = "killed himself";
		}
	}

	if ( message ) {
		gi.bprintf( PRINT_MEDIUM, "%s %s.\n",
		            self->client->pers.netname,
		            message );

		self->client->resp.score--;

		self->enemy = NULL;
		return;
	}

	self->enemy = attacker;

	if ( attacker && attacker->client ) {
		switch ( mod ) {
		case MOD_RAILGUN:
			message = "was railed by";
			break;
		case MOD_TELEFRAG:
			message = "tried to invade";
			message2 = "'s personal space";
			break;
		}

		if ( message ) {
			gi.bprintf( PRINT_MEDIUM, "%s %s %s%s\n",
			            self->client->pers.netname,
			            message, attacker->client->pers.netname,
			            message2 );

			if ( ff ) {
				attacker->client->resp.score--;
			} else {
				attacker->client->resp.score++;
			}
			return;
		}
	}

	gi.bprintf( PRINT_MEDIUM, "%s died.\n", self->client->pers.netname );
	self->client->resp.score--;
}

void LookAtKiller( edict_t *self, edict_t *inflictor, edict_t *attacker ) {
	vec3_t dir;

	if ( !self || !inflictor || !attacker ) {
		return;
	}

	if ( attacker && ( attacker != world ) && ( attacker != self ) ) {
		VectorSubtract( attacker->s.origin, self->s.origin, dir );
	} else if ( inflictor && ( inflictor != world ) && ( inflictor != self ) ) {
		VectorSubtract( inflictor->s.origin, self->s.origin, dir );
	} else {
		self->client->killer_yaw = self->s.angles[YAW];
		return;
	}

	if ( dir[0] ) {
		self->client->killer_yaw = 180 / M_PI *atan2( dir[1], dir[0] );
	} else {
		self->client->killer_yaw = 0;

		if ( dir[1] > 0 ) {
			self->client->killer_yaw = 90;
		} else if ( dir[1] < 0 ) {
			self->client->killer_yaw = -90;
		}
	}

	if ( self->client->killer_yaw < 0 ) {
		self->client->killer_yaw += 360;
	}
}

void
player_die( edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point /* unused */ ) {
	if ( !self || !inflictor || !attacker ) {
		return;
	}

	VectorClear( self->avelocity );

	gi.WriteByte( svc_temp_entity );
	gi.WriteByte( TE_BFG_BIGEXPLOSION );
	gi.WritePosition( self->s.origin );
	gi.multicast( self->s.origin, MULTICAST_PVS );

	self->takedamage = DAMAGE_YES;
	self->movetype = MOVETYPE_TOSS;

	self->s.modelindex2 = 0; /* remove linked weapon model */

	self->s.angles[0] = 0;
	self->s.angles[2] = 0;

	self->s.sound = 0;

	self->maxs[2] = -8;

	self->svflags |= SVF_DEADMONSTER;

	if ( !self->deadflag ) {
		self->client->respawn_time = level.time + 1.0;
		LookAtKiller( self, inflictor, attacker );
		self->client->ps.pmove.pm_type = PM_DEAD;
		ClientObituary( self, inflictor, attacker );

		Cmd_Score_f( self ); /* show scores */
	}

	/* remove powerups */
	self->client->invincible_framenum = 0;

	gi.sound( self, CHAN_BODY, gi.soundindex(
	              "world/spark3.wav" ), 1, ATTN_NORM, 0 );

	ThrowClientHead( self, damage );

	self->takedamage = DAMAGE_NO;
	self->deadflag = DEAD_DEAD;

	gi.linkentity( self );
}

/* ======================================================================= */

/*
 * This is only called when the game first
 * initializes in single player, but is called
 * after each death and level change in deathmatch
 */
void InitClientPersistant( gclient_t *client ) {
	if ( !client ) {
		return;
	}

	memset( &client->pers, 0, sizeof( client->pers ) );

	client->pers.weapon = true;

	client->pers.health = 100;
	client->pers.max_health = 100;

	client->pers.connected = true;
}

void InitClientResp( gclient_t *client ) {
	if ( !client ) {
		return;
	}

	memset( &client->resp, 0, sizeof( client->resp ) );
	client->resp.enterframe = level.framenum;
	client->resp.coop_respawn = client->pers;
}

/*
 * Some information that should be persistant, like health,
 * is still stored in the edict structure, so it needs to
 * be mirrored out to the client structure before all the
 * edicts are wiped.
 */
void SaveClientData( void ) {
	int i;
	edict_t *ent;

	for ( i = 0; i < game.maxclients; i++ ) {
		ent = &g_edicts[1 + i];

		if ( !ent->inuse ) {
			continue;
		}

		game.clients[i].pers.health = ent->health;
		game.clients[i].pers.max_health = ent->max_health;
		game.clients[i].pers.savedFlags =
		    ( ent->flags & ( FL_GODMODE | FL_NOTARGET ) );

	}
}

void FetchClientEntData( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->health = ent->client->pers.health;
	ent->max_health = ent->client->pers.max_health;
	ent->flags |= ent->client->pers.savedFlags;
}

/* ======================================================================= */

/*
 * Returns the distance to the
 * nearest player from the given spot
 */
float PlayersRangeFromSpot( edict_t *spot ) {
	edict_t *player;
	float bestplayerdistance;
	vec3_t v;
	int n;
	float playerdistance;

	if ( !spot ) {
		return 0;
	}

	bestplayerdistance = 9999999;

	for ( n = 1; n <= maxclients->value; n++ ) {
		player = &g_edicts[n];

		if ( !player->inuse ) {
			continue;
		}
		if ( player->health <= 0 ) {
			continue;
		}

		VectorSubtract( spot->s.origin, player->s.origin, v );
		playerdistance = VectorLength( v );

		if ( playerdistance < bestplayerdistance ) {
			bestplayerdistance = playerdistance;
		}
	}

	return bestplayerdistance;
}

/*
 * go to a random point, but NOT the two
 * points closest to other players
 */
edict_t *SelectRandomDeathmatchSpawnPoint( void ) {
	edict_t *spot, *spot1, *spot2;
	int count = 0;
	int selection;
	float range, range1, range2;

	spot = NULL;
	range1 = range2 = 99999;
	spot1 = spot2 = NULL;

	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		count++;
		range = PlayersRangeFromSpot( spot );

		if ( range < range1 ) {
			range1 = range;
			spot1 = spot;
		} else if ( range < range2 ) {
			range2 = range;
			spot2 = spot;
		}
	}

	if ( !count ) {
		return NULL;
	}

	if ( count <= 2 ) {
		spot1 = spot2 = NULL;
	} else {
		if ( spot1 ) {
			count--;
		}
		if ( spot2 ) {
			count--;
		}
	}

	selection = randk() % count;

	spot = NULL;

	do {
		spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" );

		if ( ( spot == spot1 ) || ( spot == spot2 ) ) {
			selection++;
		}
	} while ( selection-- );

	return spot;
}

edict_t *SelectFarthestDeathmatchSpawnPoint( void ) {
	edict_t *bestspot;
	float bestdistance, bestplayerdistance;
	edict_t *spot;

	spot = NULL;
	bestspot = NULL;
	bestdistance = 0;

	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		bestplayerdistance = PlayersRangeFromSpot( spot );

		if ( bestplayerdistance > bestdistance ) {
			bestspot = spot;
			bestdistance = bestplayerdistance;
		}
	}

	if ( bestspot ) {
		return bestspot;
	}

	/* if there is a player just spawned on each and every start spot/
	   we have no choice to turn one into a telefrag meltdown */
	spot = G_Find( NULL, FOFS( classname ), "info_player_deathmatch" );

	return spot;
}

edict_t *SelectDeathmatchSpawnPoint( void ) {
	if ( ( int )( dmflags->value ) & DF_SPAWN_FARTHEST ) {
		return SelectFarthestDeathmatchSpawnPoint();
	} else {
		return SelectRandomDeathmatchSpawnPoint();
	}
}

/*
 * Chooses a player start, deathmatch start, etc
 */
void SelectSpawnPoint( edict_t *ent, vec3_t origin, vec3_t angles ) {
	edict_t *spot = NULL;

	if ( !ent ) {
		return;
	}

	spot = SelectDeathmatchSpawnPoint();

	if ( !spot ) {
		gi.error( "Couldn't find spawn point\n" );
	}

	VectorCopy( spot->s.origin, origin );
	origin[2] += 9;
	VectorCopy( spot->s.angles, angles );
}

/* ====================================================================== */

void respawn( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->svflags &= ~SVF_NOCLIENT;
	PutClientInServer( self );

	/* add a teleportation effect */
	self->s.event = EV_PLAYER_TELEPORT;

	/* hold in place briefly */
	self->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
	self->client->ps.pmove.pm_time = 14;

	self->client->respawn_time = level.time;

	return;
}

/*
 * only called when pers.spectator changes
 * note that resp.spectator should be the
 * opposite of pers.spectator here
 */
void spectator_respawn( edict_t *ent ) {
	int i, numspec;

	if ( !ent ) {
		return;
	}

	/* if the user wants to become a spectator,
	   make sure he doesn't exceed max_spectators */
	if ( ent->client->pers.spectator ) {
		char *value = Info_ValueForKey( ent->client->pers.userinfo, "spectator" );

		if ( *spectator_password->string &&
		        strcmp( spectator_password->string, "none" ) &&
		        strcmp( spectator_password->string, value ) ) {
			gi.cprintf( ent, PRINT_HIGH, "Spectator password incorrect.\n" );
			ent->client->pers.spectator = false;
			gi.WriteByte( svc_stufftext );
			gi.WriteString( "spectator 0\n" );
			gi.unicast( ent, true );
			return;
		}

		/* count spectators */
		for ( i = 1, numspec = 0; i <= maxclients->value; i++ ) {
			if ( g_edicts[i].inuse && g_edicts[i].client->pers.spectator ) {
				numspec++;
			}
		}

		if ( numspec >= maxspectators->value ) {
			gi.cprintf( ent, PRINT_HIGH, "Server spectator limit is full." );
			ent->client->pers.spectator = false;

			/* reset his spectator var */
			gi.WriteByte( svc_stufftext );
			gi.WriteString( "spectator 0\n" );
			gi.unicast( ent, true );
			return;
		}
	} else {
		/* he was a spectator and wants to join the
		   game he must have the right password */
		char *value = Info_ValueForKey( ent->client->pers.userinfo, "password" );

		if ( *password->string && strcmp( password->string, "none" ) &&
		        strcmp( password->string, value ) ) {
			gi.cprintf( ent, PRINT_HIGH, "Password incorrect.\n" );
			ent->client->pers.spectator = true;
			gi.WriteByte( svc_stufftext );
			gi.WriteString( "spectator 1\n" );
			gi.unicast( ent, true );
			return;
		}
	}

	/* clear client on respawn */
	ent->client->resp.score = 0;

	ent->svflags &= ~SVF_NOCLIENT;
	PutClientInServer( ent );

	/* add a teleportation effect */
	if ( !ent->client->pers.spectator ) {
		/* send effect */
		gi.WriteByte( svc_muzzleflash );
		gi.WriteShort( ent - g_edicts );
		gi.WriteByte( MZ_LOGIN );
		gi.multicast( ent->s.origin, MULTICAST_PVS );

		/* hold in place briefly */
		ent->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
		ent->client->ps.pmove.pm_time = 14;
	}

	ent->client->respawn_time = level.time;

	if ( ent->client->pers.spectator ) {
		gi.bprintf( PRINT_HIGH, "%s has moved to the sidelines\n",
		            ent->client->pers.netname );
	} else {
		gi.bprintf( PRINT_HIGH, "%s joined the game\n",
		            ent->client->pers.netname );
	}
}

/* ============================================================== */

/*
 * Called when a player connects to
 * a server or respawns in a deathmatch.
 */
void PutClientInServer( edict_t *ent ) {
	char userinfo[MAX_INFO_STRING];

	if ( !ent ) {
		return;
	}

	vec3_t mins = {-16, -16, -24};
	vec3_t maxs = {16, 16, 32};
	int index;
	vec3_t spawn_origin, spawn_angles;
	gclient_t *client;
	int i;
	client_persistant_t saved;
	client_respawn_t resp;

	/* find a spawn point do it before setting
	   health back up, so farthest ranging
	   doesn't count this client */
	SelectSpawnPoint( ent, spawn_origin, spawn_angles );

	index = ent - g_edicts - 1;
	client = ent->client;

	/* deathmatch wipes most client data every spawn */
	resp = client->resp;
	memcpy( userinfo, client->pers.userinfo, sizeof( userinfo ) );
	InitClientPersistant( client );
	ClientUserinfoChanged( ent, userinfo );

	/* clear everything but the persistant data */
	saved = client->pers;
	memset( client, 0, sizeof( *client ) );
	client->pers = saved;

	if ( client->pers.health <= 0 ) {
		InitClientPersistant( client );
	}

	client->resp = resp;

	/* copy some data from the client to the entity */
	FetchClientEntData( ent );

	/* clear entity values */
	ent->groundentity = NULL;
	ent->client = &game.clients[index];
	ent->takedamage = DAMAGE_AIM;
	ent->movetype = MOVETYPE_WALK;
	ent->viewheight = 22;
	ent->inuse = true;
	ent->classname = "player";
	ent->mass = 200;
	ent->solid = SOLID_BBOX;
	ent->deadflag = DEAD_NO;
	ent->air_finished = level.time + 12;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->model = "players/male/tris.md2";
	ent->pain = player_pain;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags &= ~FL_NO_KNOCKBACK;
	ent->svflags = 0;

	VectorCopy( mins, ent->mins );
	VectorCopy( maxs, ent->maxs );
	VectorClear( ent->velocity );

	/* clear playerstate values */
	memset( &ent->client->ps, 0, sizeof( client->ps ) );

	client->ps.pmove.origin[0] = spawn_origin[0] * 8;
	client->ps.pmove.origin[1] = spawn_origin[1] * 8;
	client->ps.pmove.origin[2] = spawn_origin[2] * 8;

	if ( ( int )dmflags->value & DF_FIXED_FOV ) {
		client->ps.fov = 90;
	} else {
		client->ps.fov = ( int )strtol( Info_ValueForKey( client->pers.userinfo, "fov" ), ( char ** )NULL, 10 );

		if ( client->ps.fov < 1 ) {
			client->ps.fov = 90;
		} else if ( client->ps.fov > 160 ) {
			client->ps.fov = 160;
		}
	}

	/* clear entity state values */
	ent->s.effects = 0;
	ent->s.modelindex = 255; /* will use the skin specified model */
	ent->s.modelindex2 = 255; /* custom gun model */

	/* sknum is player num and weapon number
	   weapon number will be added in changeweapon */
	ent->s.skinnum = ent - g_edicts - 1;

	ent->s.frame = 0;
	VectorCopy( spawn_origin, ent->s.origin );
	ent->s.origin[2] += 1;  /* make sure off ground */
	VectorCopy( ent->s.origin, ent->s.old_origin );

	/* set the delta angle */
	for ( i = 0; i < 3; i++ ) {
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(
		                                       spawn_angles[i] - client->resp.cmd_angles[i] );
	}

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy( ent->s.angles, client->ps.viewangles );
	VectorCopy( ent->s.angles, client->v_angle );

	/* add spawn invuln */

	client->invincible_framenum = level.framenum + 15;

	/* spawn a spectator */
	if ( client->pers.spectator ) {
		client->chase_target = NULL;

		client->resp.spectator = true;

		ent->movetype = MOVETYPE_NOCLIP;
		ent->solid = SOLID_NOT;
		ent->svflags |= SVF_NOCLIENT;
		ent->client->ps.gunindex = 0;
		gi.linkentity( ent );
		return;
	} else {
		client->resp.spectator = false;
	}

	if ( !KillBox( ent ) ) {
		/* could't spawn in? */
	}

	gi.linkentity( ent );

	/* force the current weapon up */
	client->newweapon = client->pers.weapon;
	ChangeWeapon( ent );
}

/*
 * A client has just connected to the server in
 * deathmatch mode, so clear everything out before
 * starting them.
 */
void ClientBeginDeathmatch( edict_t *ent ) {
	if ( !ent ) {
		return;
	}


}

/*
 * called when a client has finished connecting, and is ready
 * to be placed into the game.  This will happen every level load.
 */
void ClientBegin( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->client = game.clients + ( ent - g_edicts - 1 );

	G_InitEdict( ent );

	InitClientResp( ent->client );

	/* locate ent at a spawn point */
	PutClientInServer( ent );

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	} else {
		/* send effect */
		gi.WriteByte( svc_muzzleflash );
		gi.WriteShort( ent - g_edicts );
		gi.WriteByte( MZ_LOGIN );
		gi.multicast( ent->s.origin, MULTICAST_PVS );
	}

	gi.bprintf( PRINT_HIGH, "%s entered the game\n", ent->client->pers.netname );

	/* make sure all view stuff is valid */
	ClientEndServerFrame( ent );
}

/*
 * Called whenever the player updates a userinfo variable.
 * The game can override any of the settings in place
 * (forcing skins or names, etc) before copying it off.
 */
void ClientUserinfoChanged( edict_t *ent, char *userinfo ) {
	char *s;
	int playernum;

	if ( !ent || !userinfo ) {
		return;
	}

	/* check for malformed or illegal info strings */
	if ( !Info_Validate( userinfo ) ) {
		strcpy( userinfo, "\\name\\badinfo\\skin\\male/grunt" );
	}

	/* set name */
	s = Info_ValueForKey( userinfo, "name" );
	Q_strlcpy( ent->client->pers.netname, s, sizeof( ent->client->pers.netname ) );

	/* set spectator */
	s = Info_ValueForKey( userinfo, "spectator" );

	/* spectators are only supported in deathmatch */
	if ( *s && strcmp( s, "0" ) ) {
		ent->client->pers.spectator = true;
	} else {
		ent->client->pers.spectator = false;
	}

	/* set skin */
	s = Info_ValueForKey( userinfo, "skin" );

	playernum = ent - g_edicts - 1;

	/* combine name and skin into a configstring */
	gi.configstring( CS_PLAYERSKINS + playernum,
	                 va( "%s\\%s", ent->client->pers.netname, s ) );

	/* fov */
	if ( ( int )dmflags->value & DF_FIXED_FOV ) {
		ent->client->ps.fov = 90;
	} else {
		ent->client->ps.fov = ( int )strtol( Info_ValueForKey( userinfo, "fov" ), ( char ** )NULL, 10 );

		if ( ent->client->ps.fov < 1 ) {
			ent->client->ps.fov = 90;
		} else if ( ent->client->ps.fov > 160 ) {
			ent->client->ps.fov = 160;
		}
	}

	/* handedness */
	s = Info_ValueForKey( userinfo, "hand" );

	if ( strlen( s ) ) {
		ent->client->pers.hand = ( int )strtol( s, ( char ** )NULL, 10 );
	}

	/* save off the userinfo in case we want to check something later */
	Q_strlcpy( ent->client->pers.userinfo, userinfo, sizeof( ent->client->pers.userinfo ) );
}

/*
 * Called when a player begins connecting to the server.
 * The game can refuse entrance to a client by returning false.
 * If the client is allowed, the connection process will continue
 * and eventually get to ClientBegin(). Changing levels will NOT
 * cause this to be called again, but loadgames will.
 */
qboolean ClientConnect( edict_t *ent, char *userinfo ) {
	char *value;

	if ( !ent || !userinfo ) {
		return false;
	}

	/* check for a spectator */
	value = Info_ValueForKey( userinfo, "spectator" );

	if ( *value && strcmp( value, "0" ) ) {
		int i, numspec;

		if ( *spectator_password->string &&
		        strcmp( spectator_password->string, "none" ) &&
		        strcmp( spectator_password->string, value ) ) {
			Info_SetValueForKey( userinfo, "rejmsg",
			                     "Spectator password required or incorrect." );
			return false;
		}

		/* count spectators */
		for ( i = numspec = 0; i < maxclients->value; i++ ) {
			if ( g_edicts[i + 1].inuse && g_edicts[i + 1].client->pers.spectator ) {
				numspec++;
			}
		}

		if ( numspec >= maxspectators->value ) {
			Info_SetValueForKey( userinfo, "rejmsg", "Server spectator limit is full." );
			return false;
		}
	} else {
		/* check for a password */
		value = Info_ValueForKey( userinfo, "password" );

		if ( *password->string && strcmp( password->string, "none" ) && strcmp( password->string, value ) ) {
			Info_SetValueForKey( userinfo, "rejmsg", "Password required or incorrect." );
			return false;
		}
	}

	/* they can connect */
	ent->client = game.clients + ( ent - g_edicts - 1 );

	/* if there is already a body waiting for us (a loadgame),
	   just take it, otherwise spawn one from scratch */
	if ( ent->inuse == false ) {
		/* clear the respawning variables */
		InitClientResp( ent->client );

		if ( !game.autosaved || !ent->client->pers.weapon ) {
			InitClientPersistant( ent->client );
		}
	}

	ClientUserinfoChanged( ent, userinfo );

	if ( game.maxclients > 1 ) {
		gi.dprintf( "%s connected\n", ent->client->pers.netname );
	}

	ent->svflags = 0; /* make sure we start with known default */
	ent->client->pers.connected = true;
	return true;
}

/*
 * Called when a player drops from the server.
 * Will not be called between levels.
 */
void ClientDisconnect( edict_t *ent ) {
	int playernum;

	if ( !ent ) {
		return;
	}

	if ( !ent->client ) {
		return;
	}

	gi.bprintf( PRINT_HIGH, "%s disconnected\n", ent->client->pers.netname );

	/* send effect */
	gi.WriteByte( svc_muzzleflash );
	gi.WriteShort( ent - g_edicts );
	gi.WriteByte( MZ_LOGOUT );
	gi.multicast( ent->s.origin, MULTICAST_PVS );

	gi.unlinkentity( ent );
	ent->s.modelindex = 0;
	ent->solid = SOLID_NOT;
	ent->inuse = false;
	ent->classname = "disconnected";
	ent->client->pers.connected = false;

	playernum = ent - g_edicts - 1;
	gi.configstring( CS_PLAYERSKINS + playernum, "" );
}

/* ============================================================== */

edict_t *pm_passent;

/*
 * pmove doesn't need to know
 * about passent and contentmask
 */
trace_t PM_trace( vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end ) {
	if ( pm_passent->health > 0 ) {
		return gi.trace( start, mins, maxs, end, pm_passent, MASK_PLAYERSOLID );
	} else {
		return gi.trace( start, mins, maxs, end, pm_passent, MASK_DEADSOLID );
	}
}

unsigned CheckBlock( void *b, int c ) {
	int v, i;

	if ( !b ) {
		return 0;
	}

	v = 0;

	for ( i = 0; i < c; i++ ) {
		v += ( ( byte * )b )[i];
	}

	return v;
}

void PrintPmove( pmove_t *pm ) {
	unsigned c1, c2;

	if ( !pm ) {
		return;
	}

	c1 = CheckBlock( &pm->s, sizeof( pm->s ) );
	c2 = CheckBlock( &pm->cmd, sizeof( pm->cmd ) );
	gi.dprintf( "sv %3i:%i %i\n", pm->cmd.impulse, c1, c2 );
}

/*
 * This will be called once for each client frame, which will
 * usually be a couple times for each server frame.
 */
void ClientThink( edict_t *ent, usercmd_t *ucmd ) {
	gclient_t *client;
	edict_t *other;
	int i, j;
	pmove_t pm;

	if ( !ent || !ucmd ) {
		return;
	}

	level.current_entity = ent;
	client = ent->client;

	if ( level.intermissiontime ) {
		client->ps.pmove.pm_type = PM_FREEZE;

		/* can exit intermission after five seconds */
		if ( ( level.time > level.intermissiontime + 5.0 ) && ( ucmd->buttons & BUTTON_ANY ) ) {
			level.exitintermission = true;
		}

		return;
	}

	pm_passent = ent;

	if ( ent->client->chase_target ) {
		client->resp.cmd_angles[0] = SHORT2ANGLE( ucmd->angles[0] );
		client->resp.cmd_angles[1] = SHORT2ANGLE( ucmd->angles[1] );
		client->resp.cmd_angles[2] = SHORT2ANGLE( ucmd->angles[2] );
	} else {
		/* set up for pmove */
		memset( &pm, 0, sizeof( pm ) );

		if ( ent->movetype == MOVETYPE_NOCLIP ) {
			client->ps.pmove.pm_type = PM_SPECTATOR;
		} else if ( ent->s.modelindex != 255 ) {
			client->ps.pmove.pm_type = PM_GIB;
		} else if ( ent->deadflag ) {
			client->ps.pmove.pm_type = PM_DEAD;
		} else {
			client->ps.pmove.pm_type = PM_NORMAL;
		}

		client->ps.pmove.gravity = sv_gravity->value;
		pm.s = client->ps.pmove;

		for ( i = 0; i < 3; i++ ) {
			pm.s.origin[i] = ent->s.origin[i] * 8;
			/* save to an int first, in case the short overflows
			 * so we get defined behavior (at least with -fwrapv) */
			int tmpVel = ent->velocity[i] * 8;
			pm.s.velocity[i] = tmpVel;
		}

		if ( memcmp( &client->old_pmove, &pm.s, sizeof( pm.s ) ) ) {
			pm.snapinitial = true;
		}

		pm.cmd = *ucmd;

		pm.trace = PM_trace; /* adds default parms */
		pm.pointcontents = gi.pointcontents;

		/* perform a pmove */
		gi.Pmove( &pm );

		/* save results of pmove */
		client->ps.pmove = pm.s;
		client->old_pmove = pm.s;

		for ( i = 0; i < 3; i++ ) {
			ent->s.origin[i] = pm.s.origin[i] * 0.125;
			ent->velocity[i] = pm.s.velocity[i] * 0.125;
		}

		VectorCopy( pm.mins, ent->mins );
		VectorCopy( pm.maxs, ent->maxs );

		client->resp.cmd_angles[0] = SHORT2ANGLE( ucmd->angles[0] );
		client->resp.cmd_angles[1] = SHORT2ANGLE( ucmd->angles[1] );
		client->resp.cmd_angles[2] = SHORT2ANGLE( ucmd->angles[2] );

		if ( ent->groundentity && !pm.groundentity && ( pm.cmd.upmove >= 10 ) && ( pm.waterlevel == 0 ) ) {
			gi.sound( ent, CHAN_VOICE, gi.soundindex(
			              "*jump1.wav" ), 1, ATTN_NORM, 0 );
		}

		ent->viewheight = pm.viewheight;
		ent->waterlevel = pm.waterlevel;
		ent->watertype = pm.watertype;
		ent->groundentity = pm.groundentity;

		if ( pm.groundentity ) {
			ent->groundentity_linkcount = pm.groundentity->linkcount;
		}

		if ( ent->deadflag ) {
			client->ps.viewangles[ROLL] = 40;
			client->ps.viewangles[PITCH] = -15;
			client->ps.viewangles[YAW] = client->killer_yaw;
		} else {
			VectorCopy( pm.viewangles, client->v_angle );
			VectorCopy( pm.viewangles, client->ps.viewangles );
		}

		gi.linkentity( ent );

		if ( ent->movetype != MOVETYPE_NOCLIP ) {
			G_TouchTriggers( ent );
		}

		/* touch other objects */
		for ( i = 0; i < pm.numtouch; i++ ) {
			other = pm.touchents[i];

			for ( j = 0; j < i; j++ ) {
				if ( pm.touchents[j] == other ) {
					break;
				}
			}

			if ( j != i ) {
				continue; /* duplicated */
			}

			if ( !other->touch ) {
				continue;
			}

			other->touch( other, ent, NULL, NULL );
		}
	}

	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons |= client->buttons & ~client->oldbuttons;

	/* save light level the player is standing
	   on for monster sighting AI */
	ent->light_level = ucmd->lightlevel;

	/* fire weapon from final position if needed */
	if ( client->latched_buttons & BUTTON_ATTACK ) {
		if ( client->resp.spectator ) {
			client->latched_buttons = 0;

			if ( client->chase_target ) {
				client->chase_target = NULL;
				client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			} else {
				GetChaseTarget( ent );
			}
		} else if ( !client->weapon_thunk ) {
			client->weapon_thunk = true;
			Think_Weapon( ent );
		}
	}

	if ( client->resp.spectator ) {
		if ( ucmd->upmove >= 10 ) {
			if ( !( client->ps.pmove.pm_flags & PMF_JUMP_HELD ) ) {
				client->ps.pmove.pm_flags |= PMF_JUMP_HELD;

				if ( client->chase_target ) {
					ChaseNext( ent );
				} else {
					GetChaseTarget( ent );
				}
			}
		} else {
			client->ps.pmove.pm_flags &= ~PMF_JUMP_HELD;
		}
	}

	/* update chase cam if being followed */
	for ( i = 1; i <= maxclients->value; i++ ) {
		other = g_edicts + i;

		if ( other->inuse && ( other->client->chase_target == ent ) ) {
			UpdateChaseCam( other );
		}
	}
}

/*
 * This will be called once for each server
 * frame, before running any other entities
 * in the world.
 */
void ClientBeginServerFrame( edict_t *ent ) {
	gclient_t *client;
	int buttonMask;

	if ( !ent ) {
		return;
	}

	if ( level.intermissiontime ) {
		return;
	}

	client = ent->client;

	if ( ( client->pers.spectator != client->resp.spectator ) && ( ( level.time - client->respawn_time ) >= 5 ) ) {
		spectator_respawn( ent );
		return;
	}

	/* run weapon animations if it hasn't been done by a ucmd_t */
	if ( !client->weapon_thunk && !client->resp.spectator ) {
		Think_Weapon( ent );
	} else {
		client->weapon_thunk = false;
	}

	if ( ent->deadflag ) {
		/* wait for any button just going down */
		if ( level.time > client->respawn_time ) {
			/* in deathmatch, only wait for attack button */
			buttonMask = BUTTON_ATTACK;

			if ( ( client->latched_buttons & buttonMask ) || ( ( int )dmflags->value & DF_FORCE_RESPAWN ) ) {
				respawn( ent );
				client->latched_buttons = 0;
			}
		}

		return;
	}

	client->latched_buttons = 0;
}
