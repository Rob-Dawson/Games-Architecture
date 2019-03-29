#pragma once
#include "Window.h"

class Window_DX :
	public Window
{
public:
	Window_DX(const int &width, const int &height, const HINSTANCE hInstance, const int nCmdShow);
	virtual ~Window_DX();

	bool InitWindow(const int &width, const int &height, const HINSTANCE hInstance, const int nCmdShow);
	virtual void Initialise();

	static LRESULT CALLBACK WindowProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam);

private:
	HWND m_Handle;
};

