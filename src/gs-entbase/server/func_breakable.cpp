/*
 * Copyright (c) 2016-2020 Marco Hladik <marco@icculus.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*QUAKED func_breakable (0 .5 .8) ? SF_TRIGGER SF_TOUCH SF_PRESSURE
"targetname"        Name
"target"            Target when triggered.
"killtarget"        Target to kill when triggered.
"material"          Material it's made of.
"delay"             Delay in seconds of when it breaks under pressure.
"explodemagnitude"  Strength of the explosion.

Brush volume that can break into lots of little pieces.

Material list:

0 = Glass
1 = Wood
2 = Metal
3 = Flesh
4 = Cinder
5 = Tile
6 = Computer
7 = Glass (Unbreakable)
8 = Rock
9 = None

When SF_TOUCH is set, it'll break when an entity runs into it at high
velocities (damage is speed in units * 0.01).

When SF_PRESSURE is set, it'll collapse once someone is standing on top of it.
At that point the "delay" key will decide after how many seconds the object
breaks.

The strength of the explosion decides the radius (magnitude * 2.5) and the
maximum damage the explosion will do (you have to stand in the center for that).
*/

enumflags
{
	SF_TRIGGER,
	SF_TOUCH,
	SF_PRESSURE,
	SF_ISMODEL
};

enum
{
	BREAKMT_GLASS,
	BREAKMT_WOOD,
	BREAKMT_METAL,
	BREAKMT_FLESH,
	BREAKMT_CINDER,
	BREAKMT_TILE,
	BREAKMT_COMPUTER,
	BREAKMT_GLASS_UNBREAKABLE,
	BREAKMT_ROCK,
	BREAKMT_NONE
};

#ifdef VALVE
const string funcbreakable_objtable[22] = {
	"",
	"spawnfunc_item_battery",
	"spawnfunc_item_healthkit",
	"weapon_9mmhandgun",
	"weapon_9mmclip",
	"weapon_9mmAR",
	"spawnfunc_ammo_9mmAR",
	"spawnfunc_ammo_ARgrenades",
	"weapon_shotgun",
	"spawnfunc_ammo_buckshot",
	"weapon_crossbow",
	"spawnfunc_ammo_crossbow",
	"weapon_357",
	"spawnfunc_ammo_357",
	"weapon_rpg",
	"spawnfunc_ammo_rpgclip",
	"spawnfunc_ammo_gaussclip",
	"weapon_hegrenade",
	"weapon_tripmine",
	"weapon_satchel",
	"weapon_snark",
	"weapon_hornetgun"
};
#else
const string funcbreakable_objtable[] = {
	""
};
#endif

class func_breakable:CBaseTrigger
{
	int m_iMaterial;
	float m_flDelay;
	float m_flExplodeMag;
	string m_strBreakSpawn;

	/*entity m_pressAttacker;
	int m_pressType;
	int m_pressDamage;*/

	void(void) func_breakable;
	virtual void(void) Respawn;
	virtual void(void) Explode;
	virtual void(entity, int) Trigger;
	virtual void(void) PlayerTouch;
	/*virtual void(void) PressureDeath;*/
	virtual void(int) Pain;
	virtual void(int) Death;
};

void
func_breakable::Pain(int body)
{
	if (spawnflags & SF_TRIGGER) {
		return;
	}
	if (serverkeyfloat("*bspversion") != 30) {
		return;
	}

	switch (m_iMaterial) {
		case BREAKMT_GLASS:
		case BREAKMT_COMPUTER:
		case BREAKMT_GLASS_UNBREAKABLE:
			sound(self, CHAN_VOICE, sprintf("debris/glass%d.wav", random(1, 4)), 1.0, ATTN_NORM);
			break;
		case BREAKMT_WOOD:
			sound(self, CHAN_VOICE, sprintf("debris/wood%d.wav", random(1, 4)), 1.0, ATTN_NORM);
			break;
		case BREAKMT_METAL:
			sound(self, CHAN_VOICE, sprintf("debris/metal%d.wav", random(1, 4)), 1.0, ATTN_NORM);
			break;
		case BREAKMT_FLESH:
			float fRand  = floor(random(1, 8));
			/* There never was a flesh4.wav */
			if (fRand == 4) {
				fRand = 5;
			}
			sound(self, CHAN_VOICE, sprintf("debris/flesh%d.wav", fRand), 1.0, ATTN_NORM);
			break;
		case BREAKMT_CINDER:
		case BREAKMT_ROCK:
			sound(self, CHAN_VOICE, sprintf("debris/concrete%d.wav", random(1, 4)), 1.0, ATTN_NORM);
			break;
	}
}


void
func_breakable::Explode(void)
{
	vector rp = absmin + (0.5 * (absmax - absmin));
	FX_BreakModel(vlen(size) / 10, absmin, absmax, [0,0,0], m_iMaterial);
	FX_Explosion(rp);
	Damage_Radius(rp, this, m_flExplodeMag, m_flExplodeMag * 2.5f, TRUE, 0);
	CBaseTrigger::UseTargets(this, TRIG_TOGGLE);
	CBaseEntity::Hide();
}

void
func_breakable::Death(int body)
{
	static void break_spawnobject(void) {
		/* these might get overwritten by the entity spawnfunction */
		vector neworg = self.origin;

		/* become the classname assigned */
		CBaseEntity t = (CBaseEntity)self;
		callfunction(self.classname);

		/* apply the saved values back */
		t.origin = t.m_oldOrigin = neworg;

		/* spawn anew */
		if (t.Respawn)
			t.Respawn();
	}

	if (m_iMaterial == BREAKMT_GLASS_UNBREAKABLE) {
		return;
	}

	health = 0;
	eActivator = g_eAttacker;

	if (m_strBreakSpawn != "" && isfunction(m_strBreakSpawn)) {
		entity unit = spawn();
		unit.classname = m_strBreakSpawn;
		unit.think = break_spawnobject;
		unit.nextthink = time;
		unit.real_owner = this;
		setorigin(unit, absmin + (0.5 * (absmax - absmin)));
		print(sprintf("breakable spawning %s\n", unit.classname));
	}

	/* This may seem totally absurd. That's because it is. It's very
	 * unreliable but exploding breakables in close proximity it WILL cause
	 * an OVERFLOW because we'll be busy running through thousands
	 * of entities in total when one breakable damages another in a frame. 
	 * The only way around this is to set a hard-coded limit of loops per
	 * frame and that would break functionality. */
	if (m_flExplodeMag) {
		think = Explode;
		nextthink = time + random(0.0,0.5);
	} else {
		FX_BreakModel(vlen(size) / 10, absmin, absmax, [0,0,0], m_iMaterial);
		/* TODO: ability to have whoever destroyed the crate be the activator */
		CBaseTrigger::UseTargets(this, TRIG_TOGGLE);
		CBaseEntity::Hide();
	}
}

void
func_breakable::Trigger(entity act, int state)
{
	if (health > 0)
		func_breakable::Death(0);
}

/*void
func_breakable::PressureDeath(void)
{
	func_breakable::Death(m_pressAttacker, m_pressType, m_pressDamage);
}*/

void
func_breakable::PlayerTouch(void)
{
	static void TriggerWrap(void) {
		/* TODO: 'this' should be the person who touched the ent instead */
		Trigger(this, TRIG_TOGGLE);
	}

	if (other.classname == classname) {
		return;
	}

	if (spawnflags & SF_TOUCH) {
		int fDamage = (float)(vlen(other.velocity) * 0.01f);

		if (fDamage >= health) {
			touch = __NULL__;
			Damage_Apply(this, other, fDamage, 0, DMG_CRUSH);

			if ((m_iMaterial == BREAKMT_GLASS) || (m_iMaterial == BREAKMT_COMPUTER)) {
				Damage_Apply(other, this, fDamage / 4, 0, DMG_CRUSH);
			}
		}
	}

	if ((spawnflags & SF_PRESSURE) && (other.absmin[2] >= maxs[2] - 2)) {
		touch = __NULL__;
		think = TriggerWrap;

		if (m_flDelay == 0) {
			m_flDelay = 0.1f;
		}

		nextthink = time + m_flDelay;
	}
}

void
func_breakable::Respawn(void)
{
	CBaseEntity::Respawn();

	SetMovetype(MOVETYPE_NONE);

	if (spawnflags & SF_ISMODEL) {
		SetSolid(SOLID_BBOX);
	} else {
		SetSolid(SOLID_BSP);
	}

	SetModel(m_oldModel);
	SetOrigin(m_oldOrigin);
	touch = PlayerTouch;
	think = __NULL__;

	if (spawnflags & SF_TRIGGER) {
		takedamage = DAMAGE_NO;
	} else {
		takedamage = DAMAGE_YES;
	}

	health = m_oldHealth;

	if (!health) {
		health = 15;
	}
}

void
func_breakable::func_breakable(void)
{
	vector vvm_angles = [0,0,0];
	precache_model(model);
	CBaseEntity::CBaseEntity();

	for (int i = 1; i < (tokenize(__fullspawndata) - 1); i += 2) {
		switch (argv(i)) {
		case "vvm_angles":
			vvm_angles = stov(argv(i+1));
			break;
		case "vvm_model":
			// hack, gotta get the world pos */
			solid = SOLID_BSP;
			SetModel(model);

			/* change the origin */
			origin = absmin + (0.5 * (absmax - absmin));
			m_oldOrigin = origin;

			/* Now we can fake being a point entity. */
			model = argv(i+1);
			m_oldModel = model;
			precache_model(model);
			spawnflags |= SF_ISMODEL;
			break;
		case "material":
			m_iMaterial = stoi(argv(i+1));
			break;
		case "delay":
			m_flDelay = stof(argv(i+1));
			break;
		case "explodemagnitude":
			m_flExplodeMag = stof(argv(i+1));
			break;
		case "spawnobject":
			int oid = stoi(argv(i+1));

			if (oid >= funcbreakable_objtable.length) {
				print(sprintf("^1func_breakable^7: spawnobject %i out of bounds! fix your mod!\n", oid));
				m_strBreakSpawn = "";
			} else {
				m_strBreakSpawn = funcbreakable_objtable[oid];
			}
			break;
		case "spawnonbreak":
			m_strBreakSpawn = argv(i+1);
			break;
		default:
			break;
		}
	}

	SetAngles(vvm_angles);
}
