#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

/*
	Options:
			- options are per window
		perspective -> t/f, sets perspective
		msaa -> MSAA
		closed -> t/f, sets if window is closed
		vSync -> t/f, sets if vSync is enabled
		width -> width of the window
		height -> height of the window

	WindowType Functions:
		requestClose();		// ask the window to close
		setVSync();			// sets vsinc using option
		resize();			// resizes viewPort
		focus();			// ready window for drawing
		swapBuffers();		// swap the drawing buffers
		handleInput();		// remembers input pressed,
							// returns true if event occured
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

	WindowCore (int width, int height, std::string name = "name",
			Options options = Options(),
			EventMemory *eventMemory = new EventMemory())
	: options(options),
		options["width"](width),
		options["height"](height),		
		eventMemory(eventMemory),
		RawWindow(options["width"], options["height"], name, options["msaa"])
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

		// iterate through windows
		for (auto&& windowDesc : windows) {
			auto &window = *windowDesc.second;
			if (window->hasEvent()) { 
				if (currentWindow->handleInput() == false)
					// will update next time
					currentWindow["closed"] = true;
				else
					currentWindow->event();
			}

			window->postDraw();
			window->draw();
		}
	}
};

#endif