#pragma once

enum class Modifier
{
	NoModifier = 0,
	Shift = 1,
	Control = 2,
	Alt = 4,
	Super = 8,
};

enum class Action
{
	Release = 0,
	Press = 1,
	Repeat = 2,
};

enum class ButtonCode
{
	Unknown = -1,
	LeftButton = 0,
	RightButton = 1,
	ScrollButton = 2,
};

enum class KeyCode
{
	UNKNOWN = -1,
	Space = 32,
	UP = 265,
	DOWN = 264,
	RIGHT = 262,
	LEFT = 263,
	ESC = 256,
	ENTER = 257,
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,
	F1 = 290,
	F2 = 291,
	F3 = 292,
	F4 = 293,
	F5 = 294,
	F6 = 295,
	F7 = 296,
	F8 = 297,
	F9 = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301
};

class View;

class Controller
{
public:
	virtual ~Controller() {}
	virtual void onEnter(View&) {}
	virtual void onExit(View&) {}
	virtual void onMouseMove(View& view, double x, double y) {}
	virtual void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) {}
	virtual void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) {}
};
