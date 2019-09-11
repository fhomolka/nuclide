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
	SHOCKRIFLE_IDLE1,
	SHOCKRIFLE_SHOOT,
	SHOCKRIFLE_DRAW,
	SHOCKRIFLE_HOLSTER,
	SHOCKRIFLE_IDLE2
};

void
w_shockrifle_precache(void)
{
	precache_model("models/v_shock.mdl");
	precache_model("models/w_shock.mdl");
	precache_model("models/p_shock.mdl");
	
	precache_sound("weapons/shock_discharge.wav");
	precache_sound("weapons/shock_draw.wav");
	precache_sound("weapons/shock_fire.wav");
	precache_sound("weapons/shock_impact.wav");
	precache_sound("weapons/shock_recharge.wav");
}
void
w_shockrifle_pickup(void)
{
#ifdef SSQC
	player pl = (player)self;
	pl.ammo_shock = 10;
#endif
}

void
w_shockrifle_updateammo(player pl)
{
#ifdef SSQC
	Weapons_UpdateAmmo(pl, -1, pl.ammo_shock, -1);
#endif
}

string
w_shockrifle_wmodel(void)
{
	return "models/w_shock.mdl";
}

string
w_shockrifle_pmodel(void)
{
	return "models/p_shock.mdl";
}

string
w_shockrifle_deathmsg(void)
{
	return "";
}

void
w_shockrifle_draw(void)
{
	Weapons_SetModel("models/v_shock.mdl");
	Weapons_ViewAnimation(SHOCKRIFLE_DRAW);
#ifdef SSQC
	player pl = (player)self;
	Weapons_UpdateAmmo(pl, -1, pl.ammo_shock, -1);
#endif
}

void
w_shockrifle_holster(void)
{
	
}

#ifdef SSQC
void
w_shockrifle_shoothornet(void)
{
	static void Hornet_Touch(void) {
		if (other.takedamage == DAMAGE_YES) {
			Damage_Apply(other, self.owner, 10, trace_endpos, FALSE);
		} else {
		}
		remove(self);
	}
	Weapons_MakeVectors();
	entity bolt = spawn();
	//setmodel(bolt, "models/hornet.mdl");
	setorigin(bolt, Weapons_GetCameraPos() + (v_forward * 16) + (v_up * -8));
	bolt.owner = self;
	bolt.velocity = v_forward * 1000;
	bolt.movetype = MOVETYPE_FLY;
	bolt.solid = SOLID_BBOX;
	//bolt.flags |= FL_LAGGEDMOVE;
	bolt.gravity = 0.5f;
	bolt.angles = vectoangles(bolt.velocity);
	bolt.touch = Hornet_Touch;
	setsize(bolt, [0,0,0], [0,0,0]);
}
#endif

void
w_shockrifle_release(void)
{
	player pl = (player)self;

	if (pl.w_idle_next > 0.0) {
		return;
	}

	#ifdef CSQC
		if (pl.a_ammo2 < 10) {
			pl.a_ammo2 = bound(0, pl.a_ammo2 + 1, 10);
			pl.w_idle_next = 0.5f;
		}
	#else
		if (pl.ammo_shock < 10) {
			pl.ammo_shock = bound(0, pl.ammo_shock + 1, 10);
			Weapons_UpdateAmmo(pl, -1, pl.ammo_shock, -1);
			pl.w_idle_next = 0.5f;
		}
	#endif

	if (pl.w_idle_next > 0.0) {
		return;
	}

	if (random() < 0.5) {
		Weapons_ViewAnimation(SHOCKRIFLE_IDLE1);
	} else {
		Weapons_ViewAnimation(SHOCKRIFLE_IDLE2);
	}
	pl.w_idle_next = 3.333333f;
}

void
w_shockrifle_primary(void)
{
	player pl = (player)self;
	if (pl.w_attack_next > 0.0) {
		return;
	}

	/* Ammo check */
#ifdef CSQC
	if (pl.a_ammo2 <= 0) {
		w_shockrifle_release();
		return;
	}
#else
	if (pl.ammo_shock <= 0) {
		w_shockrifle_release();
		return;
	}
#endif


#ifdef SSQC
	w_shockrifle_shoothornet();
	Weapons_PlaySound(pl, CHAN_WEAPON, "weapons/shock_fire.wav", 1, ATTN_NORM);
	
	pl.ammo_shock--;
	Weapons_UpdateAmmo(pl, -1, pl.ammo_shock, -1);
#else
	pl.a_ammo2--;
#endif

	Weapons_ViewAnimation(SHOCKRIFLE_SHOOT);
	pl.w_attack_next = 0.1f;
	pl.w_idle_next = 0.75f;
}

void
w_shockrifle_crosshair(void)
{
#ifdef CSQC
	vector cross_pos;
	vector aicon_pos;

	/* crosshair */
	cross_pos = (video_res / 2) + [-12,-12];
	drawsubpic(
		cross_pos,
		[24,24],
		"sprites/ofch1.spr_0.tga",
		[0,24/72],
		[24/72, 24/72],
		[1,1,1],
		1.0,
		DRAWFLAG_NORMAL
	);

	/* ammo counter */
	HUD_DrawAmmo2();

	/* ammo icon */
	aicon_pos = video_mins + [video_res[0] - 48, video_res[1] - 42];
	drawsubpic(
		aicon_pos,
		[24,24],
		"sprites/640hud7.spr_0.tga",
		[224/256,48/128],
		[24/256, 24/128],
		g_hud_color,
		pSeat->ammo2_alpha,
		DRAWFLAG_ADDITIVE
	);
#endif
}

float
w_shockrifle_aimanim(void)
{
	return self.flags & FL_CROUCHING ? ANIM_CR_AIMHIVE : ANIM_AIMHIVE;
}

void
w_shockrifle_hudpic(int selected, vector pos)
{
#ifdef CSQC
	if (selected) {
		drawsubpic(
			pos,
			[170,45],
			"sprites/640hudof04.spr_0.tga",
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
			"sprites/640hudof03.spr_0.tga",
			[0,45/256],
			[170/256,45/256],
			g_hud_color,
			1.0f,
			DRAWFLAG_ADDITIVE
		);
	}
#endif
}

weapon_t w_shockrifle =
{
	.id		= ITEM_SHOCKRIFLE,
	.slot		= 6,
	.slot_pos	= 1,
	.ki_spr		= "sprites/320hudof01.spr_0.tga",
	.ki_size	= [48,16],
	.ki_xy		= [48,240],
	.draw		= w_shockrifle_draw,
	.holster	= w_shockrifle_holster,
	.primary	= w_shockrifle_primary,
	.secondary	= __NULL__,
	.reload		= __NULL__,
	.release	= w_shockrifle_release,
	.crosshair	= w_shockrifle_crosshair,
	.precache	= w_shockrifle_precache,
	.pickup		= w_shockrifle_pickup,
	.updateammo	= w_shockrifle_updateammo,
	.wmodel		= w_shockrifle_wmodel,
	.pmodel		= w_shockrifle_pmodel,
	.deathmsg	= w_shockrifle_deathmsg,
	.aimanim	= w_shockrifle_aimanim,
	.hudpic		= w_shockrifle_hudpic
};

#ifdef SSQC
void
weapon_shockrifle(void)
{
	Weapons_InitItem(WEAPON_SHOCKRIFLE);
}
#endif
