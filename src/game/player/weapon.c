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

void
P_ProjectSource(gclient_t *client, vec3_t point, vec3_t distance,
		vec3_t forward, vec3_t right, vec3_t result)
{
	vec3_t _distance;

	if (!client)
	{
		return;
	}

	VectorCopy(distance, _distance);

	if (client->pers.hand == LEFT_HANDED)
	{
		_distance[1] *= -1;
	}
	else if (client->pers.hand == CENTER_HANDED)
	{
		_distance[1] = 0;
	}

	G_ProjectSource(point, _distance, forward, right, result);
}

/*
 * Each player can have two noise objects associated with it:
 * a personal noise (jumping, pain, weapon firing), and a weapon
 * target noise (bullet wall impacts)
 *
 * Monsters that don't directly see the player can move
 * to a noise in hopes of seeing the player from there.
 */
void
PlayerNoise(edict_t *who, vec3_t where, int type)
{
	edict_t *noise;

	if (!who)
	{
		return;
	}

	if (deathmatch->value)
	{
		return;
	}

	if (who->flags & FL_NOTARGET)
	{
		return;
	}

	if (!who->mynoise)
	{
		noise = G_Spawn();
		noise->classname = "player_noise";
		VectorSet(noise->mins, -8, -8, -8);
		VectorSet(noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svflags = SVF_NOCLIENT;
		who->mynoise = noise;

		noise = G_Spawn();
		noise->classname = "player_noise";
		VectorSet(noise->mins, -8, -8, -8);
		VectorSet(noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svflags = SVF_NOCLIENT;
		who->mynoise2 = noise;
	}

	if ((type == PNOISE_SELF) || (type == PNOISE_WEAPON))
	{
		noise = who->mynoise;
		level.sound_entity = noise;
		level.sound_entity_framenum = level.framenum;
	}
	else
	{
		noise = who->mynoise2;
		level.sound2_entity = noise;
		level.sound2_entity_framenum = level.framenum;
	}

	VectorCopy(where, noise->s.origin);
	VectorSubtract(where, noise->maxs, noise->absmin);
	VectorAdd(where, noise->maxs, noise->absmax);
	noise->last_sound_time = level.time;
	gi.linkentity(noise);
}

void
ChangeWeapon(edict_t *ent)
{
	int i;

	if (!ent)
	{
		return;
	}

	ent->client->pers.lastweapon = ent->client->pers.weapon;
	ent->client->pers.weapon = ent->client->newweapon;
	ent->client->newweapon = NULL;

	/* set visible model */
	if (ent->s.modelindex == 255)
	{
		if (ent->client->pers.weapon)
		{
			i = ((ent->client->pers.weapon->weapmodel & 0xff) << 8);
		}
		else
		{
			i = 0;
		}

		ent->s.skinnum = (ent - g_edicts - 1) | i;
	}

	ent->client->ammo_index = 0;

	if (!ent->client->pers.weapon)
	{
		/* dead */
		ent->client->ps.gunindex = 0;
		return;
	}

	ent->client->weaponstate = WEAPON_ACTIVATING;
	ent->client->ps.gunframe = 0;
	ent->client->ps.gunindex = gi.modelindex(
			ent->client->pers.weapon->view_model);

	ent->client->anim_priority = ANIM_PAIN;

	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
	{
		ent->s.frame = FRAME_crpain1;
		ent->client->anim_end = FRAME_crpain4;
	}
	else
	{
		ent->s.frame = FRAME_pain301;
		ent->client->anim_end = FRAME_pain304;
	}
}

/*
 * Called by ClientBeginServerFrame and ClientThink
 */
void
Think_Weapon(edict_t *ent)
{
	if (!ent)
	{
		return;
	}

	/* if just died, put the weapon away */
	if (ent->health < 1)
	{
		ent->client->newweapon = NULL;
		ChangeWeapon(ent);
	}

	/* call active weapon think routine */
	if (ent->client->pers.weapon && ent->client->pers.weapon->weaponthink)
	{
		ent->client->pers.weapon->weaponthink(ent);
	}
}

/*
 * A generic function to handle
 * the basics of weapon thinking
 */
void
Weapon_Generic(edict_t *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST,
		int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int *pause_frames,
		int *fire_frames, void (*fire)(edict_t *ent))
{
	int n;

	if (!ent || !fire_frames || !fire)
	{
		return;
	}

	if (ent->deadflag || (ent->s.modelindex != 255)) /* VWep animations screw up corpses */
	{
		return;
	}

	if (ent->client->weaponstate == WEAPON_DROPPING)
	{
		if (ent->client->ps.gunframe == FRAME_DEACTIVATE_LAST)
		{
			ChangeWeapon(ent);
			return;
		}
		else if ((FRAME_DEACTIVATE_LAST - ent->client->ps.gunframe) == 4)
		{
			ent->client->anim_priority = ANIM_REVERSE;

			if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crpain4 + 1;
				ent->client->anim_end = FRAME_crpain1;
			}
			else
			{
				ent->s.frame = FRAME_pain304 + 1;
				ent->client->anim_end = FRAME_pain301;
			}
		}

		ent->client->ps.gunframe++;
		return;
	}

	if (ent->client->weaponstate == WEAPON_ACTIVATING)
	{
		if (ent->client->ps.gunframe == FRAME_ACTIVATE_LAST)
		{
			ent->client->weaponstate = WEAPON_READY;
			ent->client->ps.gunframe = FRAME_IDLE_FIRST;
			return;
		}

		ent->client->ps.gunframe++;
		return;
	}

	if ((ent->client->newweapon) && (ent->client->weaponstate != WEAPON_FIRING))
	{
		ent->client->weaponstate = WEAPON_DROPPING;
		ent->client->ps.gunframe = FRAME_DEACTIVATE_FIRST;

		if ((FRAME_DEACTIVATE_LAST - FRAME_DEACTIVATE_FIRST) < 4)
		{
			ent->client->anim_priority = ANIM_REVERSE;

			if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crpain4 + 1;
				ent->client->anim_end = FRAME_crpain1;
			}
			else
			{
				ent->s.frame = FRAME_pain304 + 1;
				ent->client->anim_end = FRAME_pain301;
			}
		}

		return;
	}

	if (ent->client->weaponstate == WEAPON_READY)
	{
		if (((ent->client->latched_buttons |
			  ent->client->buttons) & BUTTON_ATTACK))
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;

			ent->client->ps.gunframe = FRAME_FIRE_FIRST;
			ent->client->weaponstate = WEAPON_FIRING;

			/* start the animation */
			ent->client->anim_priority = ANIM_ATTACK;

			if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crattak1 - 1;
				ent->client->anim_end = FRAME_crattak9;
			}
			else
			{
				ent->s.frame = FRAME_attack1 - 1;
				ent->client->anim_end = FRAME_attack8;
			}
		}
		else
		{
			if (ent->client->ps.gunframe == FRAME_IDLE_LAST)
			{
				ent->client->ps.gunframe = FRAME_IDLE_FIRST;
				return;
			}

			if (pause_frames)
			{
				for (n = 0; pause_frames[n]; n++)
				{
					if (ent->client->ps.gunframe == pause_frames[n])
					{
						if (randk() & 15)
						{
							return;
						}
					}
				}
			}

			ent->client->ps.gunframe++;
			return;
		}
	}

	if (ent->client->weaponstate == WEAPON_FIRING)
	{
		for (n = 0; fire_frames[n]; n++)
		{
			if (ent->client->ps.gunframe == fire_frames[n])
			{
				fire(ent);
				break;
			}
		}

		if (!fire_frames[n])
		{
			ent->client->ps.gunframe++;
		}

		if (ent->client->ps.gunframe == FRAME_IDLE_FIRST + 1)
		{
			ent->client->weaponstate = WEAPON_READY;
		}
	}
}

/* ====================================================================== */

/* SHOTGUN / SUPERSHOTGUN */

void
fire_rail(edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick)
{
	vec3_t from;
	vec3_t end;
	trace_t tr;
	edict_t *ignore;
	int mask;

	if (!self)
	{
		return;
	}

	VectorMA(start, 8192, aimdir, end);
	VectorCopy(start, from);
	ignore = self;
	mask = MASK_SHOT | CONTENTS_SLIME | CONTENTS_LAVA;

	while (ignore)
	{
		tr = gi.trace(from, NULL, NULL, end, ignore, mask);

		if (tr.contents & (CONTENTS_SLIME | CONTENTS_LAVA))
		{
			mask &= ~(CONTENTS_SLIME | CONTENTS_LAVA);
		}
		else
		{
			if ((tr.ent->client) || (tr.ent->solid == SOLID_BBOX))
			{
				ignore = tr.ent;
			}
			else
			{
				ignore = NULL;
			}

			if ((tr.ent != self) && (tr.ent->takedamage))
			{
				T_Damage(tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal,
						damage, kick, 0, MOD_RAILGUN);
			}
			else
			{
				ignore = NULL;
			}
		}

		VectorCopy(tr.endpos, from);
	}

	/* send gun puff / flash */
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BFG_LASER);
	gi.WritePosition(start);
	gi.WritePosition(tr.endpos);
	gi.multicast(self->s.origin, MULTICAST_PHS);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_SCREEN_SPARKS);
	gi.WritePosition(tr.endpos);
	gi.WriteDir(tr.plane.normal);
	gi.multicast(tr.endpos, MULTICAST_PVS);

	if (self->client)
	{
		PlayerNoise(self, tr.endpos, PNOISE_IMPACT);
	}
}

void
weapon_shotgun_fire(edict_t *ent)
{
	vec3_t start;
	vec3_t forward, right;
	vec3_t offset;

	if (!ent)
	{
		return;
	}

	if (ent->client->ps.gunframe == 9)
	{
		ent->client->ps.gunframe++;
		return;
	}

	AngleVectors(ent->client->v_angle, forward, right, NULL);

	VectorScale(forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -2;

	VectorSet(offset, 0, 8, ent->viewheight - 8);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

	fire_rail(ent, start, forward, 200, 50);

	/* send muzzle flash */
	gi.WriteByte(svc_muzzleflash);
	gi.WriteShort(ent - g_edicts);
	gi.WriteByte(MZ_RAILGUN);
	gi.multicast(ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);
}

void
Weapon_Shotgun(edict_t *ent)
{
	static int pause_frames[] = {22, 28, 34, 0};
	static int fire_frames[] = {8, 9, 0};

	if (!ent)
	{
		return;
	}

	Weapon_Generic(ent, 7, 18, 36, 39, pause_frames,
			fire_frames, weapon_shotgun_fire);
}