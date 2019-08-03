/***
*
*	Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
*
*	See the file LICENSE attached with the sources for usage details.
*
****/

enumflags
{
	RADIO_VISIBLE,
	RADIO_HOVER,
	RADIO_DOWN,
	RADIO_ACTIVE
};

class CUIRadio : CUIWidget
{
	vector m_vecColor;
	float m_flAlpha;
	vector m_vecSize;
	string m_strTitle;
	virtual void() m_vFunc = 0;
	
	void() CUIRadio;
	virtual void() Draw;
	virtual void( vector ) SetSize;
	virtual void( string ) SetTitle;
	virtual int() GetValue;
	virtual void( int ) SetValue;
	virtual void( void() ) SetFunc;
	virtual void( float, float, float, float ) Input;
};

void CUIRadio :: CUIRadio ( void )
{
	m_vecColor = UI_MAINCOLOR;
	m_flAlpha = 1.0f;
	m_vecSize = '96 24';
	m_iFlags = RADIO_VISIBLE;
}

void CUIRadio :: SetSize ( vector vecSize )
{
	m_vecSize = vecSize;
}
void CUIRadio :: SetTitle ( string strName )
{
	m_strTitle = strName;
}
int CUIRadio :: GetValue ( void )
{
	if ( m_iFlags & RADIO_ACTIVE ) {
		return TRUE;
	} else {
		return FALSE;
	}
}
void CUIRadio :: SetValue ( int iValue )
{
	if ( iValue == TRUE ) {
		m_iFlags |= RADIO_ACTIVE;
	} else {
		m_iFlags -= ( m_iFlags & RADIO_ACTIVE );
	}
}
void CUIRadio :: SetFunc ( void() vFunc )
{
	m_vFunc = vFunc;
}

void CUIRadio :: Draw ( void )
{
	drawfill( m_parent.m_vecOrigin + m_vecOrigin, m_vecSize, m_vecColor, m_flAlpha );
	
	if ( m_iFlags & RADIO_DOWN ) {
		drawfill( m_parent.m_vecOrigin + m_vecOrigin, [m_vecSize[0], 1], '0 0 0', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ 0, m_vecSize[1] - 1], [m_vecSize[0], 1], '1 1 1', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ 0, 1], [1, m_vecSize[1] - 2], '0 0 0', 0.5f );
		drawfill( m_parent.m_vecOrigin + m_vecOrigin + [ m_vecSize[0] - 1, 1], [1, m_vecSize[1] - 2], '1 1 1', 0.5f );
	}
	
	if ( m_iFlags & RADIO_ACTIVE ) {
		drawpic( m_parent.m_vecOrigin + m_vecOrigin, "textures/ui/steam/icon_radiosel", '16 16', '1 1 1', 1.0f );
	} else {
		drawpic( m_parent.m_vecOrigin + m_vecOrigin, "textures/ui/steam/icon_radiounsel", '16 16', '1 1 1', 1.0f );
	}
	
	if ( m_strTitle ) {
		Font_DrawText( m_parent.m_vecOrigin + m_vecOrigin + [ 24, 3 ], m_strTitle, g_fntDefault );
	}
}

void CUIRadio :: Input ( float flEVType, float flKey, float flChar, float flDevID )
{
	if ( flEVType == IE_KEYDOWN ) {
		if ( flKey == K_MOUSE1 ) {
			if ( Util_MouseAbove( getmousepos(), m_parent.m_vecOrigin + m_vecOrigin, m_vecSize ) ) {
				m_iFlags |= RADIO_DOWN;
			}
		}
	} else if ( flEVType == IE_KEYUP ) {
		if ( flKey == K_MOUSE1 ) {
			if ( m_iFlags & RADIO_DOWN && Util_MouseAbove( getmousepos(), m_parent.m_vecOrigin + m_vecOrigin, m_vecSize ) ) {
				if ( m_vFunc ) {
					m_vFunc();
				}
				SetValue( TRUE );
			}
			m_iFlags -= ( m_iFlags & RADIO_DOWN );
		}
	}
}
