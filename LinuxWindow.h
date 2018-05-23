#ifndef LINUX_WINDOW_H
#define LINUX_WINDOW_H

#include "Keyboard.h"
#include "Mouse.h"

#include <cstring>
#include <sstream>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

// 2 x TO_DO
class LinuxWindow {
public:
	Display *display;
	Window parrentWindow; 
	Window window;

	Colormap colormap; 
	XVisualInfo *visualInfo;
	XSetWindowAttributes windowAttributes;
	GLXContext glContext;
	Atom wm_delete_window;

	Mouse mouse;
	Keyboard<> keyboard;

	int width;
	int height;
	
	int x = 0;
	int y = 0;

	std::string name;

	bool active = false;
	bool closePending = false;
	bool cursorHidden = false;
	bool focusIn;

	int msaa;

	LinuxWindow (int width, int height,
			std::string name = "name", int msaa = 8, Window parrent = 0)
	: width(width), height(height), name(name), msaa(msaa)
	{
		connectDisplay();
		setParrent(parrent);
		setVisuals();
		createColorMap();
		createWindow();
		changeName(name);
		createContext();
		setCloseProtocol();
		initKeyboard();

		mapWindow();
		active = true;
	}

	LinuxWindow (const LinuxWindow& other) = delete;
	LinuxWindow (const LinuxWindow&& other) = delete;
	LinuxWindow& operator = (const LinuxWindow& other) = delete;
	LinuxWindow& operator = (const LinuxWindow&& other) = delete;

	void connectDisplay() {
		if ((display = XOpenDisplay(NULL)) == NULL)
			throw std::runtime_error("Can't connect to X server");
	}

	void setParrent (Window parrent) {
		if (parrent != 0) {
			parrentWindow = parrent;
		}
		else {
			parrentWindow = DefaultRootWindow(display);
		}
	}

	void setVisuals() {
		// should be checked, I couldn't find the right manual for glx
		static const int visualAttr[] =
		{
			GLX_X_RENDERABLE    , True,
			GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
			GLX_RENDER_TYPE     , GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
			GLX_RED_SIZE        , 8,
			GLX_GREEN_SIZE      , 8,
			GLX_BLUE_SIZE       , 8,
			GLX_ALPHA_SIZE      , 8,
			GLX_DEPTH_SIZE      , 24,
			GLX_STENCIL_SIZE    , 8,
			GLX_DOUBLEBUFFER    , True,
			GLX_SAMPLE_BUFFERS  , msaa > 0,			// <-- MSAA
			GLX_SAMPLES         , msaa,				// <-- MSAA 
			None					
		};

		int attribs[100];
		memcpy(attribs, visualAttr, sizeof(visualAttr));

		GLXFBConfig fbconfig = 0;
		int fbcount;

		GLXFBConfig *fbc = glXChooseFBConfig(display,
				0/*screen */, attribs, &fbcount);

		if (fbc) {
			if (fbcount >= 1)
				fbconfig = fbc[0];
			XFree(fbc);
		}

		if (!fbconfig)
			throw std::runtime_error("Failed to get MSAA GLXFBConfig");

		if ((visualInfo = glXGetVisualFromFBConfig(display, fbconfig)) == NULL)
			throw std::runtime_error("No visual chosen");
	}

	void createColorMap() {
		colormap = XCreateColormap(display, parrentWindow,
				visualInfo->visual, AllocNone);
	}

	void setCloseProtocol() {
		wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
		Atom protocols[] = {wm_delete_window};
		XSetWMProtocols(display, window, protocols, 1);
	}

	void createWindow() {
		windowAttributes.colormap = colormap; 
		windowAttributes.event_mask =
				ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
				ButtonReleaseMask | PointerMotionMask | FocusChangeMask;

		window = XCreateWindow(
			display,
			parrentWindow,
			0,
			0,
			width,
			height,
			0,
			visualInfo->depth,
			InputOutput,
			visualInfo->visual,
			CWColormap | CWEventMask,
			&windowAttributes
		);
	}

	void changeName (std::string name) {
		this->name = name; 
		XStoreName(display, window, name.c_str());
	}

	void createContext() {
		glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
		glXMakeCurrent(display, window, glContext);
	}

	void setWindowPosition() {
		int oldX, oldY;
		Window child;
		XWindowAttributes xwa;
		XTranslateCoordinates(display, window, parrentWindow,
				0, 0, &x, &y, &child);
		XGetWindowAttributes(display, window, &xwa);
		
		x = oldX - xwa.x;
		y = oldY - xwa.y;
	}

	void mapWindow() {
		XMapWindow(display, window);
	}

	void moveMouseTo (int dx, int dy) {
		setWindowPosition();
		XSelectInput(display, parrentWindow, KeyReleaseMask); 
		XWarpPointer(display, None, parrentWindow, 0, 0, 0, 0, x + dx, y + dy);
		XFlush(display);
	}

	void hideCursor() {
		if (!cursorHidden) {
			XColor dummy;
			const char data = 0;

			Pixmap blank = XCreateBitmapFromData (display, window, &data, 1, 1);
			if (blank == None)
				std::cout << "out of memory" << std::endl;
			
			Cursor cursor = XCreatePixmapCursor(display, blank, blank,
					&dummy, &dummy, 0, 0);
			XFreePixmap (display, blank);

			cursorHidden = true;
			XDefineCursor(display, window, cursor);
		}
	}

	void showCursor() {
		if (cursorHidden) {
			cursorHidden = false; 
			XUndefineCursor(display, window);
		}
	}

	void close() {
		if (!active)
			return;
		glXMakeCurrent(display, None, NULL);
		glXDestroyContext(display, glContext);
		XDestroyWindow(display, window);
		XCloseDisplay(display);

		active = false; 
	}

	void updateKeyboard (const XEvent& event) {
		KeySym code = XkbKeycodeToKeysym(display, event.xkey.keycode, 0,
				event.xkey.state & ShiftMask ? 1 : 0);
		keyboard.registerEvent(code, event.type == KeyPress);
	}

	void updateMouse (const XEvent& event) {
		if (event.type == ButtonPress) {
			if (event.xbutton.button == Button1)
				mouse.updateLmb(true);
			if (event.xbutton.button == Button2)
				mouse.updateMmb(true);
			if (event.xbutton.button == Button3)
				mouse.updateRmb(true);
		}
		else if (event.type == ButtonRelease) {
			if (event.xbutton.button == Button1)
				mouse.updateLmb(false);
			if (event.xbutton.button == Button2)
				mouse.updateMmb(false);
			if (event.xbutton.button == Button3)
				mouse.updateRmb(false);
		}
		else if (event.type == MotionNotify) {
			mouse.updateXY(event.xmotion.x, event.xmotion.y);
		}
	}

	void focus() {
		if (!active)
			return;
		glXMakeCurrent(display, window, glContext);
	}

	void resize() {
		if (!active)
			return;
		// TO DO
	}

	void setVSync (bool sync) {
		if (!active)
			return;
		// TO DO; nothing, really 
	}

	void swapBuffers() {
		if (!active)
			return;
		glXSwapBuffers(display, window);
	}

	void requestClose() {
		closePending = true;
	}

	bool handleInput() {
		bool needRedraw = false;
		bool hadEvent = false;
		XWindowAttributes eventWindowAttributes;
		XEvent event;

		if (!active)
			return false;
		while (active && XPending(display)) {
			XNextEvent(display, &event);

			hadEvent = true;
			if (event.type == Expose) {
				XGetWindowAttributes (display, window, &eventWindowAttributes);
				needRedraw = true; 
			}
			else if (Util::isEqualToAny(event.type, {KeyPress, KeyRelease})) {
				updateKeyboard(event);
			}
			else if (Util::isEqualToAny(event.type,
					{MotionNotify, ButtonPress, ButtonRelease}))
			{
				updateMouse(event);
			}
			else if (Util::isEqualToAny(event.type, {FocusIn, FocusOut})) {
				if (event.type == FocusIn)
					focusIn = true;
				if (event.type == FocusOut)
					focusIn = false;
			}
			else if (ClientMessage && event.xclient.data.l[0] == wm_delete_window) {
				closePending = true;
			}
		}

		if (active) {
			if (needRedraw) {
				width = eventWindowAttributes.width; 
				height = eventWindowAttributes.height;

				setWindowPosition();
				resize();
			}
		}

		if (closePending) {
			close();
		}

		return hadEvent;
	}

	void initKeyboard() {
		std::map<std::string, int> mapping;

		mapping["ENTER"] = XK_Return;
		mapping["SPACE"] = XK_space;
		mapping["CAPS_LOCK"] = XK_Caps_Lock;
		mapping["TAB"] = XK_Tab;
		mapping["L_ALT"] = XK_Alt_L;
		mapping["R_ALT"] = XK_Alt_R;
		mapping["L_CTRL"] = XK_Control_L;
		mapping["R_CTRL"] = XK_Control_R;
		mapping["L_SHIFT"] = XK_Shift_L;
		mapping["R_SHIFT"] = XK_Shift_R;
		mapping["ARROW_UP"] = XK_Up;
		mapping["ARROW_DOWN"] = XK_Down;
		mapping["ARROW_LEFT"] = XK_Left;
		mapping["ARROW_RIGHT"] = XK_Right;
		mapping["F1"] = XK_F1;
		mapping["F2"] = XK_F2;
		mapping["F3"] = XK_F3;
		mapping["F4"] = XK_F4;
		mapping["F5"] = XK_F5;
		mapping["F6"] = XK_F6;
		mapping["F7"] = XK_F7;
		mapping["F8"] = XK_F8;
		mapping["F9"] = XK_F9;
		mapping["F10"] = XK_F10;
		mapping["F11"] = XK_F11;
		mapping["F12"] = XK_F12;
		mapping["BACKSPACE"] = XK_BackSpace;
		mapping["INSERT"] = XK_Insert;
		mapping["DELETE"] = XK_Delete;
		mapping["HOME"] = XK_Home;
		mapping["PAGE_UP"] = XK_Page_Up;
		mapping["PAGE_DOWN"] = XK_Page_Down;
		mapping["END"] = XK_End;
		mapping["PRINT_SCREEN"] = XK_Print;
		mapping["SCREEN_LOCK"] = XK_Scroll_Lock;
		mapping["PAUSE"] = XK_Pause;
		mapping["NUM_LOCK"] = XK_Num_Lock;
		mapping["NUM_ENTER"] = XK_KP_Enter;
		mapping["NUM_INSERT"] = XK_KP_Insert;
		mapping["WINKEY"] = XK_Super_L;
		mapping["FN"] = 0;
		mapping["ESC"] = XK_Escape;

		keyboard.mapKeys(mapping);
	}

	std::string toString() {
		std::stringstream ss;
		ss << "name: " << name << std::endl;
		ss << "coord[w;h;x;y]: " << width << " " << height << " "
				<< x << " " << y << std::endl;
		ss << "display: " << display << std::endl;
		ss << "glContext: " << glContext << std::endl;
		ss << "window: " << window << std::endl;
		ss << "parrent: " << parrentWindow << std::endl;
		ss << "status[active; close; cursor; focus]: " << active << " "
				<< closePending << " " << cursorHidden << " "
				<< focusIn << std::endl;
		return ss.str();
	}

	~LinuxWindow() {
		close();
	}
};

#endif