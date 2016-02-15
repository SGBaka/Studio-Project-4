#include "CharacterObject.h"

CharacterObject::CharacterObject() : Holding(NULL), f_HungerMeter(100.f), f_PeeMeter(100.f), name("UNNAMED"), rotation(0.f), SO_USING(NULL)
{
	Holding = NULL;
}

CharacterObject::~CharacterObject()
{

}

void CharacterObject::Init(Vector3 position)
{
	this->position = position;
	rotation = 0.f;
	Holding = NULL;
}

void CharacterObject::Update(double dt)
{
	
}

std::string CharacterObject::getState(void)
{
	return "None";
}