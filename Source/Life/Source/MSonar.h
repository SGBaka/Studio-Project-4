#ifndef MSONAR_H
#define MSONAR_H

#include "RingSegments.h"
#include "MultScene.h"
#include "MyMath.h"
#include <vector>

using namespace::std;

class MSonar
{

public:
	MSonar();
	~MSonar();

	void GenerateSonar(Vector3 position, int type);
	void Init(float radius, float radius2, int numSides, float speed = 2);
	void Update(double dt);
	float GetSonarRadius();
	vector<RingSegments*> segmentList;
	float radius, radius2, maxRad, rad2Counter;
	int type;
	Color colorStore;
	int alertType;
	Vector3 position;

private:
	int numSides;
	double rotationCounter;
	float speed;

};

#endif