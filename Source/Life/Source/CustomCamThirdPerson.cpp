/******************************************************************************/
/*!
\file	CustomCamThirdPerson.cpp
\author Gabriel Wong Choon Jieh
\par	email: AuraTigital\@gmail.com
\brief
Custom made camera
*/
/******************************************************************************/
#include "CustomCamThirdPerson.h"
#include "Application.h"
#include "Mtx44.h"

CustomCamThirdPerson::CustomCamThirdPerson()
{

}

CustomCamThirdPerson::~CustomCamThirdPerson()
{

}

/******************************************************************************/
/*!
\brief
Initialize default camera position, target, and up vector
\param pos
position of camera
\param target
where camera is looking at
\param up
the up direction of the camera
\param mouseSensitivity
the sensitivity of the mouse control
*/
/******************************************************************************/
void CustomCamThirdPerson::Init(const Vector3& Origin, const Vector3& Lookat, const Vector3& up, const Vector3&Offset, const float mouseSensitivity)
{
	this->position = Origin + Offset;
	this->Origin = Origin;
	this->target = this->position + Lookat;
	if (Offset != 0)
	{
		this->Offset = Offset;
		this->MoveOffset = this->Offset;
	}
	else
	{
		this->Offset = Vector3(0, 0, -1);
		this->MoveOffset = this->Offset;
	}

	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = right.Cross(view).Normalized();
	this->mouseSensitivity = mouseSensitivity;
	mouseX = 0.0;
	mouseY = 0.0;

	f_currentPitch = CalAnglefromPosition(target, position, false);
	f_currentYaw = CalAnglefromPosition(target, position, true);
	f_pitchLimit = 50.f;
}

/******************************************************************************/
/*!
\brief
updates camera rotation
\param dt
delta time
*/
/******************************************************************************/
void CustomCamThirdPerson::Update(double dt)
{
	double Xaxis = (Application::GetWindowWidth() * 0.5);
	double Yaxis = (Application::GetWindowHeight() * 0.5);
	Application::GetMousePos(mouseX, mouseY);
	mouseX = Xaxis - mouseX;
	mouseY = Yaxis - mouseY;
	mouseX *= 0.1;
	mouseY *= 0.1;
	mouseX *= static_cast<double>(mouseSensitivity);
	mouseY *= static_cast<double>(mouseSensitivity);

	
	if (mouseX != 0)
	{
		f_currentYaw += static_cast<float>(mouseX);

		if (f_currentYaw > 360)
		{
			f_currentYaw -= 360.f;
		}
		else if (f_currentYaw < -360)
		{
			f_currentYaw += 360.f;
		}
	}

	if (Application::IsKeyPressed(VK_MBUTTON))
	{
		if (!(MoveOffset.z > -10 && mouseY > 0) && !(MoveOffset.z < -40 && mouseY < 0))
		{
			MoveOffset.z += static_cast<float>(mouseY);
		}

	}
	else if (mouseY != 0)
	{
		if (mouseY + f_currentPitch > f_pitchLimit && mouseY > 0)
		{
			mouseY = f_pitchLimit - f_currentPitch;

			if (f_currentPitch >= f_pitchLimit)
			{
				mouseY = 0.f;
			}
		}

		else if (mouseY + f_currentPitch < -f_pitchLimit && mouseY < 0)
		{
			mouseY = -f_pitchLimit - f_currentPitch;

			if (f_currentPitch <= -f_pitchLimit)
			{
				mouseY = 0.f;
			}
		}

		if (mouseY != 0)
		{
			f_currentPitch += static_cast<float>(mouseY);
		}
	}

	//TO BE OPTIMISED
	Mtx44 RotationYaw, RotationPitch;
	RotationYaw.SetToRotation(f_currentYaw, 0, 1, 0);

	Vector3 view = (target - position).Normalize();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalize();

	RotationPitch.SetToRotation(f_currentPitch, right.x, right.y, right.z);

	position = Origin + (RotationPitch * RotationYaw * Offset);
	target = position + (RotationPitch * RotationYaw * Vector3(0, 0, 1));

	view = (target - position).Normalize();
	right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalize();

	RotationPitch.SetToRotation(f_currentPitch, right.x, right.y, right.z);

	position = Origin + (RotationPitch * RotationYaw * Offset);
	target = position + (RotationPitch * RotationYaw * Vector3(0, 0, 1));


	if (Offset != MoveOffset)
	{
		if (Offset.x < MoveOffset.x + 0.01f && Offset.x > MoveOffset.x - 0.01f && Offset.y < MoveOffset.y + 0.01f && Offset.y > MoveOffset.y - 0.01f && Offset.z < MoveOffset.z + 0.01f && Offset.z > MoveOffset.z - 0.01f)
		{
			Offset = MoveOffset;
		}
		else
		{
			Vector3 v3_temp1 = MoveOffset - Offset;
			Offset += v3_temp1 * static_cast<float>(dt)* 10;
		}
	}
	
	if (mouseY != 0 || mouseX != 0)
	{
		Application::SetMouseinput(Xaxis, Yaxis);
	}
}

/******************************************************************************/
/*!
\brief
sets the offset target to offset to
\param Offset
the offset to set to
*/
/******************************************************************************/
void CustomCamThirdPerson::setOffset(const Vector3 &NewOffset)
{
	MoveOffset = NewOffset;
}

/******************************************************************************/
/*!
\brief
sets the offset target x to offset to
\param X
the offset x to set to
*/
/******************************************************************************/
void CustomCamThirdPerson::setOffsetX(float X)
{
	MoveOffset.x = X;
}

/******************************************************************************/
/*!
\brief
sets the offset target y to offset to
\param Y
the offset y to set to
*/
/******************************************************************************/
void CustomCamThirdPerson::setOffsetY(float Y)
{
	MoveOffset.y = Y;
}

/******************************************************************************/
/*!
\brief
sets the offset target z to offset to
\param Z
the offset z to set to
*/
/******************************************************************************/
void CustomCamThirdPerson::setOffsetZ(float Z)
{
	MoveOffset.z = Z;
}

/******************************************************************************/
/*!
\brief
rotates camera pitch
\param degrees
the degrees to rotate by
*/
/******************************************************************************/
void CustomCamThirdPerson::rotateCamVertical(float degrees)
{
	if (degrees + f_currentPitch > f_pitchLimit && degrees > 0)
	{
		degrees = f_pitchLimit - f_currentPitch;

		if (f_currentPitch >= f_pitchLimit)
		{
			degrees = 0.f;
		}
	}

	else if (degrees + f_currentPitch < -f_pitchLimit && degrees < 0)
	{
		degrees = -f_pitchLimit - f_currentPitch;

		if (f_currentPitch <= -f_pitchLimit)
		{
			degrees = 0.f;
		}
	}

	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();
	Mtx44 rotation;
	target -= position;
	f_currentPitch += degrees;
	rotation.SetToRotation(static_cast<float>(degrees), right.x, right.y, right.z);
	target = rotation * target;
	target += position;
}

CustomCamThirdPerson operator+(CustomCamThirdPerson C1, const Vector3 Delta)
{
	C1.target = C1.target + Delta;
	C1.position = C1.position + Delta;
	C1.Origin = C1.Origin + Delta;
	return C1;
}