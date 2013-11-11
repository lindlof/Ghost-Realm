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

GhostCollision::GhostCollision() {
	m_pModel = NULL;

	borderTexture = new CIwTexture;
	borderTexture->LoadFromFile ("textures/ectoplasma/ecto_bar_border.png");
	borderTexture->Upload();

	centerTexture = new CIwTexture;
	centerTexture->LoadFromFile ("textures/ectoplasma/ecto_bar_center.png");
	centerTexture->Upload();

	endTexture = new CIwTexture;
	endTexture->LoadFromFile ("textures/ectoplasma/ecto_bar_end.png");
	endTexture->Upload();

	startTexture = new CIwTexture;
	startTexture->LoadFromFile ("textures/ectoplasma/ecto_bar_start.png");
	startTexture->Upload();
}

GhostCollision::~GhostCollision() {
	if (borderTexture)
		delete borderTexture;
	if (centerTexture)
		delete centerTexture;
	if (endTexture)
		delete endTexture;
	if (startTexture)
		delete startTexture;
}

void GhostCollision::init(CIwFMat* modelMatrix) {
	GhostCollision::modelMatrix = modelMatrix;
	ghostY = ghostX = ghostW = -1;
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

// Vertex data
CIwFVec3    border_Verts[8];
CIwFVec3    start_Verts[8];
CIwFVec3    end_Verts[8];
CIwFVec3    center_Verts[8];

// Index stream
uint16      s_TriStrip[22] =
{
    1, 2, 5, 6, 4, 7, 0, 3, 1, 2,
    2, 4, // degenerates
    4, 0, 5, 1,
    1, 3, // degenerates
    3, 7, 2, 6,
};

// Index stream for textured material
uint16      s_QuadStrip[4] =
{
	0, 3, 1, 2,
};

CIwFVec2 s_UVs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(1, 0),
    CIwFVec2(1, 1),
    CIwFVec2(0, 1),
};

CIwColour   s_Cols[8] =
{
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00},
};

void GhostCollision::RenderEctoplasmaBar(float ectoPercent, double ghostRotation)
{
	CIwFMat ectoMatrix = CIwFMat();
	ectoMatrix.CopyRot(*modelMatrix);
	ectoMatrix.CopyTrans(*modelMatrix);
	ectoMatrix.PostRotateY(PI-ghostRotation);

	IwGxSetModelMatrix(&ectoMatrix);
	IwGxSetScreenSpaceSlot(1);

	if (ghostW < 0 || ghostX < 0 || ghostY < 0) ResolveLocation();

	const int16 w = 180;
	const int16 border_x1 = ghostX+ghostW/2 - w;
	const int16 border_x2 = ghostX+ghostW/2 + w;

	const int16 border_y1 = ghostY+0x20;
	const int16 border_y2 = ghostY+0x30;
	const int16 z = 0x6;

	{
		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(borderTexture);

		int16 x1 = border_x1, x2 = border_x2, 
			y1 = border_y1, y2 = border_y2;
		border_Verts[0] = CIwFVec3(x1, y2, -z);
		border_Verts[1] = CIwFVec3(x2, y2, -z);
		border_Verts[2] = CIwFVec3(x2, y1, -z);
		border_Verts[3] = CIwFVec3(x1, y1, -z);
		border_Verts[4] = CIwFVec3(x1, y2,  z);
		border_Verts[5] = CIwFVec3(x2, y2,  z);
		border_Verts[6] = CIwFVec3(x2, y1,  z);
		border_Verts[7] = CIwFVec3(x1, y1,  z);

		IwGxSetMaterial(pMat);
		IwGxSetVertStream(border_Verts, 8);
		//IwGxSetColStream(s_Cols, 8);
		IwGxSetUVStream(s_UVs);
		IwGxDrawPrims(IW_GX_QUAD_STRIP, s_QuadStrip, 4);
	}

	int16 start_y1 = border_y1 + abs((float)(border_y2 - border_y1)*0.235);
	int16 start_y2 = start_y1  + abs((float)(border_y2 - border_y1)*0.625);
	int16 startEndW = (start_y2 - start_y1);
	int16 start_x1 = border_x1 + abs((float)(border_x2 - border_x1)*0.007);
	int16 start_x2 = start_x1 + startEndW;
	
	{
		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(startTexture);

		int16 x1 = start_x1, x2 = start_x2, 
			y1 = start_y1, y2 = start_y2;
		start_Verts[0] = CIwFVec3(x1, y2, -z);
		start_Verts[1] = CIwFVec3(x2, y2, -z);
		start_Verts[2] = CIwFVec3(x2, y1, -z);
		start_Verts[3] = CIwFVec3(x1, y1, -z);
		start_Verts[4] = CIwFVec3(x1, y2,  z);
		start_Verts[5] = CIwFVec3(x2, y2,  z);
		start_Verts[6] = CIwFVec3(x2, y1,  z);
		start_Verts[7] = CIwFVec3(x1, y1,  z);

		IwGxSetMaterial(pMat);
		IwGxSetVertStream(start_Verts, 8);
		//IwGxSetColStream(s_Cols, 8);
		IwGxSetUVStream(s_UVs);
		IwGxDrawPrims(IW_GX_QUAD_STRIP, s_QuadStrip, 4);
	}

	int16 end_y1 = start_y1;
	int16 end_y2 = start_y2;
	
	int endMaxX = border_x2 - abs((float)(border_x2 - border_x1)*0.007) - startEndW;
	int endMinX = start_x2;
	int16 end_x1 = (double)(endMaxX - endMinX)*ectoPercent + endMinX;
	int16 end_x2 = end_x1 + startEndW;
	
	{
		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(endTexture);

		int16 x1 = end_x1, x2 = end_x2, 
			y1 = end_y1, y2 = end_y2;
		end_Verts[0] = CIwFVec3(x1, y2, -z);
		end_Verts[1] = CIwFVec3(x2, y2, -z);
		end_Verts[2] = CIwFVec3(x2, y1, -z);
		end_Verts[3] = CIwFVec3(x1, y1, -z);
		end_Verts[4] = CIwFVec3(x1, y2,  z);
		end_Verts[5] = CIwFVec3(x2, y2,  z);
		end_Verts[6] = CIwFVec3(x2, y1,  z);
		end_Verts[7] = CIwFVec3(x1, y1,  z);

		IwGxSetMaterial(pMat);
		IwGxSetVertStream(end_Verts, 8);
		//IwGxSetColStream(s_Cols, 8);
		IwGxSetUVStream(s_UVs);
		IwGxDrawPrims(IW_GX_QUAD_STRIP, s_QuadStrip, 4);
	}
	
	int16 center_y1 = start_y1;
	int16 center_y2 = start_y2;
	int16 center_x1 = start_x2 - 1;
	int16 center_x2 = end_x1 + 1;
	
	{
		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(centerTexture);

		int16 x1 = center_x1, x2 = center_x2, 
			y1 = center_y1, y2 = center_y2;
		center_Verts[0] = CIwFVec3(x1, y2, -z);
		center_Verts[1] = CIwFVec3(x2, y2, -z);
		center_Verts[2] = CIwFVec3(x2, y1, -z);
		center_Verts[3] = CIwFVec3(x1, y1, -z);
		center_Verts[4] = CIwFVec3(x1, y2,  z);
		center_Verts[5] = CIwFVec3(x2, y2,  z);
		center_Verts[6] = CIwFVec3(x2, y1,  z);
		center_Verts[7] = CIwFVec3(x1, y1,  z);

		IwGxSetMaterial(pMat);
		IwGxSetVertStream(center_Verts, 8);
		//IwGxSetColStream(s_Cols, 8);
		IwGxSetUVStream(s_UVs);
		IwGxDrawPrims(IW_GX_QUAD_STRIP, s_QuadStrip, 4);
	}
	
}

// Resolve ghost modelspace location
void GhostCollision::ResolveLocation() {

	float y1 = INT32_MAX;
	float y2 = INT32_MIN;
	float x1 = INT32_MAX;
    float x2 = INT32_MIN;

    for (uint32 i = 0; i < m_Points.size(); i++)
    {
        CIwFVec3 v = (CIwFVec3)GetVert(i);

		if (v.y < y1) y1 = v.y;
		if (v.y > y2) y2 = v.y;
		if (v.x < x1) x1 = v.x;
		if (v.x > x2) x2 = v.x;
    }

	ghostY = y2;
	ghostX = x1;
	ghostW = x2 - x1;
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

    for (uint32 i = 0; i < m_Points.size(); i += 3)
    {
        CIwFVec3 v1 = (CIwFVec3)GetVert(i);
        CIwFVec3 v2 = (CIwFVec3)GetVert(i+1);
        CIwFVec3 v3 = (CIwFVec3)GetVert(i+2);

        float f = 0;
        if (IwIntersectLineTriNorm(pos, dir, v1, v2, v3, m_Norms[i/3], f)) {
            if (f < minf)
            {
                minf = f;
                nearest = i;
            }
        }

		/*{ // Draw the custom resource. Kills fps but helps debugging.
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
		}*/
    }

    if (minf != INT32_MAX)
    {
        return nearest;
    }

    return -1;
}
