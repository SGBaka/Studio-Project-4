#ifndef SPECIALOBJECT_H
#define SPECIALOBJECT_H

#include "GameObject.h"

class SpecialObject : public GameObject
{
public:
	SpecialObject();
	virtual ~SpecialObject();
	std::string tag;
	bool Occupied;
private:

};

#endif