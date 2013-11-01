/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include <math.h>

#include "FightTutorial.h"
#include "Ghost.h"

void CameraModelInit();
void CameraModelTerm();
bool CameraModelUpdate();

void initFight();
void accelometerUpdate(int32 x, int32 y, int32 z);
FightTutorial* getFightTutorial();
bool gameIsHalt();
bool viewFightInitRequired();
void endFight();
