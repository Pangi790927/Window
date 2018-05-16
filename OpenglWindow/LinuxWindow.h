#ifndef OPENGLWINDOW_H_INCLUDED
#define OPENGLWINDOW_H_INCLUDED 

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
// #include <X11/extensions/Xrandr.h>

/// Opengl 
#include <GL/glx.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <cstring>

#include "ErrorLogs.h"

#include "Util.h"

#include "Mouse.h"
#include "Keyboard.h"
#include "MathLib.h"

class OpenGLWindow {
private: 
	Display *display; 
	Window rootWindow; 
	Window window;

	Colormap colormap; 
	XVisualInfo *visualInfo;
	XSetWindowAttributes windowAttributes;
	GLXContext glContext;

	XEvent event;
	XWindowAttributes eventWindowAttributes;
	Atom wm_delete_window;

public:
	Mouse mouse; 
	Keyboard keyboard; 

	int width = 600; 
	int height = 600; 

	Point2i windowPosition;

	std::string name; 

	bool windowClosed = false; 
	bool hasToClose = false;

	bool cursorHidden = false;

	int MSAA;

	OpenGLWindow (bool windowVisible, int width = 600, int height = 600, std::string name = "123", int MSAA = 8) 
			: window(window), height(height), name(name), MSAA(MSAA)
	{
		init(windowVisible);
	}

	void init (bool windowVisible) {
		openDispplay(); 
		getParentWindow();
		setWindowSettings();
		createColorMap();
		setWindowAttributes();
		creteWindow(); 
		changeName(name);
		createContext();
		setProtocols();

		if (windowVisible) 
			showWindow();
	}

	bool openDispplay (std::string name = "") {
		display = XOpenDisplay(name == "" ? NULL : name.c_str());

		if (display == NULL) {
			ERROR_PRINT("Can't connect to X server", 0, 0);
			return false; 
		}
		return true; 
	}

	void getParentWindow() {
		rootWindow = DefaultRootWindow(display);
	}

	bool setWindowSettings() {
		GLint atributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None }; // make more generic

		static const int Visual_attribs[] =
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
			GLX_SAMPLE_BUFFERS  , MSAA > 0,            // <-- MSAA
			GLX_SAMPLES         , MSAA,            // <-- MSAA 
			None					
		};

		int attribs [ 100 ] ;
		memcpy( attribs, Visual_attribs, sizeof( Visual_attribs ) );

		GLXFBConfig fbconfig = 0;
		int         fbcount;

		GLXFBConfig *fbc = glXChooseFBConfig( display, 0/*screen */, attribs, &fbcount );
		if ( fbc )
		{
			if ( fbcount >= 1 )
				fbconfig = fbc[0];
			XFree( fbc );
		}

		if ( !fbconfig ) {
			ERROR_PRINT("Failed to get MSAA GLXFBConfig", 0, 0); 
			return false; 
		}

		// --- Get its VisualInfo ---
		visualInfo = glXGetVisualFromFBConfig( display, fbconfig );

		if (visualInfo == NULL) {
			ERROR_PRINT("no visual chosen", 0, 0); 
			return false; 
		}
		return true;
	}

	void createColorMap() {
		colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
	}

	// https://tronche.com/gui/x/xlib/events/mask.html
	void setWindowAttributes() {
		windowAttributes.colormap = colormap; 
		windowAttributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | 
										ButtonReleaseMask | PointerMotionMask;
	}

	void setProtocols() { // ICCCM Client to Window Manager Comunication (X Button)
		wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
		Atom protocols[] = {wm_delete_window};
		XSetWMProtocols(display, window, protocols, 1);
	}

	// void makeFullscreen() {
	// 	Atom atoms[2] = { XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False), None };
	// 	XChangeProperty(
	// 		display, 
	// 		window, 
	// 		XInternAtom(display, "_NET_WM_STATE", False),
	// 		XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 1
	// 	);
	// }

	// void makeWindowed() {
	// 	Atom atoms[2] = { XInternAtom(display, "_NET_WM_STATE_WINDOWED", False), None };
	// 	XChangeProperty(
	// 		display, 
	// 		window, 
	// 		XInternAtom(display, "_NET_WM_STATE", False),
	// 		XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 1
	// 	);
	// }

	void creteWindow() {
		window = XCreateWindow(
			display, 
			rootWindow, 
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

	void showWindow() {
		XMapWindow(display, window);
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
		int x, y;
		Window child;
		XWindowAttributes xwa;
		XTranslateCoordinates( display, window, rootWindow, 0, 0, &x, &y, &child );
		XGetWindowAttributes( display, window, &xwa );
		
		windowPosition.x = x - xwa.x; 
		windowPosition.y = y - xwa.y; 
	}

	void moveMouseTo (int x, int y) {
		setWindowPosition();
		XSelectInput(display, rootWindow, KeyReleaseMask); 
		XWarpPointer(display, None, rootWindow, 0, 0, 0, 0, windowPosition.x + x, windowPosition.y + y); 
		XFlush(display);
	}

	void hideCursor() {
		if (!isCursorHidden()) {
			XColor dummy;
			const char data = 0;

			Pixmap blank = XCreateBitmapFromData (display, window, &data, 1, 1);
			if (blank == None) 
				ERROR_PRINT("Out of Memory", 0, 0);
			
			Cursor cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
			XFreePixmap (display, blank);

			cursorHidden = true;
			XDefineCursor(display, window, cursor);
		}
	}

	void showCursor() {
		if (isCursorHidden()) {
			cursorHidden = false; 
			XUndefineCursor(display, window);
		}
	}

	bool isCursorHidden() {
		return cursorHidden; 
	}

	template <class Type>
	void handleEventsAndDrawing(Type &drawObject) {
		bool needDrawing = false;
		while (!isWindowClosed() && XPending(display)) {
			XNextEvent(display, &event);

			if (event.type == Expose) {
				XGetWindowAttributes(display, window, &eventWindowAttributes);
				needDrawing = true; 
			}
			else if (Util::isEqualToAny(event.type, {KeyPress, KeyRelease})) {
				keyboard.handleEvent(event);
			}
			else if (Util::isEqualToAny(event.type, {MotionNotify, ButtonPress, ButtonRelease})) {
				mouse.handleEvent(event);
			}
			else if (ClientMessage && event.xclient.data.l[0] == wm_delete_window) {
				close();
			}
			else {
				/// --- nothing 
			}
		}

		if (!isWindowClosed()) {
			if (needDrawing) {
				width = eventWindowAttributes.width; 
				height = eventWindowAttributes.height;

				setWindowPosition();

				drawObject.onRescale(); 
			}
			
			drawObject.mainLoop();
			glXSwapBuffers(display, window);
		}

		if (hasToClose) {
			close();
		}
	}

	template <class Type>
	void startMainLoop(Type &drawObject) {
		while (!isWindowClosed())
			handleEventsAndDrawing(drawObject); 
	}

	void closeAnounce() {
		hasToClose = true; 
	}

	void close() {
		glXMakeCurrent(display, None, NULL);
        glXDestroyContext(display, glContext);
        XDestroyWindow(display, window);
        XCloseDisplay(display);

        windowClosed = true; 
	}

	bool isWindowClosed() {
		return windowClosed; 
	}
};

#endif