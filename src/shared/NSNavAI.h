/*
 * Copyright (c) 2016-2022 Vera Visions LLC.
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

/* NSNavAI is a moving/pathfinding object. It knows how to deal
   with waypoint based nodes. 
*/
class
NSNavAI:NSSurfacePropEntity
{
#ifdef SERVER
	/* pathfinding */
	int m_iNodes;
	int m_iCurNode;
	nodeslist_t *m_pRoute;
	vector m_vecLastNode;
	vector m_vecTurnAngle;
#endif

	void(void) NSNavAI;

#ifdef SERVER
	/* methods we'd like others to override */
	virtual bool(void) CanCrouch;

	virtual vector(void) GetRouteMovevalues;
	virtual vector(void) GetRouteDirection;

	virtual void(void) RouteEnded;
	virtual void(void) ClearRoute;
	virtual void(void) CheckRoute;
	virtual void(vector) NewRoute;
#endif
};