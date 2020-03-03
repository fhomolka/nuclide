/*
 * Copyright (c) 2016-2019 Marco Hladik <marco@icculus.org>
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

enum
{
	BNAIL_IDLE1,
	BNAIL_IDLE2,
	BNAIL_IDLE3,
	BNAIL_SHOOT,
	BNAIL_UNUSED1,
	BNAIL_RELOAD,
	BNAIL_UNUSED2,
	BNAIL_DRAW,
	BNAIL_HOLSTER,
	BNAIL_UNUSED3,
	BNAIL_TILT,
	BNAIL_TILTBACK,
	BNAIL_SHOOT2
};

void
w_bradnailer_precache(void)
{
	precache_sound("weapons/brad_hit1.wav");
	precache_sound("weapons/brad_hit2.wav");
	precache_sound("weapons/bradnailer.wav");


	precache_model("models/nail.mdl");
	precache_model("models/v_bradnailer.mdl");
	precache_model("models/w_bradnailer.mdl");
}

void
w_bradnailer_updateammo(player pl)
{
#ifdef SSQC
	Weapons_UpdateAmmo(pl, __NULL__, __NULL__, __NULL__);
#endif
}

string
w_bradnailer_wmodel(void)
{
	return "models/w_bradnailer.mdl";
}

int
w_bradnailer_pickup(int new)
{
/* TODO */
}

void
w_bradnailer_draw(void)
{
	Weapons_SetModel("models/v_bradnailer.mdl");
	Weapons_ViewAnimation(BNAIL_DRAW);
}

void
w_bradnailer_holster(void)
{
	Weapons_ViewAnimation(BNAIL_HOLSTER);
}

#ifdef SSQC
void
w_bradnailer_shootnail(void)
{
	player pl = (player)self;
	static void Nail_Touch(void) {
		Effect_CreateSpark(self.origin, trace_plane_normal);
		if (other.takedamage == DAMAGE_YES) {
			Damage_Apply(other, self.owner, 15, WEAPON_BRADNAILER, DMG_GENERIC);
			if (random() < 0.5) {
				Weapons_PlaySound(self, CHAN_WEAPON, "weapons/brad_hit1.wav", 1, ATTN_NORM);
			} else {
				Weapons_PlaySound(self, CHAN_WEAPON, "weapons/brad_hit2.wav", 1, ATTN_NORM);
			}
		} else {
			Weapons_PlaySound(self, CHAN_WEAPON, "weapons/xbow_hit1.wav", 1, ATTN_NORM);
		}
		remove(self);
	}

	Weapons_MakeVectors();
	entity nail = spawn();
	setmodel(nail, "models/nail.mdl");
	setorigin(nail, Weapons_GetCameraPos() + (v_forward * 16));
	nail.owner = self;
	nail.velocity = v_forward * 2000;
	nail.movetype = MOVETYPE_FLY;
	nail.solid = SOLID_BBOX;
	nail.angles = vectoangles(nail.velocity);
	nail.avelocity[2] = 10;
	nail.touch = Nail_Touch;
	setsize(nail, [0,0,0], [0,0,0]);

	if (self.flags & FL_CROUCHING)
		Animation_PlayerTopTemp(ANIM_SHOOT1HAND, 0.45f);
	else
		Animation_PlayerTopTemp(ANIM_CR_SHOOT1HAND, 0.45f);

	sound(pl, CHAN_WEAPON, "weapons/bradnailer.wav", 1, ATTN_NORM);
}
#endif

void
w_bradnailer_primary(void)
{
	player pl = (player)self;

	if (pl.w_attack_next > 0.0) {
		return;
	}

#ifdef SSQC
	w_bradnailer_shootnail();
#else
	Weapons_ViewPunchAngle([-2,0,0]);
	Weapons_ViewAnimation(BNAIL_SHOOT);
#endif

	pl.w_attack_next = 0.3f;
	pl.w_idle_next = 5.0f;
}

void
w_bradnailer_secondary(void)
{
	player pl = (player)self;

	if (pl.w_attack_next > 0.0) {
	return;
	}

	/* Hack */
	if (pl.a_ammo3 == 0) {
		pl.a_ammo3 = 1;
		Weapons_ViewAnimation(BNAIL_TILT);
		pl.w_attack_next = 0.4f;
		pl.w_idle_next = pl.w_attack_next;
		return;
	}

#ifdef SSQC
	w_bradnailer_shootnail();
#else
	Weapons_ViewPunchAngle([-2,0,0]);
	Weapons_ViewAnimation(BNAIL_SHOOT2);
#endif

	pl.w_attack_next = 0.2f;
	pl.w_idle_next = pl.w_attack_next;
}


void
w_bradnailer_release(void)
{
	player pl = (player)self;

	if (pl.w_idle_next > 0.0) {
		return;
	}

	if (pl.a_ammo3 == 1) {
		pl.a_ammo3 = 0;
		Weapons_ViewAnimation(BNAIL_TILTBACK);
		pl.w_attack_next = 0.4f;
		pl.w_idle_next = pl.w_attack_next;
		return;
	}

	int r = floor(random(0,3));
	switch (r) {
	case 0:
		Weapons_ViewAnimation(BNAIL_IDLE1);
		break;
	case 1:
		Weapons_ViewAnimation(BNAIL_IDLE2);
		break;
	case 2:
		Weapons_ViewAnimation(BNAIL_IDLE3);
		break;
	}
	pl.w_idle_next = 10.0f;
}

void
w_bradnailer_hudpic(int selected, vector pos)
{
#ifdef CSQC
	if (selected) {
		drawsubpic(
			pos,
			[170,45],
			"sprites/hud640_01.spr_0.tga",
			[0,45/256],
			[170/256,45/256],
			g_hud_color,
			1.0f,
			DRAWFLAG_ADDITIVE
		);
	} else {
		drawsubpic(
			pos,
			[170,45],
			"sprites/hud640_01.spr_0.tga",
			[0,45/256],
			[170/256,45/256],
			g_hud_color,
			1.0f,
			DRAWFLAG_ADDITIVE
		);
	}
#endif
}

weapon_t w_bradnailer =
{
	.id		= ITEM_BRADNAILER,
	.slot		= 1,
	.slot_pos	= 0,
	.ki_spr		= __NULL__,
	.ki_size	= __NULL__,
	.ki_xy		= __NULL__,
	.draw		= w_bradnailer_draw,
	.holster	= w_bradnailer_holster,
	.primary	= w_bradnailer_primary,
	.secondary	= w_bradnailer_secondary,
	.reload		= __NULL__,
	.release	= w_bradnailer_release,
	.crosshair	= __NULL__,
	.precache	= w_bradnailer_precache,
	.pickup		= w_bradnailer_pickup,
	.updateammo	= w_bradnailer_updateammo,
	.wmodel		= w_bradnailer_wmodel,
	.pmodel		= __NULL__,
	.deathmsg	= __NULL__,
	.aimanim	= __NULL__,
	.hudpic		= w_bradnailer_hudpic
};

/* entity definitions for pickups */
#ifdef SSQC
void
weapon_bradnailer(void)
{
	Weapons_InitItem(WEAPON_BRADNAILER);
}
#endif