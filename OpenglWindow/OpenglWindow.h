#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

/*
	Options:
			- options are per window
		perspective -> t/f, sets perspective
		closed -> t/f, sets if window is closed
		vSync -> t/f, sets if vSync is enabled
		width -> width of the window
		height -> height of the window

	WindowType Functions:
		setVSync();			// sets vsinc using option
		reSizeGLScene();	// resizes viewPort
		focus();			// ready window for drawing
		unfocus();			// remove window for drawing
		swapBuffers();		// swap the drawing buffers
		handleInput();		// remembers input pressed
		getFocusedWindow();	// static function that tels
							// us which window is in focus
							// returns value of type NameType
*/

#include "Options.h"
#if defined(__linux__)
	#include "LinuxWindow.h"
#elif defined(_WIN32)
	#include "WindowsWindow.h"
#endif

template <typename RawWindow>
class WindowCore : public RawWindow {
public:
	EventMemory *eventMemory;
	Options options;

	WindowCore (int width, int height, Options options = Options(),
			EventMemory *eventMemory = new EventMemory())
	: options(options),
		options["width"](width),
		options["height"](height),		
		eventMemory(eventMemory),
		RawWindow(options["perspective"], options[width], options["height"],
			eventMemory)
	{
		options["closed"] = false;
		setVSync(options["vSync"]);
		glewInit();
		reSizeGLScene();
		postOpenGLinit();
	}

	void draw() {
		focus();
		core->draw();
		swapBuffers();
		unfocus();
	}

	int& operator [] (std::string name) {
		return options[name];
	}

	virtual void drawFunc() {

	}

	virtual void event() {
		
	}

	virtual void postDraw() {
		
	}

	virtual void destroy() {

	}

	virtual void postOpenGLinit() {

	}
};

template <typename NameType, typename RawWindow>
class WindowManager {
public:
	std::map<NameType, WidnowCore *> windows;

	template <typename WindowType>
	void addWindow (WindowType *window) {
		if (!window)
			throw std::runtime_error("invalid window registered");
		windows.insert(std::make_pair(name, window));
	}

	bool hasWindow (NameType name) {
		return windows.find(name) != windows.end();
	}

	template <typename WindowType>
	WindowType& accessWindow (NameType name) {
		return *dynamic_cast<WindowType *>(windows[name].get());
	}

	void loop() {
		// remove closed windows
		windows.erase(
			std::remove_if(
				windows.begin(),
				windows.end(),
				[](auto& pair) -> bool {
					return pair.second["closed"] == true;
				}
			),
			windows.end();
		);

		// handle input for in-focus window
		WindowCore *currentPtr = windows[RawWindow::getFocusedWindow()];
		if (currentPtr) {
			auto& currentWindow = *currentPtr;
			if (currentWindow->handleInput() == false) {
				currentWindow["closed"] = true;
			}
			else {
				currentWindow->event();
			}
		}

		// iterate through windows
		for (auto&& windowDesc : windows) {
			auto &window = *windowDesc.second;
			window->postDraw();
			window->draw();
		}
	}
};

#endif