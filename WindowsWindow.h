#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H

#include "Keyboard.h"
#include "Mouse.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <functional>
#include <windows.h>
#include <windowsx.h>

class WindowsWindow {
public:
	WNDCLASSEX wcex;
	HINSTANCE hInstance;
	HWND window;
	HWND parrent;
	HDC  hDC;
	HGLRC hRC;
	DWORD dwExStyle;
	DWORD dwStyle;

	Mouse mouse;
	Keyboard<> keyboard;

	int width;
	int height;

	int x;
	int y;

	std::string name;

	bool active = false;
	bool closePending = false;
	bool cursorHidden = false;
	bool focusIn;
	bool needRedraw = false;

	int msaa;	// not used inside the windows window
	std::function<void(int, int, int, int)> onResize = [&](int x, int y, int w, int h) {
		focus();
		glViewport(x, y, w, h);
	};

	WindowsWindow (int width, int height, std::string name,
			int msaa = 8, HWND parrent = 0)
	: width(width), height(height), name(name), msaa(msaa), parrent(parrent)
	{
		getHInstance();
		registerClass();
		createWindow(parrent);
		registerWindowEvent();
		showWindow();
		initOpengl();
		initKeyboard();

		active = true;
	}

	WindowsWindow (const WindowsWindow& other) = delete;
	WindowsWindow (const WindowsWindow&& other) = delete;
	WindowsWindow& operator = (const WindowsWindow& other) = delete;
	WindowsWindow& operator = (const WindowsWindow&& other) = delete;

	static std::map<HWND, WindowsWindow *> eventMap;
	static LRESULT CALLBACK globalEventProc (HWND hwnd, UINT uMsg,
			WPARAM wParam, LPARAM lParam)
	{
		if (eventMap.find(hwnd) != eventMap.end()) {
			WindowsWindow *ptr = (*eventMap.find(hwnd)).second;

			if (ptr) {
				return ptr->eventProc(hwnd, uMsg, wParam, lParam);
			}
			else
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		else
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK eventProc (HWND hwnd, UINT uMsg,
			WPARAM wParam, LPARAM lParam)
	{
		uint8_t kbs[256];
		WORD code[2];
		switch (uMsg) {
			case WM_CLOSE: closePending = true; break;
			case WM_DESTROY: closePending = true; break;
			
			case WM_KEYDOWN:
					GetKeyboardState(kbs);
					if (ToAscii(wParam, MapVirtualKey(wParam,
							MAPVK_VK_TO_VSC), kbs, code, 0) <= 0)
						keyboard.registerEvent(wParam + 256, true);
					else
						keyboard.registerEvent(code[0] +
								(256 + wParam) * bool(!code[0]), true);
				break;
			case WM_KEYUP:
					GetKeyboardState(kbs);
					if (ToAscii(wParam, MapVirtualKey(wParam,
							MAPVK_VK_TO_VSC), kbs, code, 0) <= 0)
						keyboard.registerEvent(wParam + 256, false);
					else
						keyboard.registerEvent(code[0] +
								(256 + wParam) * bool(!code[0]), false);
				break;

			case WM_MOUSEMOVE: mouse.updateXY(GET_X_LPARAM(lParam),
					GET_Y_LPARAM(lParam)); break;
			case WM_MOUSEWHEEL: mouse.updateMmbPos(
					GET_WHEEL_DELTA_WPARAM(wParam)); break;
			
			case WM_LBUTTONDOWN: mouse.updateLmb(true); break;
            case WM_MBUTTONDOWN: mouse.updateMmb(true); break;
            case WM_RBUTTONDOWN: mouse.updateRmb(true); break;
            case WM_LBUTTONUP: mouse.updateLmb(false); break;
            case WM_MBUTTONUP: mouse.updateMmb(false); break;
            case WM_RBUTTONUP: mouse.updateRmb(false); break;

            case WM_SIZE: needRedraw = true; break;

            default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	void getHInstance() {
		hInstance = GetModuleHandle(NULL);
	}

	void registerClass () {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = &WindowsWindow::globalEventProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = name.c_str();
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex))
			throw std::runtime_error("Couldn't register window class");
	}

	void createWindow (HWND parrent = 0) {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW |	WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		
		window = CreateWindowEx(
			dwExStyle,
			name.c_str(),
			name.c_str(),
			dwStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			parrent,
			NULL,
			hInstance,
			NULL
		);

		if (!window)
			throw std::runtime_error("Couldn't create window");
	}

	void registerWindowEvent() {
		eventMap[window] = this;	
	}

	void initOpengl (uint8_t colorDepth = 32, uint8_t depthBuffer = 32,
			uint8_t stencilBuffer = 32)
	{
		PIXELFORMATDESCRIPTOR pfd;
		GLuint PixelFormat;
		DWORD flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

		pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			flags,
			PFD_TYPE_RGBA,
			colorDepth,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			depthBuffer,
			stencilBuffer,
			0,
			PFD_OVERLAY_PLANE,
			0,
			0, 0, 0
		};

		if (!(hDC = GetDC(window)))
			throw std::runtime_error("Can't create a GL device context!");

		if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
			throw std::runtime_error("Can't find a suitable pixel format!");

		if (!SetPixelFormat(hDC, PixelFormat, &pfd))
			throw std::runtime_error("Can't set the pixel format!");

		if (!(hRC = wglCreateContext(hDC)))
			throw std::runtime_error("Can't create a GL rendering context!");

		if (!wglMakeCurrent(hDC, hRC))
			throw std::runtime_error("Can't acivate the rendering context!");

		resize();
	}

	void showWindow (int nCmdShow = SW_SHOWDEFAULT) {
		ShowWindow(window, nCmdShow);
	}

	template <typename FuncType>
	void setRezise (FuncType&& func) {
		onResize = func;
	}

	void resize() {
		if (!active)
			return;
		if (closePending) {
			close();
			return;
		}
		onResize(0, 0, width, height);
	}

	void focus() {
		if (!active)
			return;
		if (closePending) {
			close();
			return;
		}
		if (!wglMakeCurrent(hDC, hRC))
			throw std::runtime_error("Can't focus rendering context!");
	}

	void setVSync (bool sync) {
		if (!active)
			return;
		typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
		PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
		const char *extensions = (char*)glGetString(GL_EXTENSIONS);

		if (strstr(extensions, "WGL_EXT_swap_control") == 0) {
			return;
		}
		else {
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

			if(wglSwapIntervalEXT)
				wglSwapIntervalEXT(sync);
		}
	}

	void swapBuffers() {
		if (!active)
			return;
		if (closePending) {
			close();
			return;
		}
		SwapBuffers(hDC);
	}

	void requestClose() {
		closePending = true;
	}

	bool handleInput() {
		MSG msg;
		bool hadEvent = false;

		if (!active)
			return false;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			hadEvent = true;
		}

		if (needRedraw) {
			RECT rect;

			if (GetClientRect(window, &rect)) {
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				resize();
			}
		}

		if (closePending) {
			close();
		}

		return hadEvent; 
	}

	void unregisterWindowEvent() {
		eventMap.erase(window);
	}

	void close() {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(window, hDC);
		DestroyWindow(window);
		unregisterWindowEvent();

		active = false;
	}

	void initKeyboard() {
		std::map<std::string, int> mapping;

		mapping["ENTER"] = VK_RETURN;
		mapping["SPACE"] = VK_SPACE;
		mapping["CAPS_LOCK"] = VK_CAPITAL + 256;
		mapping["TAB"] = VK_TAB;
		// mapping["L_ALT"] = VK_MENU + 256; // not working?
		// mapping["R_ALT"] = VK_MENU + 256; // not working?
		mapping["L_CTRL"] = VK_CONTROL + 256;
		mapping["R_CTRL"] = VK_CONTROL + 256;
		mapping["L_SHIFT"] = VK_SHIFT + 256;
		mapping["R_SHIFT"] = VK_SHIFT + 256;
		mapping["ARROW_UP"] = VK_UP + 256;
		mapping["ARROW_DOWN"] = VK_DOWN + 256;
		mapping["ARROW_LEFT"] = VK_LEFT + 256;
		mapping["ARROW_RIGHT"] = VK_RIGHT + 256;
		mapping["F1"] = VK_F1 + 256;
		mapping["F2"] = VK_F2 + 256;
		mapping["F3"] = VK_F3 + 256;
		mapping["F4"] = VK_F4 + 256;
		mapping["F5"] = VK_F5 + 256;
		mapping["F6"] = VK_F6 + 256;
		mapping["F7"] = VK_F7 + 256;
		mapping["F8"] = VK_F8 + 256;
		mapping["F9"] = VK_F9 + 256;
		mapping["F10"] = VK_F10 + 256;
		mapping["F11"] = VK_F11 + 256;
		mapping["F12"] = VK_F12 + 256;
		mapping["BACKSPACE"] = VK_BACK;
		mapping["INSERT"] = VK_INSERT + 256;
		mapping["DELETE"] = VK_DELETE;
		mapping["HOME"] = VK_HOME + 256;
		mapping["PAGE_UP"] = VK_PRIOR + 256;
		mapping["PAGE_DOWN"] = VK_NEXT + 256;
		mapping["END"] = VK_END + 256;
		mapping["PRINT_SCREEN"] = VK_SNAPSHOT + 256;
		mapping["SCREEN_LOCK"] = VK_SCROLL + 256;
		mapping["PAUSE"] = VK_PAUSE + 256;
		mapping["NUM_LOCK"] = VK_NUMLOCK + 256;
		mapping["NUM_ENTER"] = VK_RETURN;
		mapping["NUM_INSERT"] = VK_INSERT;
		mapping["WINKEY"] = VK_LWIN + 256;
		mapping["FN"] = 0;
		mapping["ESC"] = VK_ESCAPE;

		keyboard.mapKeys(mapping);
	}

	std::string toString() {
		std::stringstream ss;
		ss << "name: " << name << std::endl;
		ss << "coord[w;h;x;y]: " << width << " " << height << " "
				<< x << " " << y << std::endl;
		ss << "hDC: " << hDC << std::endl;
		ss << "hRC: " << hRC << std::endl;
		ss << "window: " << window << std::endl;
		ss << "parrent: " << parrent << std::endl;
		ss << "status[active; close; cursor; focus]: " << active << " "
				<< closePending << " " << cursorHidden << " "
				<< focusIn << std::endl;
		return ss.str();
	}

	~WindowsWindow() {
		close();
	}
};

#endif