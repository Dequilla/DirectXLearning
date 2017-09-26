#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <queue>

#include "String.h"
#include "Utility.h"

namespace deq
{

	enum WindowStyle
	{
		WINDOW_MAXAMIZED = 0x00000001, // Window is maxamized (includes titlebar, exit, maxamize and minimize)
		WINDOW_MINIMIZED = 0x00000002, // Window is minimized (includes titlebar, exit, maxamize and minimize)
		WINDOW_BORDERLESS = 0x00000004, // Window is borderless
		WINDOW_VISIBLE = 0x00000008  // Window is visible (includes titlebar, exit, maxamize and minimize)
	};

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	enum VirtualKeyCode
	{
		NO_KEY = 0x00,	// Reserved
		M_LBUTTON = 0x01,
		M_RBUTTON = 0x02,
		K_CANCEL = 0x03,
		M_MBUTTON = 0x04,
		M_XBUTTON1 = 0x05,
		M_XBUTTON2 = 0x06,
		K_BACKSPACE = 0x08,
		K_TAB = 0x09,
		K_CLEAR = 0x0C,
		K_RETURN = 0x0D,
		K_SHIFT = 0x10,
		K_CONTROL = 0x11,
		K_ALT = 0x12,
		K_PAUSE = 0x13,
		K_CAPSLOCK = 0x14,
		K_ESCAPE = 0x1B,
		K_SPACE = 0x20,
		K_PAGEUP = 0x21,
		K_PAGEDOWN = 0x22,
		K_END = 0x23,
		K_HOME = 0x24,
		K_LEFT = 0x25,
		K_UP = 0x26,
		K_RIGHT = 0x27,
		K_DOWN = 0x28,
		K_PRINTSCREEN = 0x2C,
		K_INSERT = 0x2D,
		K_DELETE = 0x2E,

		K_0 = 0x30,
		K_1 = 0x31,
		K_2 = 0x32,
		K_3 = 0x33,
		K_4 = 0x34,
		K_5 = 0x35,
		K_6 = 0x36,
		K_7 = 0x37,
		K_8 = 0x38,
		K_9 = 0x39,

		K_A = 0x41,
		K_B = 0x42,
		K_C = 0x43,
		K_D = 0x44,
		K_E = 0x45,
		K_F = 0x46,
		K_G = 0x47,
		K_H = 0x48,
		K_I = 0x49,
		K_J = 0x4A,
		K_K = 0x4B,
		K_L = 0x4C,
		K_M = 0x4D,
		K_N = 0x4E,
		K_O = 0x4F,
		K_P = 0x50,
		K_Q = 0x51,
		K_R = 0x52,
		K_S = 0x53,
		K_T = 0x54,
		K_U = 0x55,
		K_V = 0x56,
		K_W = 0x57,
		K_X = 0x58,
		K_Y = 0x59,
		K_Z = 0x5A,

		K_LEFTWIN = 0x5B,
		K_RIGHTWIN = 0x5C,

		K_NUMPAD0 = 0x60,
		K_NUMPAD1 = 0x61,
		K_NUMPAD2 = 0x62,
		K_NUMPAD3 = 0x63,
		K_NUMPAD4 = 0x64,
		K_NUMPAD5 = 0x65,
		K_NUMPAD6 = 0x66,
		K_NUMPAD7 = 0x67,
		K_NUMPAD8 = 0x68,
		K_NUMPAD9 = 0x69,

		K_MULTIPLY = 0x6A,
		K_ADD = 0x6B,
		K_SUBTRACT = 0x6D,
		K_DECIMAL = 0x6E,
		K_DIVIDE = 0x6F,

		K_F1 = 0x70,
		K_F2 = 0x71,
		K_F3 = 0x72,
		K_F4 = 0x73,
		K_F5 = 0x74,
		K_F6 = 0x75,
		K_F7 = 0x76,
		K_F8 = 0x77,
		K_F9 = 0x78,
		K_F10 = 0x79,
		K_F11 = 0x7A,
		K_F12 = 0x7B,

		K_NUMLOCK = 0x90,
		K_SCROLLLOCK = 0x91,

		K_LSHIFT = 0xA0,
		K_RSHIFT = 0xA1,
		K_LCONTROL = 0xA2,
		K_RCONTROL = 0xA3
	};

	enum EventType // TODO: add all events (mouse, etc)
	{
		EV_QUIT,		// Used internally by the window see Window::isOpen()
		EV_KEYDOWN,
		EV_KEYUP,
		EV_MOUSEMOVED,
		EV_MBUTTONDOWN,
		EV_MBUTTONUP
	};

	struct MouseEvent
	{
		int xPos = 0;
		int yPos = 0;

		int xRel = 0; // The relative movement 
		int yRel = 0; // The relative movement
	};

	struct Event
	{
		EventType type;
		VirtualKeyCode vKey = NO_KEY;
		MouseEvent mouse;
	};

	class Window
	{
	private:
		MouseEvent m_lastMouseEvent;

	protected:
		HWND m_hWnd;	 // Window handle
		WNDCLASSEX m_wc; // This struct holds window information

		LPCSTR m_wtitle;		// Window title : TITLE IS SUPER FUCKING BROKEN -> probably a pointer problem
		std::wstring m_wstitle; // Window title

		bool m_isOpen = false;

		std::queue<Event> m_eventQueue;

	public:
		Window(uint16 width, uint16 height, std::string title, uint32 style = WINDOW_VISIBLE, uint16 posx = 300, uint16 posy = 300);

		bool isOpen() { return m_isOpen; }

		// Returns true whilst there is still events to be processed
		bool pollEvents(Event& e);

		// Currently only supports ascii (no ��� :( a sad day for sweden)
		void setTitle(std::string title);

		HWND getWindowHandle() { return m_hWnd; }
	};
}