#include "Sonar.h"

Sonar::Sonar()
: numSides(0)
, radius(1)  
, rotationCounter(0)
, maxRad(0)
, speed(1.5)
{

}

Sonar::~Sonar()
{

}

void Sonar::Init(float radius, int numSides, float speed)
{
	this->maxRad = radius;
	this->numSides = numSides;
	this->speed = speed;
}

void Sonar::GenerateSonar(Vector3 position)
{
	this->position = position;

	vector<Vector3> vertexStorage;
	rotationCounter = 0;	

	for (int i = 0; i < numSides; ++i)
	{
		Vector3 temp;
		temp.x = radius * cos(Math::TWO_PI * (i / numSides)) + position.x;
		temp.y = radius * sin(Math::TWO_PI * (i / numSides)) + position.y;
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

		if (rotationCounter <= -360)
			rotationCounter += 360;
		
		RingSegments *RS;
		RS = new RingSegments();
		RS->Init(Vector3(vertexStorage[i].x, vertexStorage[i].y, 0));
		RS->scale.Set(lengthOfSide / 2, 1, 1);
		RS->rotation = rotationCounter;

		RS->posStart = RS->position + (lengthOfSide / 2);
		RS->posEnd = RS->position - (lengthOfSide / 2);
		RS->startTrans = RS->posStart - RS->position;
		RS->endTrans = RS->posEnd - RS->position;

		double angle = Math::DegreeToRadian(RS->rotation);

		double oldStartX = RS->startTrans.x;
		RS->startTrans.x = RS->startTrans.x * cos(angle) - RS->startTrans.y * sin(angle);
		RS->startTrans.y = oldStartX * sin(angle) + RS->startTrans.y * cos(angle);

		double oldEndX = RS->endTrans.x;
		RS->endTrans.x = RS->endTrans.x * cos(angle) - RS->endTrans.y * sin(angle);
		RS->endTrans.y = oldEndX * sin(angle) + RS->endTrans.y * cos(angle);

		RS->posStart = RS->startTrans + RS->position;
		RS->posEnd = RS->endTrans + RS->position;

		RS->mesh = MainScene::GetInstance()->P_meshArray[MainScene::E_GEO_LINE];
		segmentList.push_back(RS);
	}

}

void Sonar::Update(double dt)
{
	if (radius >= maxRad)
		segmentList.clear();

	radius += speed;

	for (int i = 0; i < segmentList.size(); ++i)
	{
		if (segmentList[i]->active)
		{
			segmentList[i]->segmentColor.r = (1 - (radius / maxRad));
			segmentList[i]->segmentColor.g = (1 - (radius / maxRad));
			segmentList[i]->segmentColor.b = (1 - (radius / maxRad));

			segmentList[i]->position.x = radius * cos(2 * Math::PI * i / numSides) + position.x;
			segmentList[i]->position.y = radius * sin(2 * Math::PI * i / numSides) + position.y;
			float lengthOfSide = 2 * radius * tan(Math::PI / numSides);
			segmentList[i]->scale.Set(lengthOfSide / 2, 1, 1);

			segmentList[i]->posStart = segmentList[i]->position + (lengthOfSide / 2);
			segmentList[i]->posEnd = segmentList[i]->position - (lengthOfSide / 2);
			segmentList[i]->startTrans = segmentList[i]->posStart - segmentList[i]->position;
			segmentList[i]->endTrans = segmentList[i]->posEnd - segmentList[i]->position;

			double angle = Math::DegreeToRadian(segmentList[i]->rotation);

			double oldStartX = segmentList[i]->startTrans.x;
			segmentList[i]->startTrans.x = segmentList[i]->startTrans.x * cos(angle) - segmentList[i]->startTrans.y * sin(angle);
			segmentList[i]->startTrans.y = oldStartX * sin(angle) + segmentList[i]->startTrans.y * cos(angle);

			double oldEndX = segmentList[i]->endTrans.x;
			segmentList[i]->endTrans.x = segmentList[i]->endTrans.x * cos(angle) - segmentList[i]->endTrans.y * sin(angle);
			segmentList[i]->endTrans.y = oldEndX * sin(angle) + segmentList[i]->endTrans.y * cos(angle);

			segmentList[i]->posStart = segmentList[i]->startTrans + segmentList[i]->position;
			segmentList[i]->posEnd = segmentList[i]->endTrans + segmentList[i]->position;
		}
	}

}

float Sonar::GetSonarRadius()
{
	return radius;
}