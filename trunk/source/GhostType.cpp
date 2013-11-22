/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "GhostType.h"

const GhostType GhostType::VIKING  = GhostType(0, 700, 1200, 1);
const GhostType GhostType::SKELMAN = GhostType(1, 320, 600, 1);

GhostType::GhostType(int id, int distance, int initialDistance, float strength) {
	this->id = id;
	this->distance = distance;  
    this->initialDistance = initialDistance;  
	this->strength = strength;
}

int GhostType::getDistance() {
	return distance;
}

int GhostType::getInitialDistance() {
	return initialDistance;
}

float GhostType::getStrength() {
	return strength;
}

bool GhostType::operator==(const GhostType& type) const {
	return this->id == type.id;
}
