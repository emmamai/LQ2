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
 * Targets.
 *
 * =======================================================================
 */

#include "header/local.h"

/*
 * QUAKED target_temp_entity (1 0 0) (-8 -8 -8) (8 8 8)
 * Fire an origin based temp entity event to the clients.
 *
 *  "style"	type byte
 */
void
Use_Target_Tent( edict_t *ent, edict_t *other /* unused */, edict_t *activator /* unused */ ) {
	if ( !ent ) {
		return;
	}

	gi.WriteByte( svc_temp_entity );
	gi.WriteByte( ent->style );
	gi.WritePosition( ent->s.origin );
	gi.multicast( ent->s.origin, MULTICAST_PVS );
}

void
SP_target_temp_entity( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->use = Use_Target_Tent;
}

/* ========================================================== */

/*
 * QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) looped-on looped-off reliable
 *
 * "noise" wav file to play
 *
 * "attenuation"
 *   -1 = none, send to whole level
 *    1 = normal fighting sounds
 *    2 = idle sound level
 *    3 = ambient sound level
 *
 * "volume"	0.0 to 1.0
 *
 * Normal sounds play each time the target is used.
 * The reliable flag can be set for crucial voiceovers.
 *
 * Looped sounds are always atten 3 / vol 1, and the use function toggles it on/off.
 * Multiple identical looping sounds will just increase volume without any speed cost.
 */
void
Use_Target_Speaker( edict_t *ent, edict_t *other /* unused */, edict_t *activator /* unused */ ) {
	int chan;

	if ( !ent ) {
		return;
	}

	if ( ent->spawnflags & 3 ) {
		/* looping sound toggles */
		if ( ent->s.sound ) {
			ent->s.sound = 0; /* turn it off */
		} else {
			ent->s.sound = ent->noise_index; /* start it */
		}
	} else {
		/* normal sound */
		if ( ent->spawnflags & 4 ) {
			chan = CHAN_VOICE | CHAN_RELIABLE;
		} else {
			chan = CHAN_VOICE;
		}

		/* use a positioned_sound, because this entity won't
		   normally be sent to any clients because it is invisible */
		gi.positioned_sound( ent->s.origin, ent, chan, ent->noise_index,
		                     ent->volume, ent->attenuation, 0 );
	}
}

void
SP_target_speaker( edict_t *ent ) {
	char buffer[MAX_QPATH];

	if ( !ent ) {
		return;
	}

	if ( !st.noise ) {
		gi.dprintf( "target_speaker with no noise set at %s\n",
		            vtos( ent->s.origin ) );
		return;
	}

	if ( !strstr( st.noise, ".wav" ) ) {
		Com_sprintf( buffer, sizeof( buffer ), "%s.wav", st.noise );
	} else {
		Q_strlcpy( buffer, st.noise, sizeof( buffer ) );
	}

	ent->noise_index = gi.soundindex( buffer );

	if ( !ent->volume ) {
		ent->volume = 1.0;
	}

	if ( !ent->attenuation ) {
		ent->attenuation = 1.0;
	} else if ( ent->attenuation == -1 ) { /* use -1 so 0 defaults to 1 */
		ent->attenuation = 0;
	}

	/* check for prestarted looping sound */
	if ( ent->spawnflags & 1 ) {
		ent->s.sound = ent->noise_index;
	}

	ent->use = Use_Target_Speaker;

	/* must link the entity so we get areas and clusters so
	   the server can determine who to send updates to */
	gi.linkentity( ent );
}

/* ========================================================== */

/*
 * QUAKED target_explosion (1 0 0) (-8 -8 -8) (8 8 8)
 * Spawns an explosion temporary entity when used.
 *
 * "delay"		wait this long before going off
 * "dmg"		how much radius damage should be done, defaults to 0
 */
void
target_explosion_explode( edict_t *self ) {
	float save;

	if ( !self ) {
		return;
	}

	gi.WriteByte( svc_temp_entity );
	gi.WriteByte( TE_EXPLOSION1 );
	gi.WritePosition( self->s.origin );
	gi.multicast( self->s.origin, MULTICAST_PHS );

	save = self->delay;
	self->delay = 0;
	G_UseTargets( self, self->activator );
	self->delay = save;
}

void
use_target_explosion( edict_t *self, edict_t *other /* unused */, edict_t *activator ) {
	self->activator = activator;

	if ( !self || !activator ) {
		return;
	}

	if ( !self->delay ) {
		target_explosion_explode( self );
		return;
	}

	self->think = target_explosion_explode;
	self->nextthink = level.time + self->delay;
}

void
SP_target_explosion( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	ent->use = use_target_explosion;
	ent->svflags = SVF_NOCLIENT;
}

/* ========================================================== */

/*
 * QUAKED target_changelevel (1 0 0) (-8 -8 -8) (8 8 8)
 * Changes level to "map" when fired
 */
void
use_target_changelevel( edict_t *self, edict_t *other, edict_t *activator ) {
	if ( !self || !other  || !activator ) {
		return;
	}

	if ( level.intermissiontime ) {
		return; /* already activated */
	}

	/* if noexit, do a ton of damage to other */
	if ( !( ( int )dmflags->value & DF_ALLOW_EXIT ) &&
	        ( other != world ) ) {
		T_Damage( other, self, self, vec3_origin, other->s.origin,
		          vec3_origin, 10 * other->max_health, 1000,
		          0, MOD_EXIT );
		return;
	}

	/* if multiplayer, let everyone know who hit the exit */
	if ( activator && activator->client ) {
		gi.bprintf( PRINT_HIGH, "%s exited the level.\n",
		            activator->client->pers.netname );
	}

	/* if going to a new unit, clear cross triggers */
	if ( strstr( self->map, "*" ) ) {
		game.serverflags &= ~( SFL_CROSS_TRIGGER_MASK );
	}

	BeginIntermission( self );
}

void
SP_target_changelevel( edict_t *ent ) {
	if ( !ent ) {
		return;
	}

	if ( !ent->map ) {
		gi.dprintf( "target_changelevel with no map at %s\n", vtos( ent->s.origin ) );
		G_FreeEdict( ent );
		return;
	}

	/* Mapquirk for secret exists in fact1 and fact3 */
	if ( ( Q_stricmp( level.mapname, "fact1" ) == 0 ) &&
	        ( Q_stricmp( ent->map, "fact3" ) == 0 ) ) {
		ent->map = "fact3$secret1";
	}

	ent->use = use_target_changelevel;
	ent->svflags = SVF_NOCLIENT;
}

/* ========================================================== */

/*
 * QUAKED target_splash (1 0 0) (-8 -8 -8) (8 8 8)
 * Creates a particle splash effect when used.
 *
 * Set "sounds" to one of the following:
 * 1) sparks
 * 2) blue water
 * 3) brown water
 * 4) slime
 * 5) lava
 * 6) blood
 *
 * "count"	how many pixels in the splash
 * "dmg"	if set, does a radius damage at this location when it splashes
 *          useful for lava/sparks
 */
void
use_target_splash( edict_t *self, edict_t *other /* unused */, edict_t *activator ) {
	if ( !self || !activator ) {
		return;
	}

	gi.WriteByte( svc_temp_entity );
	gi.WriteByte( TE_SPLASH );
	gi.WriteByte( self->count );
	gi.WritePosition( self->s.origin );
	gi.WriteDir( self->movedir );
	gi.WriteByte( self->sounds );
	gi.multicast( self->s.origin, MULTICAST_PVS );
}

void
SP_target_splash( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->use = use_target_splash;
	G_SetMovedir( self->s.angles, self->movedir );

	if ( !self->count ) {
		self->count = 32;
	}

	self->svflags = SVF_NOCLIENT;
}

/* ========================================================== */

/*
 * QUAKED target_crosslevel_trigger (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
 * Once this trigger is touched/used, any trigger_crosslevel_target with
 * the same trigger number is automatically used when a level is started
 * within the same unit.  It is OK to check multiple triggers. Message,
 * delay, target, and killtarget also work.
 */
void
trigger_crosslevel_trigger_use( edict_t *self, edict_t *other /* unused */,
                                edict_t *activator ) {
	if ( !self || !activator ) {
		return;
	}

	game.serverflags |= self->spawnflags;
	G_UseTargets ( self, activator );
	G_FreeEdict( self );
}

void
SP_target_crosslevel_trigger( edict_t *self ) {
	if ( !self ) {
		return;
	}

	self->svflags = SVF_NOCLIENT;
	self->use = trigger_crosslevel_trigger_use;
}

/*
 * QUAKED target_crosslevel_target (.5 .5 .5) (-8 -8 -8) (8 8 8) trigger1 trigger2 trigger3 trigger4 trigger5 trigger6 trigger7 trigger8
 * Triggered by a trigger_crosslevel elsewhere within a unit.
 * If multiple triggers are checked, all must be true. Delay,
 * target and killtarget also work.
 *
 * "delay" delay before using targets if the trigger has been
 *         activated (default 1)
 */
void
target_crosslevel_target_think( edict_t *self ) {
	if ( !self ) {
		return;
	}

	if ( self->spawnflags ==
	        ( game.serverflags & SFL_CROSS_TRIGGER_MASK & self->spawnflags ) ) {
		G_UseTargets( self, self );
		G_FreeEdict( self );
	}
}

void
SP_target_crosslevel_target( edict_t *self ) {
	if ( !self ) {
		return;
	}

	if ( !self->delay ) {
		self->delay = 1;
	}

	self->svflags = SVF_NOCLIENT;

	self->think = target_crosslevel_target_think;
	self->nextthink = level.time + self->delay;
}