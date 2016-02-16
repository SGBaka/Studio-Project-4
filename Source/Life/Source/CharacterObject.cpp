#include "CharacterObject.h"

CharacterObject::CharacterObject() : Holding(NULL), name("UNNAMED"), rotation(0.f)
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