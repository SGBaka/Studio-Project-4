/******************************************************************************/
/*!
\file	Application.h
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
To handle the window and the application
*/
/******************************************************************************/
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

class SceneManager;

/******************************************************************************/
/*!
		Class Application:
\brief	A 4 by 4 matrix
*/
/******************************************************************************/
class Application
{
public:	
	Application();
	~Application();
	void Init();
	void Run();
	void Exit();
	static int GetWindowWidth();
	static int GetWindowHeight();
	static bool GetWindowFocusStatus();
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
	static void GetMousePos(double &_x, double &_y);
	static void SetMouseinput(double X, double Y);
	static void SetCursor(bool enable);
	static void GetMouseChange(float &x, float &y);
	static void resizeWindow(int width, int height, bool fullscreen = false);
	static void fullscreentoggle(void);
	static void waitforload(void);
	static void closeApplication(void);

private:
	/******************************************************************************/
	/*!
			StopWatch m_timer:
	\brief	A timer to calculate frame time
	*/
	/******************************************************************************/
	StopWatch m_timer;

	SceneManager *S_MANAGER;

	double m_dElapsedTime;
	double m_dAccumulatedTime_Thread;
	/******************************************************************************/
	/*!
			int windowWidth:
	\brief	The width of the window
	*/
	/******************************************************************************/
	static int i_WINDOW_WIDTH;

	/******************************************************************************/
	/*!
			int windowHeight:
	\brief	The height of the window
	*/
	/******************************************************************************/
	static int i_WINDOW_HEIGHT;
};

#endif