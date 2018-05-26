#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <map>
#include "Util.h"

struct KeyEvent {
	int key;
	int press;

	KeyEvent (int key = -1, int press = 0)
	: key(key), press(press) {}
};

template <int QUE_SIZE = 256>
class Keyboard : public Util::StaticQueue <KeyEvent, QUE_SIZE> {
public:
	const static int MAX_KEY_CODE = 65536;

	int keyState[MAX_KEY_CODE];
	int onceKeyState[MAX_KEY_CODE];
	std::map<std::string, int> currentKeyMap;

	Keyboard() {
		for (int i = 0; i < MAX_KEY_CODE; ++i) {
			keyState[i] = 0;
			onceKeyState[i] = false;
		}
	}

	void registerEvent (int key, int press) {
		if (press)
			keyState[key] = true;
		else {
			keyState[key] = false;
			onceKeyState[key] = false;
		}
		Util::StaticQueue <KeyEvent, QUE_SIZE>::insert(KeyEvent(key, press));
	}

	int getKeyState (int key) {
		if (key < 0 || key >= MAX_KEY_CODE)
			return false;
		return keyState[key];
	}

	int getOnceKeyState (int key) {
		if (!onceKeyState[key] && keyState[key]) {
			onceKeyState[key] = true;
			return true;
		}
		else {
			if (!keyState[key])
				onceKeyState[key] = false;
			return false;
		}
	}

	KeyEvent popEvent() {
		return Util::StaticQueue <KeyEvent, QUE_SIZE>::pop();
	}

	bool queEmpty() {
		return Util::StaticQueue <KeyEvent, QUE_SIZE>::empty();
	}

	static std::string nonAsciiKeys[];

	int ENTER;
	int SPACE;
	int CAPS_LOCK;
	int TAB;
	int L_ALT;
	int R_ALT;
	int L_CTRL;
	int R_CTRL;
	int L_SHIFT;
	int R_SHIFT;
	int ARROW_UP;
	int ARROW_DOWN;
	int ARROW_LEFT;
	int ARROW_RIGHT;
	int F1;
	int F2;
	int F3;
	int F4;
	int F5;
	int F6;
	int F7;
	int F8;
	int F9;
	int F10;
	int F11;
	int F12;
	int BACKSPACE;
	int INSERT;
	int DELETE;
	int HOME;
	int PAGE_UP;
	int PAGE_DOWN;
	int END;
	int PRINT_SCREEN;
	int SCREEN_LOCK;
	int PAUSE;
	int NUM_LOCK;
	int NUM_ENTER;
	int NUM_INSERT;
	int WINKEY;
	int FN;
	int ESC;

	void mapKeys (const std::map<std::string, int>& mapping) {
		auto getCode = [&](std::string strCode) -> int {
			if (mapping.find(strCode) != mapping.end())
				return (*mapping.find(strCode)).second;
			else
				return -1;
		};

		currentKeyMap = mapping;

		ENTER = getCode("ENTER");
		SPACE = getCode("SPACE");
		CAPS_LOCK = getCode("CAPS_LOCK");
		TAB = getCode("TAB");
		L_ALT = getCode("L_ALT");
		R_ALT = getCode("R_ALT");
		L_CTRL = getCode("L_CTRL");
		R_CTRL = getCode("R_CTRL");
		L_SHIFT = getCode("L_SHIFT");
		R_SHIFT = getCode("R_SHIFT");
		ARROW_UP = getCode("ARROW_UP");
		ARROW_DOWN = getCode("ARROW_DOWN");
		ARROW_LEFT = getCode("ARROW_LEFT");
		ARROW_RIGHT = getCode("ARROW_RIGHT");
		F1 = getCode("F1");
		F2 = getCode("F2");
		F3 = getCode("F3");
		F4 = getCode("F4");
		F5 = getCode("F5");
		F6 = getCode("F6");
		F7 = getCode("F7");
		F8 = getCode("F8");
		F9 = getCode("F9");
		F10 = getCode("F10");
		F11 = getCode("F11");
		F12 = getCode("F12");
		BACKSPACE = getCode("BACKSPACE");
		INSERT = getCode("INSERT");
		DELETE = getCode("DELETE");
		HOME = getCode("HOME");
		PAGE_UP = getCode("PAGE_UP");
		PAGE_DOWN = getCode("PAGE_DOWN");
		END = getCode("END");
		PRINT_SCREEN = getCode("PRINT_SCREEN");
		SCREEN_LOCK = getCode("SCREEN_LOCK");
		PAUSE = getCode("PAUSE");
		NUM_LOCK = getCode("NUM_LOCK");
		NUM_ENTER = getCode("NUM_ENTER");
		NUM_INSERT = getCode("NUM_INSERT");
		WINKEY = getCode("WINKEY");
		FN = getCode("FN");
		ESC = getCode("ESC");
	}

	std::string getName (int code) {
		for (auto&& pair : currentKeyMap) {
			if (pair.second == code)
				return pair.first;
		}

		if (0 <= code && code <= 128) {
			return std::string() + char(code);
		}

		return "NOT_FOUND";
	}
};

template <int QUE_SIZE>
std::string Keyboard<QUE_SIZE>::nonAsciiKeys[] = {
	"ENTER", "SPACE", "CAPS_LOCK", "TAB", "L_ALT", "R_ALT", "L_CTRL",
	"R_CTRL", "L_SHIFT", "R_SHIFT", "ARROW_UP", "ARROW_DOWN", "ARROW_LEFT",
	"ARROW_RIGHT", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
	"F11", "F12", "BACKSPACE", "INSERT", "DELETE", "HOME", "PAGE_UP",
	"PAGE_DOWN", "END", "PRINT_SCREEN", "SCREEN_LOCK", "PAUSE", "NUM_LOCK",
	"NUM_ENTER", "NUM_INSERT", "WINKEY", "FN", "ESC"
};

	// example of mapping:
	
	// mapping["ENTER"] = 1;
	// mapping["SPACE"] = 2;
	// mapping["CAPS_LOCK"] = 3;
	// mapping["TAB"] = 4;
	// mapping["L_ALT"] = 5;
	// mapping["R_ALT"] = 6;
	// mapping["L_CTRL"] = 8;
	// mapping["R_CTRL"] = 9;
	// mapping["L_SHIFT"] = 10;
	// mapping["R_SHIFT"] = 11;
	// mapping["ARROW_UP"] = 12;
	// mapping["ARROW_DOWN"] = 13;
	// mapping["ARROW_LEFT"] = 14;
	// mapping["ARROW_RIGHT"] = 15;
	// mapping["F1"] = 16;
	// mapping["F2"] = 17;
	// mapping["F3"] = 18;
	// mapping["F4"] = 19;
	// mapping["F5"] = 20;
	// mapping["F6"] = 21;
	// mapping["F7"] = 22;
	// mapping["F8"] = 23;
	// mapping["F9"] = 24;
	// mapping["F10"] = 25;
	// mapping["F11"] = 26;
	// mapping["F12"] = 27;
	// mapping["BACKSPACE"] = 28;
	// mapping["INSERT"] = 29;
	// mapping["DELETE"] = 30;
	// mapping["HOME"] = 31;
	// mapping["PAGE_UP"] = 32;
	// mapping["PAGE_DOWN"] = 33;
	// mapping["END"] = 34;
	// mapping["PRINT_SCREEN"] = 35;
	// mapping["SCREEN_LOCK"] = 36;
	// mapping["PAUSE"] = 37;
	// mapping["NUM_LOCK"] = 38;
	// mapping["NUM_ENTER"] = 39;
	// mapping["NUM_INSERT"] = 40;
	// mapping["WINKEY"] = 41;
	// mapping["FN"] = 42;
	// mapping["ESC"] = 43;

#endif