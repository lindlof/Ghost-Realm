/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "IwGraphics.h"
#include "Iw2D.h"

class ManaBar {
	private:
	CIw2DImage* borderTexture;
	CIw2DImage* centerTexture;
	CIw2DImage* endTexture;
	CIw2DImage* startTexture;

	CIwFVec2 startVerts[4];

	CIwFVec2 borderTopLeft;
	CIwFVec2 borderSize;

	public:
	ManaBar();
	~ManaBar();

	void Render();

};
