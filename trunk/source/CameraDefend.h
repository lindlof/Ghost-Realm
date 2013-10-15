/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "IwGraphics.h"

class CameraDefend {
	private:
	CIwTexture* defendingDotTexture;
	CIwFVec2 defendVertsLeft[4];
	CIwFVec2 defendVertsRight[4];

	public:
	CameraDefend();
	~CameraDefend();
	void Render();
};
