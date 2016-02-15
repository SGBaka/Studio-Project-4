#ifndef ITEMOBJECT_H
#define ITEMOBJECT_H

#include "Vector3.h"
#include "Mesh.h"
#include "GameObject.h"

class ItemObject : public GameObject
{
public:
	ItemObject();
	virtual ~ItemObject();

	bool isPlaced;
	std::string owner;

private:

};

#endif