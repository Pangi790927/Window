#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H


class WindowsWindow {
public:
	WNDCLASSEX wcex;
	HINSTANCE hInstance;
	HWND window;
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

	WindowsWindow (int width, int height, std::string name,
			int msaa = 8, HWND parrent)
	: width(width), height(height), name(name), msaa(msaa)
	{
		getHInstance();
		registerClass();
		createWindow(parrent);
		registerWindowEvent();
		showWindow();
		initOpengl();
	}

	WindowsWindow (const WindowsWindow& other) = delete;
	WindowsWindow (const WindowsWindow&& other) = delete;
	WindowsWindow& operator = (const WindowsWindow& other) = delete;
	WindowsWindow& operator = (const WindowsWindow&& other) = delete;

	static std::map<HWND, WindowsWindow> eventMap;
	static LRESULT CALLBACK globalEventProc (HWND hwnd, UINT uMsg,
			WPARAM wParam, LPARAM lParam)
	{
		if (eventMap.find(hwnd) != eventMap.end()) {
			WindowsWindow *ptr = (*eventMap.find(hwnd)).second;

			if (ptr) {
				return ptr->eventProc(hwnd, uMsg, wParam, lParam);
			}
			else
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		else
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK eventProc (HWND hwnd, UINT uMsg,
			WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
			case WM_CLOSE: closePending = true; break;
			case WM_DESTROY: return 0; break;
			
			case WM_KEYDOWN: keyboard.registerEvent(wParam, false); break;
			case WM_KEYUP: keyboard.registerEvent(wParam, true); break;
			
			case WM_MOUSEMOVE: mouse.updateXY(GET_X_LPARAM(lParam),
					GET_Y_LPARAM(lParam)); break;
			case WM_MOUSEWHEEL: mouse.updateMmbPos(
					GET_WHEEL_DELTA_WPARAM(wParam)); break;
			
			case WM_LBUTTONDOWN: Mouse.lmb = true; break;
            case WM_MBUTTONDOWN: Mouse.mmb = true; break;
            case WM_RBUTTONDOWN: Mouse.rmb = true; break;
            case WM_LBUTTONUP: Mouse.lmb = false; break;
            case WM_MBUTTONUP: Mouse.mmb = false; break;
            case WM_RBUTTONUP: Mouse.rmb = false; break;

            case WM_SIZE: needRedraw = true; break;

            default: return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	void getHInstance() {
		hInstance = GetModuleHandle(NULL);
	}

	void registerClass () {
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = &WindowsWindow::globalEventProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = name.c_str();
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

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
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width,
			height,
			NULL,
			parrent,
			hInstance,
			NULL
		);

		if (!window)
			throw std::runtime_error("Couldn't create window");
	}

	void registerWindowEvent() {
		eventMap[window] = this;	
	}

	void initOpengl (int colorDepth = 32, int depthBuffer = 32,
			int stencilBuffer = 32)
	{
		PIXELFORMATDESCRIPTOR pfd;
		GLuint PixelFormat;
		int flags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

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

	void resize() {
		if (!active)
			return;
		// TO DO;
	}

	void focus() {
		if (!active)
			return;
		if (!wglMakeCurrent(hDC, hRC))
			throw std::runtime_error("Can't acivate the rendering context!");
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
		// TO DO;
	}

	void requestClose() {
		closePending = true;
	}

	bool handleInput() {
		MSG msg;
		bool hadEvent = false;

		if (!active)
			return false;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			hadEvent = true;
		}

		if (needRedraw) {
			RECT rect;

			if( GetClientRect(window, &rect)) {
				Width = rect.right - rect.left;
				Height = rect.bottom - rect.top;
				resize();
			}
		}

		if (closePending)
			close();

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
	}

	~WindowsWindow() {
		close();
	}
};

std::map<HWND, WindowsWindow> WindowsWindow::eventMap;

#endif