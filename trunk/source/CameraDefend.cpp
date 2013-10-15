/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraDefend.h"

#include "IwRandom.h"

static CIwFVec2 defend_uvs[4] =
{
    CIwFVec2(1, 1),
    CIwFVec2(1, 0),
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
};

CameraDefend::CameraDefend() {
	defendingDotTexture = new CIwTexture;
	defendingDotTexture->LoadFromFile ("textures/defending_dot.png");
	defendingDotTexture->Upload();

	IwRandSeed((int32)s3eTimerGetMs());
	float width = 82;
	float height = 82;
	float padding = (float)IwGxGetScreenWidth()*0.05f;

	{
		float leftSpotY = IwRandMinMax(0, IwGxGetScreenHeight()-height);
		defendVertsLeft[0] = CIwFVec2(padding, leftSpotY);
		defendVertsLeft[1] = CIwFVec2(padding, leftSpotY+height);
		defendVertsLeft[2] = CIwFVec2(padding+width, leftSpotY+height);
		defendVertsLeft[3] = CIwFVec2(padding+width, leftSpotY);
	}

	{
		float rightSpotY = IwRandMinMax(0, IwGxGetScreenHeight()-height);
		int screenW = IwGxGetScreenWidth();
		defendVertsRight[0] = CIwFVec2(screenW-padding-width, rightSpotY);
		defendVertsRight[1] = CIwFVec2(screenW-padding-width, rightSpotY+height);
		defendVertsRight[2] = CIwFVec2(screenW-padding, rightSpotY+height);
		defendVertsRight[3] = CIwFVec2(screenW-padding, rightSpotY);
	}
}

CameraDefend::~CameraDefend() {
	if (defendingDotTexture)
		delete defendingDotTexture;
}

void CameraDefend::Render() {
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();

    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
    pMat->SetTexture(defendingDotTexture);
    IwGxSetMaterial(pMat);
	IwGxSetUVStream(defend_uvs);

	IwGxSetVertStreamScreenSpace(defendVertsLeft, 4);
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

	IwGxSetVertStreamScreenSpace(defendVertsRight, 4);
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}
