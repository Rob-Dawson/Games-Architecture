#include "Window.h"

Window *Window::TheWindow = 0;

Window::Window(const int &width, const int &height)
	: m_renderer{ 0 }, m_Width {width}, m_Height{ height }
{
	TheWindow = this;
}


Window::~Window()
{
}
