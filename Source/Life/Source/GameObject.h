#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Vector3.h"
#include "Mesh.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	Mesh *mesh;
	bool enableCollision;
	bool active;

	Vector3 position;
	Vector3 velocity;
	Vector3 scale;
	double rotation;

	virtual void Init(Vector3 position);
	virtual void Update(double dt);
	Vector3 currTile;
private:
	
};

#endif