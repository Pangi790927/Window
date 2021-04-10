#ifndef LINUX_WINDOW_H
#define LINUX_WINDOW_H

#include "Keyboard.h"
#include "Mouse.h"
#include <cstring>
#include <sstream>
#include <functional>
#include <GL/glew.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension)
{
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	/* It takes a bit of care to be fool-proof about parsing the
	   OpenGL extensions string. Don't be fooled by sub-strings,
	   etc. */
	for (start=extList;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if ( where == start || *(where - 1) == ' ' )
			if ( *terminator == ' ' || *terminator == '\0' )
				return true;

		start = terminator;
	}

	return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
{
	ctxErrorOccurred = true;
	return 0;
}

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
	bool debug;

	int msaa;
	std::function<void(int, int, int, int)> onResize = [&](int x, int y, int w, int h) {
		focus();
		glViewport(x, y, w, h);
	};

	LinuxWindow (int width, int height,
			std::string name = "name", int msaa = 8, Window parrent = 0,
			bool debug = true)
	: width(width), height(height), name(name), msaa(msaa), debug(debug)
	{
		if ((display = XOpenDisplay(NULL)) == NULL)
			throw std::runtime_error("Can't connect to X server");

		parrentWindow = parrent ? parrent : DefaultRootWindow(display);

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

		 // FBConfigs were added in GLX version 1.3.
  		int glx_major, glx_minor;
		if (!glXQueryVersion(display, &glx_major, &glx_minor) ||
				((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
			throw std::runtime_error("Invalid GLX version");

		int attribs[100];
		memcpy(attribs, visualAttr, sizeof(visualAttr));


		int fbcount;
		GLXFBConfig fbconfig = 0;
		GLXFBConfig *fbc = glXChooseFBConfig(display,
				DefaultScreen(display), attribs, &fbcount);

		/* TO DO: Find out what those are and how to use them better */
		if (fbc) {
			if (fbcount >= 1)
				fbconfig = fbc[0];
			XFree(fbc);
		}

		if (!fbconfig)
			throw std::runtime_error("Failed to get MSAA GLXFBConfig");

		if ((visualInfo = glXGetVisualFromFBConfig(display, fbconfig)) == NULL)
			throw std::runtime_error("No visual chosen");
		
		colormap = XCreateColormap(display, parrentWindow,
				visualInfo->visual, AllocNone);
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

		if (!window)
			throw std::runtime_error("Failed to create window.\n");

		changeName(name);
		XMapWindow(display, window);

		const char *glxExts = glXQueryExtensionsString(display,
				DefaultScreen(display));

		using glXCreateContextAttribsARBProc = GLXContext (*)(Display*,
				GLXFBConfig, GLXContext, Bool, const int*);
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
		glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
				glXGetProcAddressARB(
				(const GLubyte *)"glXCreateContextAttribsARB");

		// TO DO
		// Install an X error handler so the application won't exit if GL 3.0
		// context allocation fails.
		//
		// Note this error handler is global.  All display connections in all threads
		// of a process use the same error handler, so be sure to guard against other
		// threads issuing X commands while this code is running.
		ctxErrorOccurred = false;
		int (*oldHandler)(Display*, XErrorEvent*) =
				XSetErrorHandler(&ctxErrorHandler);

		// Check for the GLX_ARB_create_context extension string and the function.
		// If either is not present, use GLX 1.3 context creation method.
		if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") ||
				!glXCreateContextAttribsARB)
		{
			printf("glXCreateContextAttribsARB() not found"
					" ... using old-style GLX context\n");
			glContext = glXCreateNewContext(display, fbconfig,
					GLX_RGBA_TYPE, 0, true);
		}
		else {
			int context_attribs[] =
			{
				GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
				GLX_CONTEXT_MINOR_VERSION_ARB, 0,
				debug ? GLX_CONTEXT_FLAGS_ARB : 0, debug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
				//GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
				None
			};

			glContext = glXCreateContextAttribsARB(display, fbconfig, 0,
					true, context_attribs);

			// Sync to ensure any errors generated are processed.
			XSync(display, false);
			if (!(!ctxErrorOccurred && glContext))
			{
				// Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
				// When a context version below 3.0 is requested, implementations will
				// return the newest context version compatible with OpenGL versions less
				// than version 3.0.
				// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
				context_attribs[1] = 1;
				// GLX_CONTEXT_MINOR_VERSION_ARB = 0
				context_attribs[3] = 0;

				ctxErrorOccurred = false;

				printf( "Failed to create GL 3.0 context"
						" ... using old-style GLX context\n");
				glContext = glXCreateContextAttribsARB(display, fbconfig, 0, 
						true, context_attribs);
			}
		}

		// Sync to ensure any errors generated are processed.
		XSync( display, False );

		// Restore the original error handler
		XSetErrorHandler( oldHandler );

		if (ctxErrorOccurred || !glContext )
			throw std::runtime_error("Failed to create an OpenGL context\n");

		glXMakeCurrent(display, window, glContext);

		wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
		Atom protocols[] = {wm_delete_window};
		XSetWMProtocols(display, window, protocols, 1);
		
		initKeyboard();

		active = true;
	}

	LinuxWindow (const LinuxWindow& other) = delete;
	LinuxWindow (const LinuxWindow&& other) = delete;
	LinuxWindow& operator = (const LinuxWindow& other) = delete;
	LinuxWindow& operator = (const LinuxWindow&& other) = delete;

	void changeName (std::string name) {
		this->name = name; 
		XStoreName(display, window, name.c_str());
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
				throw std::runtime_error("out of memory");
			
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

	template <typename FuncType>
	void setResize (FuncType&& func) {
		onResize = func;
	}

	void resize() {
		if (!active)
			return;
		onResize(0, 0, width, height);
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