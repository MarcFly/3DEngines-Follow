#pragma once

namespace Engine {
	namespace INPUT {
		enum STATE
		{
			_STATE_UNKNOWN = -1,
			RELEASED,
			PRESSED,
			HELD
		};

		enum KB
		{
			_KB_UNKNOWN = -1,
			// 1 Initial key																													
			// MAIN PRINTABLE ASCII CHARACTERS																									
			SPACE = 32, SINGLE_QUOTE = 39, ASTERISK = 42, PLUS = 43,
			COMMA = 44, DASH = 45, DOT = 46, FORW_SLASH = 47,
			ZERO_0 = 48, ONE_1 = 49, TWO_2 = 50, THREE_3 = 51,
			FOUR_4 = 52, FIVE_5 = 53, SIX_6 = 54, SEVEN_7 = 55,
			EIGHT_8 = 56, NINE_9 = 57, COLON = 58, SEMI_COLON = 59,
			EQUALS = 61, GREATER_THAN = 62, END_QUESTION = 63, AT = 64,
			UPPER_A = 65, UPPER_B = 66, UPPER_C = 67, UPPER_D = 68,
			UPPER_E = 69, UPPER_F = 70, UPPER_G = 71, UPPER_H = 72,
			UPPER_I = 73, UPPER_J = 74, UPPER_K = 75, UPPER_L = 76,
			UPPER_M = 77, UPPER_N = 78, UPPER_O = 79, UPPER_P = 80,
			UPPER_Q = 81, UPPER_R = 82, UPPER_S = 83, UPPER_T = 84,
			UPPER_U = 85, UPPER_V = 86, UPPER_W = 87, UPPER_X = 88,
			UPPER_Y = 89, UPPER_Z = 90, OPEN_BRACKET = 91, BACK_SLASH = 92,
			CLOSE_BRACKET = 93, OPEN_ACCENT = 96,

			// 54 keys here																														

			// MAIN USAGE CHARACTERS																											
			WORLD_1 = 161, WORLD_2 = 162, ESCAPE = 256, ENTER = 257,
			TAB = 258, BACKSPACE = 259, INSERT = 260, _DELETE = 261,
			ARROW_RIGHT = 262, ARROW_LEFT = 263, ARROW_DOWN = 264, ARROW_UP = 265,
			PAGE_UP = 266, PAGE_DOWN = 267, HOME = 268, END = 269,
			CAPS_LOCK = 281, SCROLL_LOCK = 281, NUM_LOCK = 282, PRINT_SCREEN = 283,
			PAUSE = 284, F1 = 290, F2 = 291, F3 = 292,
			F4 = 293, F5 = 294, F6 = 295, F7 = 296,
			F8 = 297, F9 = 298, F10 = 299, F11 = 300,
			F12 = 301, F13 = 302, F14 = 303, F15 = 304,
			F16 = 305, F17 = 306, F18 = 307, F19 = 308,
			F20 = 309, F21 = 310, F22 = 311, F23 = 312,
			F24 = 313, F25 = 314, KEYPAD_1 = 320, KEYPAD_2 = 321,
			KEYPAD_3 = 322, KEYPAD_4 = 323, KEYPAD_5 = 324, KEYPAD_6 = 325,
			KEYPAD_7 = 326, KEYPAD_8 = 327, KEYPAD_9 = 328, KEYPAD_DECIMAL = 329,
			KEYPAD_DIVIDE = 331, KEYPAD_MULT = 332, KEYPAD_SUBTRACT = 333, KEYPAD_ADD = 334,
			KEYPAD_ENTER = 335, KEYPAD_EQUAL = 336, LEFT_SHIFT = 340, LEFT_CTRL = 341,
			LEFT_ALT = 342, LEFT_SUPER = 343, RIGHT_SHIFT = 344, RIGHT_CTRL = 345,
			RIGHT_ALT = 347, RIGHT_SUPER = 348,
			// 66 keys here																														
			// EXTENDED ASCII CHARACTERS																										

			// END																																
			KB_MAX
		};

		enum MOUSE
		{
			_LEFT = 0, _RIGHT, _CENTER, _1,
			_2, _3, _4, _5,
			_6, _7, _8, _9,
			_10, _11, _12, _13,

			MOUSE_MAX
		};

		enum ACTIONS
		{
			_ACTION_UNKNOWN = -1,
			// Button States																													
			RELEASE,
			PRESS,
			REPEAT,

			// Single Touch Controls
			TAP,
			SWIPE_UP,
			SWIPE_DOWN,
			SWIPE_LEFT,
			SWIPE_RIGHT,
			// Gameplay Actions																													

			// 																																	
			ACTION_MAX
		};
	};
}