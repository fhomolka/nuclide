/***
*
*	Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
*
*	See the file LICENSE attached with the sources for usage details.
*
****/

#ifdef MENU
enumflags
{
	MBUTTON_VISIBLE,
	MBUTTON_HOVER,
	MBUTTON_DOWN,
	MBUTTON_SHOWOFFLINE,
	MBUTTON_SHOWSP,
	MBUTTON_SHOWMP
};

class CUIMenuButton : CUIWidget
{
	vector m_vecColor;
	float m_flAlpha;
	vector m_vecSize;
	string m_strTitle;
	string m_strIcon;
	
	void() CUIMenuButton;
	virtual void() m_vFunc = 0;
	virtual void() Draw;
	virtual void( vector ) SetSize;
	virtual void( string ) SetTitle;
	virtual void( string ) SetIcon;
	virtual void( void() ) SetFunc;
	virtual void( float, float, float, float ) Input;
};

void CUIMenuButton :: CUIMenuButton ( void )
{
	m_vecColor = UI_MAINCOLOR;
	m_flAlpha = 1.0f;
	m_vecSize = '96 24';
	m_iFlags = MBUTTON_VISIBLE | MBUTTON_SHOWOFFLINE | MBUTTON_SHOWSP | MBUTTON_SHOWMP;
}

void CUIMenuButton :: Draw ( void )
{
	// If we're not ingame
	if ( clientstate() != 2 ) {
		if ( !( m_iFlags & MBUTTON_SHOWOFFLINE ) ) {
			return;
		}
	} else {
		if ( !( m_iFlags & MBUTTON_SHOWSP ) ) {
			return;
		}
	}

	drawfill( m_parent.m_vecOrigin + m_vecOrigin, m_vecSize, m_vecColor, m_flAlpha );
	if ( m_iFlags & MBUTTON_DOWN ) {
		drawfill( m_parent.m_vecOrigin + m_vecOrigin, [m_vecSize[0], 1], '0 0 0', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ 0, m_vecSize[1] - 1], [m_vecSize[0], 1], '1 1 1', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ 0, 1], [1, m_vecSize[1] - 2], '0 0 0', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ m_vecSize[0] - 1, 1], [1, m_vecSize[1] - 2], '1 1 1', 0.5f );
	} else {
		drawfill( m_parent.m_vecOrigin + m_vecOrigin, [m_vecSize[0], 1], '1 1 1', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ 0, m_vecSize[1] - 1], [m_vecSize[0], 1], '0 0 0', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ 0, 1], [1, m_vecSize[1] - 2], '1 1 1', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ m_vecSize[0] - 1, 1], [1, m_vecSize[1] - 2], '0 0 0', 0.5f );
	}

	
	if ( m_strIcon ) {
		drawpic( m_parent.m_vecOrigin + m_vecOrigin + [ 4, 4 ], m_strIcon, '16 16', '1 1 1', 1.0f );
		if ( m_strTitle ) {
			Font_DrawText( m_parent.m_vecOrigin + m_vecOrigin + [ 26, 8 ], m_strTitle, g_fntDefault );
		}
	} else {
		if ( m_strTitle ) {
			Font_DrawText( m_parent.m_vecOrigin + m_vecOrigin + [ 8, 8 ], m_strTitle, g_fntDefault );
		}
	}
}

void CUIMenuButton :: Input ( float flEVType, float flKey, float flChar, float flDevID )
{
	// If we're not ingame
	if ( clientstate() != 2 ) {
		if ( !( m_iFlags & MBUTTON_SHOWOFFLINE ) ) {
			return;
		}
	} else {
		if ( !( m_iFlags & MBUTTON_SHOWSP ) ) {
			return;
		}
	}

	if ( flEVType == IE_KEYDOWN ) {
		if ( flKey == K_MOUSE1 ) {
			if ( Util_MouseAbove( getmousepos(), m_parent.m_vecOrigin + m_vecOrigin, m_vecSize ) ) {
				m_iFlags |= MBUTTON_DOWN;
			}
		}
	} else if ( flEVType == IE_KEYUP ) {
		if ( flKey == K_MOUSE1 ) {
			if ( m_iFlags & MBUTTON_DOWN && Util_MouseAbove( getmousepos(), m_parent.m_vecOrigin + m_vecOrigin, m_vecSize ) ) {
				if ( m_vFunc ) {
					m_vFunc();
				}
			}
			m_iFlags -= ( m_iFlags & MBUTTON_DOWN );
		}
	}
}

void CUIMenuButton :: SetSize ( vector vecSize )
{
	m_vecSize = vecSize;
}
void CUIMenuButton :: SetTitle ( string strName )
{
	m_strTitle = strName;
}
void CUIMenuButton :: SetIcon ( string strName )
{
	m_strIcon = strName;
}
void CUIMenuButton :: SetFunc ( void() vFunc )
{
	m_vFunc = vFunc;
}
#endif
