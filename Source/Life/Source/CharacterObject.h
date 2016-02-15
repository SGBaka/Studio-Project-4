#ifndef CHARACTEROBJECT_H
#define CHARACTEROBJECT_H

#include "GameObject.h"
#include "maploader.h"
#include "SpecialObject.h"
#include <string>

struct Role
{
	std::string jobtodo;
	float timeStamp;

	Role(std::string command, float commandedTime)
	{
		jobtodo = command;
		timeStamp = commandedTime;
	}
};

class CharacterObject : public GameObject
{
public:
	CharacterObject();
	virtual ~CharacterObject();

	//Directional Vector of where character is looking
	Vector3 lookat;
	float rotation;
	GameObject *Holding;


	std::vector<Role> R_Rolelist;

	std::string name;
	float f_HungerMeter;
	float f_PeeMeter;
	bool b_NeedtoEat;
	bool b_NeedtoPee;
	SpecialObject *SO_USING;//The special object you are occupying
	
	//virtual void communicate(std::string command, std::string Target, CharacterObject *Sender);
	virtual void Init(Vector3 position);
	virtual void Update(double dt);

	virtual std::string getState(void);
private:

};

#endif