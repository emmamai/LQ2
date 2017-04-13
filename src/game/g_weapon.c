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
 * Weapon support functions.
 *
 * =======================================================================
 */

#include "header/local.h"

void
fire_rail(edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick)
{
	vec3_t from;
	vec3_t end;
	trace_t tr;
	edict_t *ignore;
	int mask;
	qboolean water;

	if (!self)
	{
		return;
	}

	VectorMA(start, 8192, aimdir, end);
	VectorCopy(start, from);
	ignore = self;
	water = false;
	mask = MASK_SHOT | CONTENTS_SLIME | CONTENTS_LAVA;

	while (ignore)
	{
		tr = gi.trace(from, NULL, NULL, end, ignore, mask);

		if (tr.contents & (CONTENTS_SLIME | CONTENTS_LAVA))
		{
			mask &= ~(CONTENTS_SLIME | CONTENTS_LAVA);
			water = true;
		}
		else
		{
			if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client) ||
				(tr.ent->solid == SOLID_BBOX))
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