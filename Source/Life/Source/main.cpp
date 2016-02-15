#include <iostream>
#include <thread>
#include "Application.h"
/******************************************************************************/
/*!
\file	main.cpp
\author Wen Sheng Tang
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Main to run the application
*/
/******************************************************************************/
Application app;

void Thread1()
{

}

void Thread2()
{
	app.Run();
}

void main(void)
{
	app.Init();
	//std::thread T1(Thread1);
	//std::thread T2(Thread2);

	app.Run();
	//T1.join();
	//T2.join();
	app.Exit();
}