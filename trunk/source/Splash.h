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

class Splash {
private:
	CIwTexture* texture;
	CIwFVec2 vert[4];
	CIwFVec2 uvs[4];

public:
	Splash();
	~Splash();

	void Render();
};
