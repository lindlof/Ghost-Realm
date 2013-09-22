/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "CameraModel.h"
#include "s3e.h"
#include "s3eCamera.h"

#include "IwGx.h"
#include "IwGxPrint.h"
#include "IwMaterial.h"
#include "IwTexture.h"

#include <sys/param.h>

#define GHOST_HIT_LENGTH 500

void RenderCamera(CIwMaterial* pMat);
void RenderGhost(CIwMaterial* pMat);

static CIwTexture* g_CameraTexture = NULL;
static CIwTexture* g_GhostTexture = NULL;

static s3eCameraFrameRotation g_FrameRotation = S3E_CAMERA_FRAME_ROT90;
static CIwFMat viewMatrix;


// Camera callback. Copy the capture frame buffer into a texture ready for rendering.
static int32 frameReceived(void* systemData, void* userData)
{
    s3eCameraFrameData *data = (s3eCameraFrameData*)systemData;

    // If there is no texture, create one.
    // This is a slow operation compared to memcpy so we don't want to do it every frame.
    if (g_CameraTexture == NULL)
    {
        g_CameraTexture = new CIwTexture();
        g_CameraTexture->SetMipMapping(false);
        g_CameraTexture->SetModifiable(true);

        g_CameraTexture->CopyFromBuffer(data->m_Width, data->m_Height, CIwImage::RGB_565, data->m_Pitch, (uint8*)data->m_Data, NULL);
        g_CameraTexture->Upload();
    }

    // Copy the camera image data into the texture. Note that it does not get copied to VRAM at this point.
    memcpy(g_CameraTexture->GetTexels(), data->m_Data, data->m_Height * data->m_Pitch);
    g_FrameRotation = data->m_Rotation;

    return 0;
}

void CameraViewInit()
{
    IwGxInit();
    //IwGxLightingOff();

    // Camera field of view
    IwGxSetPerspMul(0xa0);
	// IwGxSetPerspMul((float) IwGxGetScreenWidth() * 0.5f); 90 deg
    IwGxSetFarZNearZ(0x2000,0x10);

    g_GhostTexture = new CIwTexture;
    g_GhostTexture->LoadFromFile("textures/hammersmith_ghost.png");

    // Upload the textures to VRAM ready for use
    g_GhostTexture->Upload();

    // Set up camera capture
    if (s3eCameraAvailable())
    {
        // Register callback to receive camera frames
        s3eCameraRegister(S3E_CAMERA_UPDATE_STREAMING, frameReceived, NULL);

        // Request medium sized image (for decent frame rate) with a platform independent format
		s3eCameraStart(S3E_CAMERA_STREAMING_SIZE_HINT_MEDIUM, S3E_CAMERA_PIXEL_TYPE_RGB565_CONVERTED, 
			S3E_CAMERA_STREAMING_QUALITY_HINT_MED);
    }
    else
    {
        s3eDebugTraceLine("Camera Extension not available!");
    }
}

//-----------------------------------------------------------------------------
// The following function deletes textures if they exist and terminates
// IwGx and s3e API functionality.
//-----------------------------------------------------------------------------
void CameraViewTerm()
{
    // Release all textures
    if (g_GhostTexture)
        delete g_GhostTexture;

    if (g_CameraTexture)
        delete g_CameraTexture;

    if (s3eCameraAvailable())
    {
        s3eCameraStop();
        s3eCameraUnRegister(S3E_CAMERA_UPDATE_STREAMING, frameReceived);
    }

    IwGxTerminate();
}

//-----------------------------------------------------------------------------

bool CameraViewUpdate()
{
    return true;
}

//-----------------------------------------------------------------------------
// The following function displays the augmented reality scene
//-----------------------------------------------------------------------------
void CameraViewRender()
{
	// Clear the screen
    IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	CIwMaterial* pMat = NULL;

	RenderCamera(pMat);
	RenderGhost(pMat);

    IwGxFlush();
    IwGxSwapBuffers();
}

static CIwFVec2 normalUvs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
    CIwFVec2(1, 1),
    CIwFVec2(1, 0),
};
static CIwFVec2 cameraUvs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
    CIwFVec2(1, 1),
    CIwFVec2(1, 0),
};

void RenderCamera(CIwMaterial* pMat) {
    // Refresh dynamic texture
    if (g_CameraTexture != NULL)
        g_CameraTexture->ChangeTexels(g_CameraTexture->GetTexels(), CIwImage::RGB_565);

    pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    pMat->SetTexture(g_CameraTexture);

    // Set as the active material
    IwGxSetMaterial(pMat);

    // Full screen screenspace vertex coords
    int16 x1 = 0;
    int16 x2 = (int16)IwGxGetScreenWidth();
    int16 y1 = 0;
    int16 y2 = (int16)IwGxGetScreenHeight();

    static CIwSVec2 xy3[4];
    xy3[0].x = x1, xy3[0].y = y1;
    xy3[1].x = x1, xy3[1].y = y2;
    xy3[2].x = x2, xy3[2].y = y2;
    xy3[3].x = x2, xy3[3].y = y1;

    // rotate UV coordinates based on rotation
    // (image must rotate against rotation of surface,
    // assuming that the camera image is NOT auto rotated)
    for (int j=0; j<4; j++)
    {
        cameraUvs[j] = normalUvs[(4 - (int)g_FrameRotation + j ) % 4];
    }

    IwGxSetUVStream(cameraUvs);
    IwGxSetVertStreamScreenSpace(xy3, 4);

    // Draw a single quad
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}

void RenderGhost(CIwMaterial* pMat) {

	Ghost ghost = getGhost();
	int sinceHit = clock() - ghost.getHitTime();

    pMat = IW_GX_ALLOC_MATERIAL();
	pMat->SetModulateMode(CIwMaterial::MODULATE_RGB);
	pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

	IwGxLightingEmissive(true);

	if (sinceHit < GHOST_HIT_LENGTH) {
		int halfAnimation = GHOST_HIT_LENGTH/2;

		// Hit starts from 0, maxes out at 0xff and
		// then decreases back to 0. This all happens
		// in the animation length creating fade 
		// in fade out effect.
		int hit;

		// Divide the sinceHit time so that it maxes
		// out in 0xff
		hit = sinceHit / ((float)halfAnimation/0xff);

		if (sinceHit > halfAnimation) {
			// If we are over the half way let's start
			// decreasing the number from 0xff
			hit = 0xff + (0xff - hit);
		}

		pMat->SetColEmissive(0xff, 0xff-hit, 0xff-hit, 0xff);
		//pMat->SetColEmissive(0xff0000ff);
	} else {
		// The default state that displays the image as it is
		pMat->SetColEmissive(0xffffffff);
	}

	pMat->SetTexture(g_GhostTexture);

    IwGxSetMaterial(pMat);

	int imgW = 124;
	int imgH = 273;
	int ghostWidth = (int16)IwGxGetScreenWidth()/3;
	int ghostHeight = (int16)IwGxGetScreenHeight()/4;

	// Preserve the image scale by scaling the relatively
	// larger value down
	if (imgW/imgH < ghostWidth/ghostHeight) {
		// Scale width down
		ghostWidth = ghostHeight * imgW/imgH;
	} else {
		// Scale height down
		ghostHeight = ghostWidth * imgH/imgW;
	}

	// Middle screen screenspace vertex coords
    int16 x1 = (int16)IwGxGetScreenWidth()/2 - ghostWidth/2;
    int16 x2 = (int16)IwGxGetScreenWidth()/2 + ghostWidth/2;
    int16 y1 = (int16)IwGxGetScreenHeight()/2 - ghostHeight/2;
    int16 y2 = (int16)IwGxGetScreenHeight()/2 + ghostHeight/2;

    static CIwSVec2 xy3[4];
    xy3[0].x = x1, xy3[0].y = y1;
    xy3[1].x = x1, xy3[1].y = y2;
    xy3[2].x = x2, xy3[2].y = y2;
    xy3[3].x = x2, xy3[3].y = y1;
    IwGxSetVertStreamScreenSpace(xy3, 4);

    IwGxSetUVStream(normalUvs);
	
    // Draw a single quad
    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);
}