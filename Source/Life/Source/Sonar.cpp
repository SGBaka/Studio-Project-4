#include "Sonar.h"

Sonar::Sonar()
: numSides(0)
, radius(1)
, rotationCounter(0)
, maxRad(0)
{

}

Sonar::~Sonar()
{

}

void Sonar::Init(float radius, int numSides)
{
	this->maxRad = radius;
	this->numSides = numSides;
}

void Sonar::GenerateSonar(Vector3 position)
{
	this->position = position;

	vector<Vector3> vertexStorage;
	rotationCounter = 0;	

	for (int i = 0; i < numSides; ++i)
	{
		Vector3 temp;
		temp.x = radius * cos(2 * Math::PI * i / numSides) + position.x;
		temp.y = radius * sin(2 * Math::PI * i / numSides) + position.y;
		vertexStorage.push_back(temp);
	}

	float lengthOfSide = 2 * radius * tan(Math::PI / numSides);
	double interiorAngle = (numSides - 2) * 180 / numSides;

	for (int i = 0; i < vertexStorage.size(); ++i)
	{
		if (i == 0)
			rotationCounter = 90;
		else
			rotationCounter -= interiorAngle;
		
		RingSegments *RS;
		RS = new RingSegments();
		RS->Init(Vector3(vertexStorage[i].x, vertexStorage[i].y, 0));
		RS->scale.Set(lengthOfSide / 2, 1, 1);
		RS->rotation = rotationCounter;
		RS->mesh = MainScene::GetInstance()->P_meshArray[MainScene::E_GEO_LINE];
		segmentList.push_back(RS);
	}

}

void Sonar::Update(double dt)
{
	if (radius >= maxRad)
		segmentList.clear();


	radius += dt * 100;

	for (int i = 0; i < segmentList.size(); ++i)
	{
		segmentList[i]->position.x = radius * cos(2 * Math::PI * i / numSides) + position.x;
		segmentList[i]->position.y = radius * sin(2 * Math::PI * i / numSides) + position.y;
		float lengthOfSide = 2 * radius * tan(Math::PI / numSides);
		segmentList[i]->scale.Set(lengthOfSide / 2, 1, 1);
	}
}