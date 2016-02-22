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

	void GenerateSonar(Vector3 position, bool special);
	void Init(float radius, int numSides, float speed = 2);
	void Update(double dt);
	float GetSonarRadius();
	vector<RingSegments*> segmentList;
	float radius, maxRad;
	int type;
	Color colorStore;

private:
	int numSides;
	double rotationCounter;
	float speed;
	Vector3 position;
};

#endif