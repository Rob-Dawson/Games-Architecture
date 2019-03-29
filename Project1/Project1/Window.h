#pragma once
#include <Windows.h>
class Renderer;
class Window
{
public:
	Window(const int &width, const int &height);
	~Window();

public:
	virtual void Initialise() = 0;

public:
	int			m_Width;
	int			m_Height;
	Renderer	*m_renderer;
public:
	static Window *TheWindow;
};

