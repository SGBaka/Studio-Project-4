
#ifndef CUSTOMCAMTHIRDPERSON_H
#define CUSTOMCAMTHIRDPERSON_H

#include "Camera.h"
#include "GabrielDLC.h"

class CustomCamThirdPerson : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;
	Vector3 Origin;
	Vector3 Offset;
	float mouseSensitivity;
	float f_currentPitch, f_currentYaw;
	float f_pitchLimit;
	
	CustomCamThirdPerson();
	~CustomCamThirdPerson();
	virtual void Init(const Vector3& Origin, const Vector3& Lookat, const Vector3& up, const Vector3&Offset, const float mouseSensitivity);
	virtual void Update(double dt);
	void setOffset(const Vector3 &NewOffset);
	void setOffsetX(float X);
	void setOffsetY(float Y);
	void setOffsetZ(float Z);
	void rotateCamVertical(float degrees);
private:
	double mouseX, mouseY;
	double Xaxis, Yaxis;

	//Setting this will make the Offset move towards this value	
	Vector3 MoveOffset;

	friend CustomCamThirdPerson operator+(CustomCamThirdPerson C1, const Vector3 Delta);
};

#endif