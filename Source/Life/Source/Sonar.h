#ifndef SONAR_H
#define SONAR_H

#include "RingSegments.h"
#include "MainScene.h"
#include "MyMath.h"
#include <vector>

using namespace::std;

class Sonar
{

public:
	Sonar();
	~Sonar();

	void GenerateSonar(Vector3 position);
	void Init(float radius, int numSides);
	void Update(double dt);
	float GetSonarRadius();
	vector<RingSegments*> segmentList;

private:
	int numSides;
	float radius, maxRad;
	float rotationCounter;
	Vector3 position;
};

#endif