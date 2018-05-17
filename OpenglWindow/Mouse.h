#ifndef MOUSE_H
#define MOUSE_H

class Mouse {
public:
	float x = 0;
	float y = 0;

	float lastX = 0;
	float lastY = 0;

	float mmbPos = 0;
	float lastMmbPos = 0;

	bool lmb = false;
	bool mmb = false;
	bool rmb = false;

	void updateXY (float x, float y) {
		lastX = this->x;
		lastY = this->y;

		this->x = x;
		this->y = y;
	}

	void updateMmbPos (float pos) {
		lastMmbPos = mmbPos;
		mmbPos = pos;
	}
};

#endif