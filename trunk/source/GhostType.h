/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _GHOST_TYPE_H
#define _GHOST_TYPE_H

#define GHOST_TYPE_REFERENCE_DISTANCE 700

class GhostType {
	public:
	static const GhostType VIKING;
	static const GhostType SKELMAN;

	GhostType() {}

	private:
	int id;
	int distance;
	int initialDistance;
	float strength;

	GhostType(int id, int distance, int initialDistance, float strength);

	public:
	int getDistance();
	int getInitialDistance();
	float getStrength();
	bool operator==(const GhostType&) const;
};

#endif
