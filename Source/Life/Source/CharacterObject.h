#ifndef CHARACTEROBJECT_H
#define CHARACTEROBJECT_H

#include "GameObject.h"
#include "maploader.h"
#include "SpecialObject.h"
#include <string>

class CharacterObject : public GameObject
{
public:
	CharacterObject();
	virtual ~CharacterObject();

	//Directional Vector of where character is looking
	Vector3 lookat;
	float rotation;
	GameObject *Holding;

	std::string name;
	
	//virtual void communicate(std::string command, std::string Target, CharacterObject *Sender);
	virtual void Init(Vector3 position);
	virtual void Update(double dt);

	virtual std::string getState(void);
private:

};

#endif