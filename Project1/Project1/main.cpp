#include <Windows.h>
#include "Window_DX.h"

static const int screenWidth = 800;
static const int screenHeight = 600;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window_DX window(screenWidth, screenHeight, hInstance, nCmdShow);
	window.Initialise();
}