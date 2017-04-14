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
 * Item handling and item definitions.
 *
 * =======================================================================
 */

#include "header/local.h"

void Use_Weapon(edict_t *ent, gitem_t *inv);
void Weapon_Shotgun(edict_t *ent);

/* ====================================================================== */

gitem_t *
GetItemByIndex(int index)
{
	if ((index == 0) || (index >= game.num_items))
	{
		return NULL;
	}

	return &itemlist[index];
}

gitem_t *
FindItem(char *pickup_name)
{
	int i;
	gitem_t *it;

	if (!pickup_name)
	{
		return NULL;
	}

	it = itemlist;

	for (i = 0; i < game.num_items; i++, it++)
	{
		if (!it->pickup_name)
		{
			continue;
		}

		if (!Q_stricmp(it->pickup_name, pickup_name))
		{
			return it;
		}
	}

	return NULL;
}

/*
 * Precaches all data needed for a given item.
 * This will be called for each item spawned in a level,
 * and for each item in each client's inventory.
 */
void
PrecacheItem(gitem_t *it)
{
	char *s, *start;
	char data[MAX_QPATH];
	int len;

	if (!it)
	{
		return;
	}

	if (it->pickup_sound)
	{
		gi.soundindex(it->pickup_sound);
	}

	if (it->world_model)
	{
		gi.modelindex(it->world_model);
	}

	if (it->view_model)
	{
		gi.modelindex(it->view_model);
	}

	if (it->icon)
	{
		gi.imageindex(it->icon);
	}

	/* parse the space seperated precache string for other items */
	s = it->precaches;

	if (!s || !s[0])
	{
		return;
	}

	while (*s)
	{
		start = s;

		while (*s && *s != ' ')
		{
			s++;
		}

		len = s - start;

		if ((len >= MAX_QPATH) || (len < 5))
		{
			gi.error("PrecacheItem: %s has bad precache string", it->classname);
		}

		memcpy(data, start, len);
		data[len] = 0;

		if (*s)
		{
			s++;
		}

		/* determine type based on extension */
		if (!strcmp(data + len - 3, "md2"))
		{
			gi.modelindex(data);
		}
		else if (!strcmp(data + len - 3, "sp2"))
		{
			gi.modelindex(data);
		}
		else if (!strcmp(data + len - 3, "wav"))
		{
			gi.soundindex(data);
		}

		if (!strcmp(data + len - 3, "pcx"))
		{
			gi.imageindex(data);
		}
	}
}

/* ====================================================================== */

gitem_t itemlist[] = {
	{
		NULL
	}, /* leave index 0 alone */

	/* QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) */
	{
		"weapon_shotgun",
		NULL,
		NULL,
		NULL,
		Weapon_Shotgun,
		"misc/w_pkup.wav",
		"models/weapons/g_shotg/tris.md2", EF_ROTATE,
		"models/weapons/v_shotg/tris.md2",
		"w_shotgun",
		"Shotgun",
		0,
		0,
		"Shotgun",
		IT_WEAPON | IT_STAY_COOP,
		WEAP_SHOTGUN,
		NULL,
		0,
		"weapons/shotgf1b.wav weapons/shotgr1b.wav"
	},

	/* end of list marker */
	{NULL}
};

void
InitItems(void)
{
	game.num_items = sizeof(itemlist) / sizeof(itemlist[0]) - 1;
}

/*
 * Called by worldspawn
 */
void
SetItemNames(void)
{
	int i;
	gitem_t *it;

	for (i = 0; i < game.num_items; i++)
	{
		it = &itemlist[i];
		gi.configstring(CS_ITEMS + i, it->pickup_name);
	}
}
