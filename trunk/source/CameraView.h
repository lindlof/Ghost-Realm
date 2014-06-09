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

#include "s3e.h"
#include "GhostCollision.h"
#include "CameraDefend.h"
#include "FightTutorialView.h"

#define GHOST_HIT_LENGTH 500
#define GHOST_HIT_LIGHT 0xf0 // 0x00 - 0xff

void CameraViewPreInit();
void CameraViewInit();
void CameraViewTerm();
bool CameraViewUpdate();

void ghostTouch(int32 x, int32 y);

FightTutorialView* getFightTutorialView();
CameraDefend* getCameraDefend();
