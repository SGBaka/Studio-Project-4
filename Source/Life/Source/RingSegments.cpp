#include "RingSegments.h"

RingSegments::RingSegments()
{

}

RingSegments::~RingSegments()
{

}

void RingSegments::Init(Vector3 position)
{
	this->position = position;
}

void RingSegments::Update(double dt)
{
	this->position += this->velocity * dt;
}