#include "Window.h"

deq::Window::Window(uint16 width, uint16 height, std::string title, uint32 style, uint16 posx, uint16 posy)
{
	uint32 windowStyle = 0x00000000;
	if (style & WINDOW_MAXAMIZED)
	{
		windowStyle = windowStyle | WS_MAXIMIZE | WS_OVERLAPPEDWINDOW;
	}
	if (style & WINDOW_MINIMIZED)
	{
		windowStyle = windowStyle | WS_MINIMIZE | WS_OVERLAPPEDWINDOW;
	}
	if (style & WINDOW_BORDERLESS)
	{
		windowStyle = windowStyle | WS_POPUP;
	}
	if (style & WINDOW_VISIBLE)
	{
		windowStyle = windowStyle | WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	}

	HINSTANCE hInstance = ApplicationContext::getInstance().getAppHandle();
	m_wstitle = stringToWstring(title);
	m_wtitle = (LPCSTR)m_wstitle.c_str();

	ZeroMemory(&m_wc, sizeof(WNDCLASSEX));

	m_wc.cbSize = sizeof(WNDCLASSEX);
	m_wc.style = CS_HREDRAW | CS_VREDRAW;
	m_wc.lpfnWndProc = WindowProc;
	m_wc.hInstance = hInstance;
	m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	m_wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	m_wc.lpszClassName = "WindowClassDeq";

	RegisterClassEx(&m_wc);

	// Make sure the client area is appropriate
	RECT windowRect = { 0, 0, width, height };
	AdjustWindowRect(&windowRect, windowStyle, FALSE);

	m_hWnd = CreateWindowEx(NULL,
		"WindowClassDeq",					// Window Class Name
		m_wtitle,							// Title
		windowStyle,						// Style
		posx,								// Posx
		posy,								// Posy
		windowRect.right - windowRect.left,	// Width
		windowRect.bottom - windowRect.top,	// Height
		NULL,								// No parent
		NULL,								// No menus
		hInstance,							// App handle
		NULL);								// No multiple windows

	ShowWindow(m_hWnd, SW_SHOW);

	m_isOpen = true;
}

bool deq::Window::pollEvents(Event& e)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);

		// Go through all the other events we need
		Event queueEvent;
		bool validEvent = true;
		bool mouseButtonEvent = false;
		int xButtonNr = 0;
		switch (msg.message)
		{
		case WM_QUIT:
			m_isOpen = false;
			queueEvent.type = EV_QUIT;
			break;

		case WM_KEYDOWN:
			queueEvent.type = EV_KEYDOWN;
			queueEvent.vKey = (VirtualKeyCode)msg.wParam; // TODO: fix casting to safer, less ugly code
			break;

		case WM_KEYUP:
			queueEvent.type = EV_KEYUP;
			queueEvent.vKey = (VirtualKeyCode)msg.wParam; // F12 and F10 broken
			break;

		case WM_MOUSEMOVE:
			queueEvent.type = EV_MOUSEMOVED;

			// Cursor position relative to window
			queueEvent.mouse.xPos = GET_X_LPARAM(msg.lParam);
			queueEvent.mouse.yPos = GET_Y_LPARAM(msg.lParam);

			// Relative movement
			queueEvent.mouse.xRel = queueEvent.mouse.xPos - m_lastMouseEvent.xPos;
			queueEvent.mouse.yRel = queueEvent.mouse.yPos - m_lastMouseEvent.yPos;

			m_lastMouseEvent = queueEvent.mouse;
			break;

			// TODO: BETTER MOUSE SOLUTION
			// Mouse middle button down
		case WM_MBUTTONDOWN:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONDOWN;
			queueEvent.vKey = M_MBUTTON;
			break;

			// Mouse middle button up
		case WM_MBUTTONUP:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONUP;
			queueEvent.vKey = M_MBUTTON;
			break;

			// Mouse left button down
		case WM_LBUTTONDOWN:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONDOWN;
			queueEvent.vKey = M_LBUTTON;
			break;

			// Mouse left button up
		case WM_LBUTTONUP:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONUP;
			queueEvent.vKey = M_LBUTTON;
			break;

			// Mouse right button down
		case WM_RBUTTONDOWN:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONDOWN;
			queueEvent.vKey = M_RBUTTON;
			break;

			// Mouse right button up
		case WM_RBUTTONUP:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONUP;
			queueEvent.vKey = M_RBUTTON;
			break;

			// Mouse xbutton down
		case WM_XBUTTONDOWN:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONDOWN;
			xButtonNr = GET_XBUTTON_WPARAM(msg.wParam);
			if (xButtonNr == 1)
				queueEvent.vKey = M_XBUTTON1;
			else
				queueEvent.vKey = M_XBUTTON2;
			break;

			// Mouse xbutton up
		case WM_XBUTTONUP:
			mouseButtonEvent = true;
			queueEvent.type = EV_MBUTTONUP;
			queueEvent.vKey = M_RBUTTON;
			xButtonNr = GET_XBUTTON_WPARAM(msg.wParam);
			if (xButtonNr == 1)
				queueEvent.vKey = M_XBUTTON1;
			else
				queueEvent.vKey = M_XBUTTON2;
			break;

			// If none of the above, register as a invalid event (doesn't get pushed on to the queue)
		default:
			validEvent = false;
		}

		if (mouseButtonEvent)
		{
			// Cursor position relative to window
			queueEvent.mouse.xPos = GET_X_LPARAM(msg.lParam);
			queueEvent.mouse.yPos = GET_Y_LPARAM(msg.lParam);

			// Relative movement
			queueEvent.mouse.xRel = queueEvent.mouse.xPos - m_lastMouseEvent.xPos;
			queueEvent.mouse.yRel = queueEvent.mouse.yPos - m_lastMouseEvent.yPos;

			m_lastMouseEvent = queueEvent.mouse;
		}

		if (validEvent)
		{
			// Send it off to the queue
			m_eventQueue.push(queueEvent);
		}
	}

	// Check if we have more events to send back
	bool moreEvents = false;
	if (m_eventQueue.size() > 0)
	{
		moreEvents = true;

		e = m_eventQueue.front();
		m_eventQueue.pop();
	}

	return moreEvents;
}

void deq::Window::setTitle(std::string title)
{
	m_wstitle = stringToWstring(title);
	m_wtitle = (LPCSTR)m_wstitle.c_str();

	SetWindowText(m_hWnd, m_wtitle);
}

LRESULT CALLBACK deq::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Sort through and find what code to run for the message given
	switch (message)
	{
		// This message is read when the window is closed
	case WM_DESTROY:
	{
		// Close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}