#pragma once

#include "../Globals.h"
#include "InputEnums.h"
#include "Events/Events.h"


namespace Engine {
	struct KeyEvent : public Event {
		EVENT_TYPE_CLASS(KeyEvent);
		KeyEvent(int _code, int _state, int _prev) : code(_code), state(_state), prev_state(_prev) {};
		int code, state, prev_state;
	};
	typedef void(KeyFun)(KeyEvent* ev);

	struct EF_API Key {
		int prev_state = -1;
		int state = -1;
		std::vector<KeyFun*> callbacks;
		std::vector<VoidFun*> state_specific_cb[INPUT::ACTIONS::REPEAT];
	};

#define MAX_MIDPOINTS 10
	struct EF_API Gesture {
		// Timer
		float start_x = INT32_MAX, start_y = INT32_MAX;
		float midpoints[MAX_MIDPOINTS][2];
		float end_x = INT32_MAX, end_y = INT32_MAX;

		float refresh_bucket = 0;
	};


	struct MousePosEvent : public Event {
		EVENT_TYPE_CLASS(MousePosEvent);
		MousePosEvent(int _pointer_id, float _x, float _y) : id(_pointer_id), x(_x), y(_y) {}
		int id; float x, y;
	};
	typedef void(MousePosFun)(MousePosEvent* ev);
	
	struct ScrollEvent : public Event {
		EVENT_TYPE_CLASS(ScrollEvent);
		ScrollEvent(float _x, float _y) : x(_x), y(_y) {}
		float x, y;
	};

	struct EF_API Pointer {
		int32_t id;

		float x = -1, y = -1;
		float scrollx = 0, scrolly = 0;
		Gesture gesture;

		std::vector<MousePosFun*> pos_callbacks;
		Key buttons[INPUT::MOUSE::MOUSE_MAX];
	};

	struct EF_API Input {
		static Key keys[INPUT::KB::KB_MAX];
		static Pointer pointers[10];

		static void Init();
		static void AttachWindow(uint64_t window_id);
		static void Close();
		static void Poll();

		static void GetPointerPos(float& x, float& y, int pointer_id = 0);
		static void GetPointerButton(int& state, INPUT::MOUSE button, int pointer_id = 0);
		static void GetPointerScroll(float& x, float& y, int pointer_id = 0);
		static void GetKey(int& state, int code);

		static void GetAvgPointerPos(float& x, float& y);

		// Quicker way to subscribe to all keyevents, semantically more pleasing, but not the intended
		static void Subscribe(KeyFun* callback) { Events::Subscribe<KeyEvent>((Events::EventFun*)callback); }
		// Register to one specific key
		static void AddKeyCallback(int code, KeyFun* callback);
		static void AddKeyPressCallback(int code, VoidFun* callback);
		static void AddKeyReleaseCallback(int code, VoidFun* callback);
		static void AddKeyRepeatCallback(int code, VoidFun* callback);
		static void AddMouseButtonCallback(int mouse_button, KeyFun* callback, int id = 0);
		static void AddMouseButtonPressCallback(int mouse_button, VoidFun* callback, int id = 0);
		static void AddMouseButtonReleaseCallback(int mouse_button, VoidFun* callback, int id = 0);
		static void AddMouseButtonRepeatCallback(int mouse_button, VoidFun* callback, int id = 0);
		/*
		SQ_API void SQUE_INPUT_DisplaySoftwareKeyboard(const bool show);
		SQ_API SQUE_INPUT_Actions SQUE_INPUT_EvalGesture();
		SQ_API SQUE_INPUT_Actions SQUE_INPUT_DetectGesture(const SQUE_Gesture& g);
		*/
	};
};