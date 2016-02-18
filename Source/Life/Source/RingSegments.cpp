#include "RingSegments.h"

RingSegments::RingSegments() 
: posStart(0,0,0)
, posEnd(0,0,0)
, endTrans(0,0,0)
, startTrans(0,0,0)
{

}

RingSegments::~RingSegments()
{

}

void RingSegments::Init(Vector3 position)
{
	this->position = posStart = posEnd = position;
}

void RingSegments::Update(double dt)
{
	//this->position += this->velocity * dt;
}