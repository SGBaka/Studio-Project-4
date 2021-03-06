/******************************************************************************/
/*!
\file	Application.cpp
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
To handle the window and the application
*/
/******************************************************************************/
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "SceneManager.h"

GLFWwindow* m_window;
unsigned char FPS = 120; // FPS of this game
unsigned int frameTime = 1000 / FPS; // time for each frame
bool frameLimiter = false;
bool FULL_SCREEN = true;
const char* WIN_NAME = "Project 42";
const int initWidth = 1280;
const int initHeight = 720;
int Application::i_WINDOW_WIDTH = initWidth;
int Application::i_WINDOW_HEIGHT = initHeight;
static bool togglefullscreen = true;
static bool loaddone = false;
static bool b_quitProgram = false;

//http://www.color-hex.com/color-palette/7098

/******************************************************************************/
/*!
\brief
Handles resizing of the window

\param window - The OpenGL Window
\param w - The width of the viewport
\param h - The height of the viewport
*/
/******************************************************************************/
void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

/******************************************************************************/
/*!
\brief
Define an error callback

\param error - The error id
\param description - The error description
*/
/******************************************************************************/
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

/******************************************************************************/
/*!
\brief
Define the key input callback

\param window - The OpenGL Window
\param key - The key pressed
\param scancode - The scan code
\param action - The action taken
\param mods - The mods
*/
/******************************************************************************/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

/******************************************************************************/
/*!
\brief
Detect user input

\param key - The key to test if is pressed

\return If the key provided was pressed
*/
/******************************************************************************/
bool Application::IsKeyPressed(unsigned short key)
{
	return ((GetAsyncKeyState(key) & 0x8001) != 0);
}

/******************************************************************************/
/*!
\brief
Detect mouse input
*/
/******************************************************************************/
bool Application::IsMousePressed(unsigned short key) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, key) != 0;
}

/******************************************************************************/
/*!
\brief
Detect user input for mouse

\param _x - Reference to a double to store the horizontal position of the mouse
\param _y - Reference to a double to store the vertical position of the mouse
*/
/******************************************************************************/
void Application::GetMousePos(double &_x, double &_y)
{
	double *xptr = new double;
	double *yptr = new double;

	glfwGetCursorPos(m_window, xptr, yptr);

	_x = *xptr;
	_y = *yptr;

	delete xptr;
	delete yptr;
}

/******************************************************************************/
/*!
\brief
Sets the mouse position

\param X - Position to set the horizontal position of the mouse
\param Y - Position to set the vertical position of the mouse
*/
/******************************************************************************/
void Application::SetMouseinput(double X, double Y)
{
	glfwSetCursorPos(m_window, X, Y);
}

/******************************************************************************/
/*!
\brief
Sets the cursor type

\param enable - Whether the cursor should be disabled or not
*/
/******************************************************************************/
void Application::SetCursor(bool enable)
{
	if (enable)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		SetMouseinput(GetWindowWidth() / 2, GetWindowHeight() / 2);
	}
}

/******************************************************************************/
/*!
\brief
Calculates the distance moved by mouse from the center. !!Will recenter the cursor
*/
/******************************************************************************/
void Application::GetMouseChange(float &x, float &y)
{
	double midScreenX = (Application::GetWindowWidth() / 2);
	double midScreenY = (Application::GetWindowHeight() / 2);

	double mousePosX;
	double mousePosY;

	GetMousePos(mousePosX, mousePosY);

	x = static_cast<float>((midScreenX - mousePosX) / 32);
	y = static_cast<float>((midScreenY - mousePosY) / 32);

	SetMouseinput(midScreenX, midScreenY);
}

/******************************************************************************/
/*!
\brief
Resizes the window
*/
/******************************************************************************/
void Application::resizeWindow(int width, int height, bool fullscreen)
{
	i_WINDOW_WIDTH = width;
	i_WINDOW_HEIGHT = height;
	glfwSetWindowSize(m_window, width, height);
}

/******************************************************************************/
/*!
\brief
toggles fullscreen
*/
/******************************************************************************/
void Application::fullscreentoggle(void)
{
	togglefullscreen = true;
}

/******************************************************************************/
/*!
\brief
freezes delta time for a tick when loading something big inside the scene
*/
/******************************************************************************/
void Application::waitforload(void)
{
	loaddone = false;
}

/******************************************************************************/
/*!
\brief
Closes the application
*/
/******************************************************************************/
void Application::closeApplication(void)
{
	b_quitProgram = true;
}

/******************************************************************************/
/*!
\brief
Return the current Width of the window

\return int with the current Width of the window
*/
/******************************************************************************/
int Application::GetWindowWidth()
{
	return i_WINDOW_WIDTH;
}

/******************************************************************************/
/*!
\brief
Return the current Height of the window
\return int with the current Height of the window
*/
/******************************************************************************/
int Application::GetWindowHeight()
{
	return i_WINDOW_HEIGHT;
}

/******************************************************************************/
/*!
\brief
Checks if the window is currently in focus

\return the focus state of the window
*/
/******************************************************************************/
bool Application::GetWindowFocusStatus()
{
	return glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
}

/******************************************************************************/
/*!
\brief
Default constructor
*/
/******************************************************************************/
Application::Application()
{

}

/******************************************************************************/
/*!
\brief
Destructor
*/
/******************************************************************************/
Application::~Application()
{

}

/******************************************************************************/
/*!
\brief
Initialise program
*/
/******************************************************************************/
void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	const GLFWvidmode *win_data = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, win_data->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, win_data->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, win_data->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, win_data->refreshRate);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	FPS = win_data->refreshRate; // FPS of this game
	frameTime = 1000 / FPS;

	//Create a window and create its OpenGL context
	if (FULL_SCREEN)
	{
		i_WINDOW_WIDTH = win_data->width;
		i_WINDOW_HEIGHT = win_data->height;
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);
		m_window = glfwCreateWindow(win_data->width, win_data->height, WIN_NAME, NULL, NULL);
	}
	else
	{
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);
		m_window = glfwCreateWindow(i_WINDOW_WIDTH, i_WINDOW_HEIGHT, WIN_NAME, NULL, NULL);
	}

	glfwSetWindowSizeCallback(m_window, resize_callback);
	//If the window couldn't be created
	if (!m_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	togglefullscreen = false;
	m_dElapsedTime = 0.0;
	m_dAccumulatedTime_ThreadOne = 0.0;
	m_dAccumulatedTime_ThreadTwo = 0.0;

	S_MANAGER = SceneManager::Instance();
	S_MANAGER->Init(SceneManager::S_MAIN_MENU_SPLASH);
}

/******************************************************************************/
/*!
\brief
Continuous loop to run the application
*/
/******************************************************************************/
void Application::Run()
{
	loaddone = false;

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!b_quitProgram)
	{
		m_dElapsedTime = m_timer.getElapsedTime();
		m_dAccumulatedTime_ThreadOne += m_dElapsedTime;
		m_dAccumulatedTime_ThreadTwo += m_dElapsedTime;

		if (m_dAccumulatedTime_ThreadOne > 0.016)
		{
			if (!loaddone)
			{
				loaddone = true;
				m_dAccumulatedTime_ThreadOne = 0.0;
			}
			//S_MANAGER->Update(m_dAccumulatedTime_ThreadOne);
			S_MANAGER->Update(m_dElapsedTime);
			m_dAccumulatedTime_ThreadOne = 0.0;
		}
		if (m_dAccumulatedTime_ThreadTwo > 0.03)
		{
			//Update
			m_dAccumulatedTime_ThreadTwo = 0.0;
		}

		S_MANAGER->Render();

		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();

		if (frameLimiter)
		{
			m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.
		}

		// Quits program if window is closed or esc is pressed

		/*if (IsKeyPressed(VK_ESCAPE))
		{
		b_quitProgram = true;
		}*/


		if (togglefullscreen || IsKeyPressed(VK_F11))
		{
			if (!FULL_SCREEN)
			{
				S_MANAGER->getCurScene()->Exit();
				S_MANAGER->getCurScene()->CleanShaders();

				FULL_SCREEN = true;
				glfwDestroyWindow(m_window);
				const GLFWvidmode *win_data = glfwGetVideoMode(glfwGetPrimaryMonitor());
				i_WINDOW_WIDTH = win_data->width;
				i_WINDOW_HEIGHT = win_data->height;
				glfwWindowHint(GLFW_DECORATED, GL_FALSE);
				m_window = glfwCreateWindow(win_data->width, win_data->height, WIN_NAME, NULL, NULL);
				glfwMakeContextCurrent(m_window);

				S_MANAGER->getCurScene()->InitShaders();
				S_MANAGER->getCurScene()->Init();
			}
			else
			{
				S_MANAGER->getCurScene()->Exit();
				S_MANAGER->getCurScene()->CleanShaders();

				FULL_SCREEN = false;
				i_WINDOW_WIDTH = initWidth;
				i_WINDOW_HEIGHT = initHeight;
				glfwDestroyWindow(m_window);
				glfwWindowHint(GLFW_DECORATED, GL_TRUE);
				m_window = glfwCreateWindow(i_WINDOW_WIDTH, i_WINDOW_HEIGHT, WIN_NAME, NULL, NULL);
				glfwMakeContextCurrent(m_window);

				S_MANAGER->getCurScene()->InitShaders();
				S_MANAGER->getCurScene()->Init();
			}
			togglefullscreen = false;
		}
	}

	S_MANAGER->destroy();
}

/******************************************************************************/
/*!
\brief
Terminates the program
*/
/******************************************************************************/
void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}