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
 * Player weapons.
 *
 * =======================================================================
 */

#include "../header/local.h"
#include "player.h"

#define FRAME_FIRE_FIRST (FRAME_ACTIVATE_LAST + 1)
#define FRAME_IDLE_FIRST (FRAME_FIRE_LAST + 1)
#define FRAME_DEACTIVATE_FIRST (FRAME_IDLE_LAST + 1)

void P_ProjectSource( gclient_t *client, vec3_t point, vec3_t distance,
                      vec3_t forward, vec3_t right, vec3_t result ) {
	vec3_t _distance;

	if ( !client ) {
		return;
	}

	VectorCopy( distance, _distance );

	if ( client->pers.hand == LEFT_HANDED ) {
		_distance[1] *= -1;
	} else if ( client->pers.hand == CENTER_HANDED ) {
		_distance[1] = 0;
	}

	result[0] = point[0] + forward[0] * _distance[0] + right[0] * _distance[1];
	result[1] = point[1] + forward[1] * _distance[0] + right[1] * _distance[1];
	result[2] = point[2] + forward[2] * _distance[0] + right[2] * _distance[1] + _distance[2];
}

void ChangeWeapon( edict_t *ent ) {
	int i;

	if ( !ent ) {
		return;
	}

	ent->client->pers.weapon = ent->client->newweapon;
	ent->client->newweapon = false;

	/* set visible model */
	if ( ent->s.modelindex == 255 ) {
		if ( ent->client->pers.weapon ) {
			i = ( ( WEAP_SHOTGUN & 0xff ) << 8 );
		} else {
			i = 0;
		}
		ent->s.skinnum = ( ent - g_edicts - 1 ) | i;
	}

	if ( !ent->client->pers.weapon ) { /* dead */
		ent->client->ps.gunindex = 0;
		return;
	}

	ent->client->weaponstate = WEAPON_ACTIVATING;
	ent->client->ps.gunframe = 0;
	ent->client->ps.gunindex = gi.modelindex( "models/weapons/v_shotg/tris.md2" );

	ent->client->anim_priority = ANIM_PAIN;

	if ( ent->client->ps.pmove.pm_flags & PMF_DUCKED ) {
		ent->s.frame = FRAME_crpain1;
		ent->client->anim_end = FRAME_crpain4;
	} else {
		ent->s.frame = FRAME_pain301;
		ent->client->anim_end = FRAME_pain304;
	}
}

/*
 * A generic function to handle
 * the basics of weapon thinking
 */
void Weapon_Generic( edict_t *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST,
                int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int *pause_frames,
                int *fire_frames, void ( *fire )( edict_t *ent ) ) {
	int n;

	if ( !ent || !fire_frames || !fire ) {
		return;
	}

	/* VWep animations screw up corpses */
	if ( ent->deadflag || ( ent->s.modelindex != 255 ) ) {
		return;
	}

	if ( ent->client->weaponstate == WEAPON_DROPPING ) {
		if ( ent->client->ps.gunframe == FRAME_DEACTIVATE_LAST ) {
			ChangeWeapon( ent );
			return;
		} else if ( ( FRAME_DEACTIVATE_LAST - ent->client->ps.gunframe ) == 4 ) {
			ent->client->anim_priority = ANIM_REVERSE;

			if ( ent->client->ps.pmove.pm_flags & PMF_DUCKED ) {
				ent->s.frame = FRAME_crpain4 + 1;
				ent->client->anim_end = FRAME_crpain1;
			} else {
				ent->s.frame = FRAME_pain304 + 1;
				ent->client->anim_end = FRAME_pain301;
			}
		}

		ent->client->ps.gunframe++;
		return;
	}

	if ( ent->client->weaponstate == WEAPON_ACTIVATING ) {
		if ( ent->client->ps.gunframe == FRAME_ACTIVATE_LAST ) {
			ent->client->weaponstate = WEAPON_READY;
			ent->client->ps.gunframe = FRAME_IDLE_FIRST;
			return;
		}

		ent->client->ps.gunframe++;
		return;
	}

	if ( ( ent->client->newweapon ) && ( ent->client->weaponstate != WEAPON_FIRING ) ) {
		ent->client->weaponstate = WEAPON_DROPPING;
		ent->client->ps.gunframe = FRAME_DEACTIVATE_FIRST;

		if ( ( FRAME_DEACTIVATE_LAST - FRAME_DEACTIVATE_FIRST ) < 4 ) {
			ent->client->anim_priority = ANIM_REVERSE;

			if ( ent->client->ps.pmove.pm_flags & PMF_DUCKED ) {
				ent->s.frame = FRAME_crpain4 + 1;
				ent->client->anim_end = FRAME_crpain1;
			} else {
				ent->s.frame = FRAME_pain304 + 1;
				ent->client->anim_end = FRAME_pain301;
			}
		}

		return;
	}

	if ( ent->client->weaponstate == WEAPON_READY ) {
		if ( ( ( ent->client->latched_buttons | ent->client->buttons ) & BUTTON_ATTACK ) ) {
			ent->client->latched_buttons &= ~BUTTON_ATTACK;

			ent->client->ps.gunframe = FRAME_FIRE_FIRST;
			ent->client->weaponstate = WEAPON_FIRING;

			/* start the animation */
			ent->client->anim_priority = ANIM_ATTACK;

			if ( ent->client->ps.pmove.pm_flags & PMF_DUCKED ) {
				ent->s.frame = FRAME_crattak1 - 1;
				ent->client->anim_end = FRAME_crattak9;
			} else {
				ent->s.frame = FRAME_attack1 - 1;
				ent->client->anim_end = FRAME_attack8;
			}
		} else {
			if ( ent->client->ps.gunframe == FRAME_IDLE_LAST ) {
				ent->client->ps.gunframe = FRAME_IDLE_FIRST;
				return;
			}

			if ( pause_frames ) {
				for ( n = 0; pause_frames[n]; n++ ) {
					if ( ent->client->ps.gunframe == pause_frames[n] ) {
						if ( randk() & 15 ) {
							return;
						}
					}
				}
			}

			ent->client->ps.gunframe++;
			return;
		}
	}

	if ( ent->client->weaponstate == WEAPON_FIRING ) {
		for ( n = 0; fire_frames[n]; n++ ) {
			if ( ent->client->ps.gunframe == fire_frames[n] ) {
				fire( ent );
				break;
			}
		}

		if ( !fire_frames[n] ) {
			ent->client->ps.gunframe++;
		}

		if ( ent->client->ps.gunframe == FRAME_IDLE_FIRST + 1 ) {
			ent->client->weaponstate = WEAPON_READY;
		}
	}
}

void weapon_fire( edict_t *ent ) {
	vec3_t start;
	vec3_t forward, right;
	vec3_t offset;
	vec3_t from, end;
	trace_t tr;
	edict_t *ignore;
	int mask;

	if ( !ent ) {
		return;
	}

	if ( ent->client->ps.gunframe == 9 ) {
		ent->client->ps.gunframe++;
		return;
	}

	AngleVectors( ent->client->v_angle, forward, right, NULL );

	VectorScale( forward, -2, ent->client->kick_origin );
	ent->client->kick_angles[0] = -2;

	VectorSet( offset, 0, 8, ent->viewheight - 8 );
	P_ProjectSource( ent->client, ent->s.origin, offset, forward, right, start );

	VectorMA( start, 8192, forward, end );
	VectorCopy( start, from );
	ignore = ent;
	mask = MASK_SHOT | CONTENTS_SLIME | CONTENTS_LAVA;

	while ( ignore ) {
		tr = gi.trace( from, NULL, NULL, end, ignore, mask );

		if ( tr.contents & ( CONTENTS_SLIME | CONTENTS_LAVA ) ) {
			mask &= ~( CONTENTS_SLIME | CONTENTS_LAVA );
		} else {
			if ( ( tr.ent->client ) || ( tr.ent->solid == SOLID_BBOX ) ) {
				ignore = tr.ent;
			} else {
				ignore = NULL;
			}

			if ( ( tr.ent != ent ) && ( tr.ent->takedamage ) ) {
				T_Damage( tr.ent, ent, ent, forward, tr.endpos, tr.plane.normal,
				          500, 50, 0, MOD_RAILGUN );
			} else {
				ignore = NULL;
			}
		}
		VectorCopy( tr.endpos, from );
	}

	/* send gun puff / flash */
	gi.WriteByte( svc_temp_entity );
	gi.WriteByte( TE_BFG_LASER );
	gi.WritePosition( start );
	gi.WritePosition( tr.endpos );
	gi.multicast( ent->s.origin, MULTICAST_PHS );

	gi.WriteByte( svc_temp_entity );
	gi.WriteByte( TE_SCREEN_SPARKS );
	gi.WritePosition( tr.endpos );
	gi.WriteDir( tr.plane.normal );
	gi.multicast( tr.endpos, MULTICAST_PVS );

	gi.WriteByte( svc_muzzleflash );
	gi.WriteShort( ent - g_edicts );
	gi.WriteByte( MZ_RAILGUN );
	gi.multicast( ent->s.origin, MULTICAST_PVS );

	ent->client->ps.gunframe++;
}

/*
 * Called by ClientBeginServerFrame and ClientThink
 */
void Think_Weapon( edict_t *ent ) {
	static int pause_frames[] = {22, 28, 34, 0};
	static int fire_frames[] = {8, 9, 0};

	if ( !ent ) {
		return;
	}

	/* if just died, put the weapon away */
	if ( ent->health < 1 ) {
		ent->client->newweapon = false;
		ChangeWeapon( ent );
	}

	/* call active weapon think routine */
	if ( ent->client->pers.weapon ) {
		Weapon_Generic( ent, 7, 18, 36, 39, pause_frames,
		                fire_frames, weapon_fire );
	}
}