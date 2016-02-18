#include "GameObject.h"

GameObject::GameObject() : enableCollision(false), active(true), currTile(0, 0), rotation(0), topLeft(0, 0, 0), bottomRight(0, 0, 0), name("NIL")
{

}

GameObject::~GameObject()
{

}

void GameObject::Init(Vector3 position)
{
	this->position = position;
	enableCollision = false;
}

void GameObject::Update(double dt)
{
	
}