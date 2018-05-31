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
	bool onceLmb = false;

	bool mmb = false;
	bool onceMmb = false;
	
	bool rmb = false;
	bool onceRmb = false;
	
	void updateXY (float x, float y) {
		lastX = this->x;
		lastY = this->y;

		this->x = x;
		this->y = y;
	}

	void update() {
		lastX = this->x;
		lastY = this->y;
	}

	void updateMmbPos (float pos) {
		lastMmbPos = mmbPos;
		mmbPos = pos;
	}

	void updateLmb (bool btn) {
		if (!btn)
			onceLmb = false;
		lmb = btn;
	}

	void updateMmb (bool btn) {
		if (!btn)
			onceMmb = false;
		mmb = btn;
	}

	void updateRmb (bool btn) {
		if (!btn)
			onceRmb = false;
		rmb = btn;
	}

	bool getLmb() {
		return lmb;
	}

	bool getMmb() {
		return mmb;
	}

	bool getRmb() {
		return rmb;
	}

	bool getOnceLmb() {
		if (lmb && !onceLmb) {
			onceLmb = true;
			return true;
		}
		else {
			if (!lmb)
				onceLmb = false;
			return false;
		}
	}

	bool getOnceMmb() {
		if (mmb && !onceMmb) {
			onceMmb = true;
			return true;
		}
		else {
			if (!mmb)
				onceMmb = false;
			return false;
		}
	}

	bool getOnceRmb() {
		if (rmb && !onceRmb) {
			onceRmb = true;
			return true;
		}
		else {
			if (!rmb)
				onceRmb = false;
			return false;
		}
	}
};

#endif