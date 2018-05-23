#include <iostream>
#include "OpenglWindow.h"

int main(int argc, char const *argv[])
{
	int width = 600;
	int height = 300;

	OpenglWindow parrent(width, height, "parrent");

	std::cout << parrent.toString() << std::endl;
	std::cout << child.toString() << std::endl;

	while (parrent.active || child.active) {
		static float x = 0;
		static float y = 0;
		
		if (parrent.handleInput()) {
			if (parrent.keyboard.getKeyState(parrent.keyboard.ESC))
				parrent.requestClose();
			while (!parrent.keyboard.queEmpty())
				std::cout << parrent.keyboard.getName(parrent.keyboard.popEvent().key);
			x = -(1.0f - parrent.mouse.x / (float)parrent.width * 2);
			y = 1.0f - parrent.mouse.y / (float)parrent.height * 2;
		}

		if (child.handleInput())
			if (child.keyboard.getKeyState(child.keyboard.ESC))
				child.requestClose();

		child.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_LINES);
			glColor3f(0, 1, 0);
			glVertex2f(-1, -1);
			glVertex2f(x, y);
		glEnd();
		child.swapBuffers();
		
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