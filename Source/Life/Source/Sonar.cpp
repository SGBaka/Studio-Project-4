#include "Sonar.h"

Sonar::Sonar()
: numSides(0)
, radius(1)  
, rotationCounter(0)
, maxRad(0)
, speed(1.5)
, type(1)
, alertType(1)
, rad2Counter(0)
{

}

Sonar::~Sonar()
{
}

void Sonar::Init(float radius, float radius2, int numSides, float speed)
{
	this->maxRad = radius;
	this->numSides = numSides;
	this->speed = speed;
	this->radius2 = radius2;

	LuaScript playerScript("character");
	type = playerScript.get<int>("player.sonar_type");
}

void Sonar::GenerateSonar(Vector3 position, int type)
{
	this->position = position;

	vector<Vector3> vertexStorage;		// Vector list to store vertexes
	rotationCounter = 0;				// Variable to keep track of rotation value

	// Plot regular n-sided polygon vertexes
	for (int i = 0; i < numSides; ++i)
	{
		Vector3 temp;
		temp.x = radius * cos(Math::TWO_PI * (i / numSides)) + position.x;
		temp.y = radius * sin(Math::TWO_PI * (i / numSides)) + position.y;
		vertexStorage.push_back(temp);
	}

	// Get length of each side for scaling
	float lengthOfSide = 2 * radius * tan(Math::PI / numSides);

	// Get interior angle of polygon
	double interiorAngle = (double)((numSides - 2) * 180) / numSides;

	// Draw polygon using lines based on vertexes
	for (int i = 0; i < vertexStorage.size(); ++i)
	{
		if (i == 0)
			rotationCounter = 90;					// Set default rotation to 90 degrees
		else
			rotationCounter -= interiorAngle;		// Increment rotation amount by interior angle for each line

		if (rotationCounter <= -360)				// Make sure rotation value doesn't fall below -360 degrees
			rotationCounter += 360;
		
		// Create the line object
		RingSegments *RS;
		RS = new RingSegments();
		RS->Init(Vector3(vertexStorage[i].x, vertexStorage[i].y, 0));		// Set line position to vertex position
									
		if (RS->type != 2)													// Set the length of the line by scaling X with lengthOfSide
			RS->scale.Set(lengthOfSide / 2, 1, 1);
		else
			RS->scale.Set(lengthOfSide / 2, 6, 1);

		RS->rotation = rotationCounter;										// Set rotation value for the line

		// Find the 2 points that makes up the line (start and end point)
		// Needed for Line-BoundingBox collision detection
		RS->posStart = RS->position + (lengthOfSide / 2);					
		RS->posEnd = RS->position - (lengthOfSide / 2);

		// Determine translation value from both points to
		// the center (used for rotating the points around
		// the center when line rotates)
		RS->startTrans = RS->posStart - RS->position;
		RS->endTrans = RS->posEnd - RS->position;

		// Set rotation value for points to line rotation value
		double angle = Math::DegreeToRadian(RS->rotation);

		// Rotate points around the center 
		double oldStartX = RS->startTrans.x;
		RS->startTrans.x = RS->startTrans.x * cos(angle) - RS->startTrans.y * sin(angle);
		RS->startTrans.y = oldStartX * sin(angle) + RS->startTrans.y * cos(angle);

		double oldEndX = RS->endTrans.x;
		RS->endTrans.x = RS->endTrans.x * cos(angle) - RS->endTrans.y * sin(angle);
		RS->endTrans.y = oldEndX * sin(angle) + RS->endTrans.y * cos(angle);

		// Translate the points back
		RS->posStart = RS->startTrans + RS->position;
		RS->posEnd = RS->endTrans + RS->position;

		// Set mesh to render the line
		RS->mesh = MainScene::GetInstance()->P_meshArray[MainScene::E_GEO_LINE];

		RS->type = type;
			
		segmentList.push_back(RS);
	}

}

void Sonar::Update(double dt)
{

	radius += speed * 60 * dt;
	rad2Counter = (radius2 - maxRad) + radius;

	for (int i = 0; i < segmentList.size(); ++i)
	{
		if (segmentList[i]->active)
		{
			if (type == 1)
			{
				if (segmentList[i]->type == 1)
				{
					segmentList[i]->segmentColor.r = (1 - (radius / maxRad));
					segmentList[i]->segmentColor.g = (1 - (radius / maxRad));
					segmentList[i]->segmentColor.b = (1 - (radius / maxRad));
				}

				else if (segmentList[i]->type == 2)
				{
					segmentList[i]->segmentColor.r = (1 - (radius / maxRad));
					segmentList[i]->segmentColor.g = (1 - (radius / maxRad)) / 2;
					segmentList[i]->segmentColor.b = 0;
				}
			}
			else
			{
				if (segmentList[i]->type != 3)
				{
					if ((radius / maxRad) <= 0.4)
					{
						segmentList[i]->segmentColor.r = (1 - (radius * 2.5 / maxRad));
						segmentList[i]->segmentColor.g = (radius * 2.5 / maxRad);
						segmentList[i]->segmentColor.b = 0;
						colorStore = segmentList[i]->segmentColor;
					}

					else if ((radius / maxRad) <= 0.8)
					{
						segmentList[i]->segmentColor.r = 0;
						segmentList[i]->segmentColor.g = 1 - ((radius - (maxRad / 2.5)) * 2.5 / maxRad);
						segmentList[i]->segmentColor.b = ((radius - (maxRad / 2.5)) * 2.5 / maxRad) - 0.2;
						colorStore = segmentList[i]->segmentColor;
					}

					else
					{
						segmentList[i]->segmentColor.r = 0;
						segmentList[i]->segmentColor.g = 0;
						segmentList[i]->segmentColor.b = 1 - radius / maxRad;
						colorStore = segmentList[i]->segmentColor;
					}
				}
				else
				{
					if (alertType == 1)
					{
						segmentList[i]->segmentColor.r = (1 - (radius / maxRad)) * 2;
						segmentList[i]->segmentColor.g = (1 - (radius / maxRad)) * 2;
						segmentList[i]->segmentColor.b = (1 - (radius / maxRad)) * 2;
					}

					else if (alertType == 2)
					{
						segmentList[i]->segmentColor.r = (1 - (radius / maxRad)) * 2;
						segmentList[i]->segmentColor.g = 0;
						segmentList[i]->segmentColor.b = 0;
					}

					else
					{
						segmentList[i]->segmentColor.r = 0;
						segmentList[i]->segmentColor.g = 0;
						segmentList[i]->segmentColor.b = (1 - (radius / maxRad)) * 2;
					}
					
				}
			}

			segmentList[i]->position.x = radius * cos(2 * Math::PI * i / numSides) + position.x;
			segmentList[i]->position.y = radius * sin(2 * Math::PI * i / numSides) + position.y;
			float lengthOfSide = 2 * radius * tan(Math::PI / numSides);

			if (segmentList[i]->type != 2)
				segmentList[i]->scale.Set(lengthOfSide / 2, 1, 1);
			else
				segmentList[i]->scale.Set(lengthOfSide / 2, 6, 1);

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

	for (int i = 0; i < segmentList.size(); ++i)
	{
		if (radius >= maxRad && segmentList[i]->attached == false)
		{
			delete segmentList[i];
			segmentList.erase(segmentList.begin() + i);
			continue;
		}

		if (segmentList[i]->attached)
		{
			segmentList[i]->lifeTime -= dt/1.2;

			if (segmentList[i]->lifeTime <= 0)
			{
				if (segmentList.size() <= numSides / 3)
				{
					for (int i = 0; i < GO.size(); ++i)
					{
						if (GO[i] != NULL)
							GO[i]->visible = false;
					}
				}

				delete segmentList[i];
				segmentList.erase(segmentList.begin() + i);
			}
		}
	}
}

float Sonar::GetSonarRadius()
{
	return radius;

}