/*
 * This file is part of the Marmalade SDK Code Samples.
 *
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This source code is intended only as a supplement to the Marmalade SDK.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */
/* ========================================================================= *
 * Ghost Hunter Project                                                      *
 * https://ghosthuntermobile.wordpress.com/                                  *
 * ========================================================================= *
 * Copyright (C) 2013 Ghost Hunter Project                                   *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * ========================================================================= */
//-----------------------------------------------------------------------------
// GhostCollision is a custom resource that is constructed from a geo at ModelParseClose time.
// It is then used to generate some procedural geometry based on a sub set of the model's faces
//-----------------------------------------------------------------------------

#include "GhostType.h"

#include "IwMaterial.h"
#include "IwGraphics.h"

class GhostCollision : public CIwResource {
public:
    IW_MANAGED_DECLARE(GhostCollision);

    GhostCollision();
	~GhostCollision();

#ifdef IW_BUILD_RESOURCES
    void AddFace(CIwFace* pFace, CIwModel* pModel);
#endif

    // Standard CIwResource interface
    void Serialise();
    void Resolve();
	void Render();

	void init(CIwFMat* modelMatrix, GhostType ghostType);

    // helper function for looking up verts from CIwModel's vertex block
    CIwFVec3& GetVert(int32 i)
    {
        return ((CIwFVec3*)m_pModel->GetVerts())[m_Points[i]];
    }

	// Functions that provide the profit
	void RenderEctoplasmaBar(float healthPercent, double ghostRotation);
	int32 GetFaceUnderCursor(int32 x, int32 y);

private:
    // The model this collision is based on
    CIwModel* m_pModel;

    // A list of points. Each triplet is one triangle.
    CIwArray<uint16> m_Points;
    CIwArray<CIwFVec3> m_Norms;

	// Model matrix used in the view
	CIwFMat* modelMatrix;

	float ghostY;
	float ghostX;
	float ghostW;
	void ResolveLocation();

	float ectobarScale;
	CIwTexture* borderTexture;
	CIwTexture* centerTexture;
	CIwTexture* endTexture;
	CIwTexture* startTexture;
};

#ifdef IW_BUILD_RESOURCES
void BuildCollision(const char* pUserString, CIwModel* pModel);
#endif
