/*
Copyright (C) 2015, 2016, 2017 Marco "eukara" Hladik

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef SSQC
void BaseMelee_Draw( void ) {
	self.iCurrentMag = 0;
	self.iCurrentCaliber = 0;
	
	Client_SendEvent( self, EV_WEAPON_DRAW );
}

void BaseMelee_Attack( void ) {
	vector vSource;
	vector vOrigin;

	makevectors( self.v_angle );
	vSource = ( self.origin + self.view_ofs );
	traceline( vSource, vSource + ( v_forward * 64 ), FALSE, self );

	if ( trace_fraction == 1.0 )
		return;

	vOrigin = trace_endpos - v_forward * 2;

	if ( trace_ent.takedamage ) {
		Damage_Apply( trace_ent, self, wptTable[ self.weapon ].iDamage, trace_endpos );
		return;
	}
}

void BaseMelee_Delayed( float fDelay ) {
	static void BaseMelee_Delayed_Trigger( void ) {
		BaseMelee_Attack();
	}
	
	self.think = BaseMelee_Delayed_Trigger;
	self.nextthink = time + fDelay;
}
#endif
