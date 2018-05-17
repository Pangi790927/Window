#include <iostream>
#include "LinuxWindow.h"

int main(int argc, char const *argv[])
{
	int width = 600;
	int height = 600;

	LinuxWindow parrent(width, height, "parrent");
	LinuxWindow window(width, height, "child", parrent.window);

	while (parrent.active) {
		static float x = 0;
		static float y = 0;

		if (window.handleInput())
			if (window.keyboard.getKeyState(window.keyboard.ESC))
				window.requestClose();
		
		if (parrent.handleInput()) {
			if (parrent.keyboard.getKeyState(parrent.keyboard.ESC))
				parrent.requestClose();
			x = -(1.0f - parrent.mouse.x / (float)parrent.width * 2);
			y = 1.0f - parrent.mouse.y / (float)parrent.height * 2;
		}
		
		window.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_LINES);
			glColor3f(0, 1, 0);
			glVertex2f(-1, -1);
			glVertex2f(x, y);
		glEnd();
		window.swapBuffers();
		
		parrent.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex2f(0, 0);
			glVertex2f(1, 1);
		glEnd();
		parrent.swapBuffers();
	}
	return 0;
}