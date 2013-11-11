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
/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */
//-----------------------------------------------------------------------------
// GhostCollision is a custom resource that is constructed from a geo at ModelParseClose time.
// It is then used to generate some procedural geometry based on a sub set of the model's faces
//-----------------------------------------------------------------------------

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

    // helper function for looking up verts from CIwModel's vertex block
    CIwFVec3& GetVert(int32 i)
    {
        return ((CIwFVec3*)m_pModel->GetVerts())[m_Points[i]];
    }

	void init(CIwFMat* modelMatrix);

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

	CIwTexture* borderTexture;
	CIwTexture* centerTexture;
	CIwTexture* endTexture;
	CIwTexture* startTexture;
};

#ifdef IW_BUILD_RESOURCES
void BuildCollision(const char* pUserString, CIwModel* pModel);
#endif
