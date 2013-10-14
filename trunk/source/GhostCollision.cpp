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

#include "GhostCollision.h"

IW_MANAGED_IMPLEMENT(GhostCollision);
IW_CLASS_FACTORY(GhostCollision);

#ifdef IW_BUILD_RESOURCES
void BuildCollision(const char* pUserString, CIwModel* pModel)
{
    //check user string to test whether to affect this asset
    //if (strcmp(pUserString, "level") != 0 )
    //    return;

    // Create a new collision resource and name it after its model partner
    GhostCollision* pCollision = new GhostCollision;
    pCollision->SetName(pModel->DebugGetName());

    for (uint32 i = 0; i < IwGetModelBuilder()->GetNumFaces(); i++)
    {
        pCollision->AddFace(IwGetModelBuilder()->GetFace(i), pModel);
    }

    // Add the collision resource to IwResManager - it will be serialised as part of the current group.
    IwGetResManager()->AddRes("GhostCollision", pCollision);
}

#endif



void GhostCollision::setModelMatrix(CIwFMat* modelMatrix) {
	GhostCollision::modelMatrix = modelMatrix;
}

#ifdef IW_BUILD_RESOURCES

void GhostCollision::AddFace(CIwFace* pFace, CIwModel* pModel)
{
    // Remember the model
    IwAssert(GRAPHICS, !m_pModel || m_pModel == pModel);
    m_pModel = pModel;

    // Model build info contains information about how the data in the model was re-arranged.
    // Verts may be reordered for cache performance, etc.
    CIwModelBuildInfoMap& map = m_pModel->GetModelBuildInfo().GetMap(IW_MB_STREAM_VERTS);

    // If this face is a triangle...
    if (pFace->GetNumPoints() == 3 )
    {
        for (uint32 i = 0; i < 3; i++)
        {
            CIwArray<uint16> deps;
            map.GetDependents(pFace->GetVertID(i), deps);
            m_Points.push_back(deps[0]);
        }
    }
    else
    {
        // Only support quads
        IwAssert(GRAPHICS, pFace->GetNumPoints() == 4);

        // Add the quad as two triangles
        uint32 i;
        for (i = 0; i < 3; i++)
        {
            CIwArray<uint16> deps;
            map.GetDependents(pFace->GetVertID(i), deps);
            m_Points.push_back(deps[0]);
        }
        for (i = 0; i < 4; i == 0 ? i += 2 : i++)
        {
            CIwArray<uint16> deps;
            map.GetDependents(pFace->GetVertID(i), deps);
            m_Points.push_back(deps[0]);
        }
    }
}
#endif
//-----------------------------------------------------------------------------
void GhostCollision::Serialise()
{
    CIwManaged::Serialise();

    IwSerialiseManagedHash(&m_pModel);
    m_Points.SerialiseHeader();
    IwSerialiseUInt16(m_Points[0], m_Points.size());
}
//-----------------------------------------------------------------------------
void GhostCollision::Resolve()
{
    CIwManaged::Resolve();

    IwResolveManagedHash(&m_pModel, IW_GRAPHICS_RESTYPE_MODEL);

    //Build face normals (done on resolve to save disk space)
    for (uint32 i = 0; i < m_Points.size(); i += 3)
    {
        CIwVec3 v1 = (CIwVec3)GetVert(i);
        CIwVec3 v2 = (CIwVec3)GetVert(i+1);
        CIwVec3 v3 = (CIwVec3)GetVert(i+2);

        CIwFVec3 cross = (CIwFVec3)(v2 - v1).Cross(v3 - v1);
        if (cross != CIwFVec3::g_Zero)
            cross.Normalise();
        m_Norms.push_back(cross);
    }
}
//-----------------------------------------------------------------------------
int32 GhostCollision::GetFaceUnderCursor(int32 x, int32 y)
{
    //Calculate pos/dir of cursor from camera
    CIwFVec3 pos = IwGxGetViewMatrix().t;
    CIwFVec3 dir(x, y, IwGxGetPerspMul());
    dir.x -= IwGxGetScreenWidth()/2;
    dir.y -= IwGxGetScreenHeight()/2;

    //Extend to the far plane
    dir *= IW_FIXED_DIV(IwGxGetFarZ(), IwGxGetPerspMul());

    //Transform pos/dir into model space
    dir = IwGxGetViewMatrix().RotateVec(dir);
    dir = modelMatrix->TransposeRotateVec(dir);

    //Use more accurate normalise
    dir.Normalise();

    // Scale to touch far plane
    dir *= IwGxGetFarZ();

    pos = modelMatrix->TransposeTransformVec(pos);

    //find first face intersection
    int32 minf = INT32_MAX; //nearest intersection distance
    uint32 nearest = 0; //nearest intersection index

	int lowx, lowy, highx, highy;
	lowx = lowy = INT16_MAX;
	highx = highy = INT16_MIN;

    for (uint32 i = 0; i < m_Points.size(); i += 3)
    {
        CIwFVec3 v1 = (CIwFVec3)GetVert(i);
        CIwFVec3 v2 = (CIwFVec3)GetVert(i+1);
        CIwFVec3 v3 = (CIwFVec3)GetVert(i+2);

		lowx = v1.x < lowx ? v1.x : lowx;
		lowy = v1.y < lowy ? v1.y : lowy;
		highx = v1.x > highx ? v1.x : highx;
		highy = v1.y > highy ? v1.y : highy;

        float f = 0;
        if (IwIntersectLineTriNorm(pos, dir, v1, v2, v3, m_Norms[i/3], f)) {
            if (f < minf)
            {
                minf = f;
                nearest = i;
            }
        }

		{ // Draw the custom resource. Kills fps but helps debugging.
			CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
			pMat->SetColAmbient(0xff0000ff);
			pMat->SetCullMode(CIwMaterial::CULL_NONE);
			IwGxSetMaterial(pMat);

			CIwFVec3* verts = IW_GX_ALLOC(CIwFVec3, 3);
			verts[0] = v1;
			verts[1] = v2;
			verts[2] = v3;
			IwGxSetVertStreamModelSpace(verts, 3);
			IwGxDrawPrims(IW_GX_TRI_LIST, NULL, 3);
		}
    }

    if (minf != INT32_MAX)
    {
        return nearest;
    }

    return -1;
}
