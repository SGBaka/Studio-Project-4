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
	void Init(float radius, int numSides, float speed = 2);
	void Update(double dt);
	float GetSonarRadius();
	vector<RingSegments*> segmentList;

private:
	int numSides;
	float radius, maxRad;
	double rotationCounter;
	float speed;
	Vector3 position;
};

#endif