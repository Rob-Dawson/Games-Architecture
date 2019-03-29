#include "Window_DX.h"
#include "Renderer_DX.h"

Window_DX::Window_DX(const int &width, const int &height, const HINSTANCE hInstance, const int nCmdShow)
	: Window(width, height)
{
	InitWindow(width, height, hInstance, nCmdShow);
}


bool Window_DX::InitWindow(const int &width, const int &height, const HINSTANCE hInstance, const int nCmdShow)
{
	WNDCLASSEX wndClass{ 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = L"WindowClass";

	if (!RegisterClassEx(&wndClass))
	{
		return false;
	}

	RECT wr = { 0, 0, width, height};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	//Create Window
	m_Handle = CreateWindowEx(NULL, L"WindowClass", L"CUBES", WS_OVERLAPPEDWINDOW,
		width, height, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(m_Handle, nCmdShow);
	return true;
}

void Window_DX::Initialise()
{
	m_renderer = new Renderer_DX(m_Handle);
	m_renderer->InitDevice(m_Width, m_Height);
	MSG msg{ nullptr };

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//m_renderer->BeginFrame
		m_renderer->ClearScreen();
		m_renderer->SwapBuffer();
	}
	m_renderer->ShutDown();
}

LRESULT Window_DX::WindowProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		//TODO Add functionality for buttons
	}
}

Window_DX::~Window_DX() = default;