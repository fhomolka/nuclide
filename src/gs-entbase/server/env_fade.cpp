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

enumflags
{
	EVF_FADEDROM,
	EVF_MODULATE,
	EVF_ONLYUSER
};

class env_fade : CBaseTrigger
{
	float m_flFadeDuration;
	float m_flFadeHold;

	void() env_fade;
	virtual void() Trigger;
};

void env_fade :: Trigger (void)
{
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_FADE);
	WriteByte(MSG_MULTICAST, m_rendercolor[0]);
	WriteByte(MSG_MULTICAST, m_rendercolor[1]);
	WriteByte(MSG_MULTICAST, m_rendercolor[2]);
	WriteByte(MSG_MULTICAST, m_renderamt);
	WriteFloat(MSG_MULTICAST, m_flFadeDuration);
	WriteFloat(MSG_MULTICAST, m_flFadeHold);
	WriteByte(MSG_MULTICAST, spawnflags);
	msg_entity = eActivator;
	
	if (spawnflags & EVF_ONLYUSER) {
		multicast('0 0 0', MULTICAST_ONE_R);
	} else {
		multicast('0 0 0', MULTICAST_ALL);
	}
}

void env_fade :: env_fade (void)
{
	for (int i = 1; i < (tokenize(__fullspawndata) - 1); i += 2) {
		switch (argv(i)) {
		case "duration":
			m_flFadeDuration = stof(argv(i + 1));
			break;
		case "holdtime":
			m_flFadeHold = stof(argv(i + 1));
			break;
		default:
			break;
		}
	}
	CBaseTrigger::CBaseTrigger();
}