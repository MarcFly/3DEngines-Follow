#pragma once

#include "../Globals.h"
#include "Core/Logger.h"

namespace Engine {
	
	typedef uint64_t EventID;
	typedef uint64_t SubscriberID;
	
	struct EF_API Event {
		virtual ~Event() {/* Take care of your events garbage! */ };
	};

#define DECL_STATIC_EV_FUN(name)	static void name(Engine::Event* evt)

#define DECL_DYN_EV_FUNS(name)	void name(Engine::Event* evt); \
										static void name##Redirect(Engine::Event* evt, void* this_ptr);
#define DECL_DYN_ENGINE_EV_FUNS(name)	void name(Event* evt); \
										static void name##Redirect(Event* evt, void* this_ptr);
// You Receive var ev of the type you need
// Need to enclose it with double } if you use another { which is good practice
#define DEF_EV_FUN(evt_type, name) \
	void name(Engine::Event* evt) { \
		const evt_type* ev = (evt_type*)evt;

#define DEF_STATIC_MEMBER_EV_FUN(evt_type, membertype, name) \
	void membertype::name(Engine::Event* evt) {\
		const evt_type* ev = (evt_type*)evt;

/// TODO: Solve lifetime issues with this_ptr
/// We can't ensure validity to that ptr
///		struct ExampleEv {
///			EVENET_TYPE_CLASS(ExampleEv);
///			DECL_DYN_EV_FUN(MemberResponse);
/// 	}
/// 	DEF_DYN_MEMBER_EV_FUNS(ExampleEv, ExampleEv, MemberResponse){
///			bool dosomething;
/// 	}}
///		{	
///			ExampleEv ev;
///			Engine::Events::SubscribeDyn(ExampleEv::type, MemberResponseRedirect, &ev);
/// 	}
/// 	// Ev is no longer valid, but we have subscribed to it!
///		ExampleEv* testev = new ExampleEv();
/// 	Engine::Events::SendHot(testev); // Undefined behaviour, might not crash but...
///			
#define DEF_DYN_MEMBER_EV_FUNS(evt_type, membertype, name) \
	void membertype##::##name##Redirect(Engine::Event* evt, void* this_ptr) { \
		((membertype*)this_ptr)->name(evt); } \
	DEF_EV_FUN(evt_type, membertype##::##name)
	

	struct EF_API Events {
		static void Init();
		static void Close();

		typedef void(EventFun)(Event* ev);
		typedef void(EventRedirectFun)(Event* ev, void* this_ptr);

		struct Subscriber {
			Subscriber() {}
			std::vector <std::pair<void*, EventRedirectFun*>> redirected_responses;
			std::vector<EventFun*> responses;
			std::vector<Event*> queue;
		};
		
		template<typename T>
		static void Register() {
			T::type = typeid(T).hash_code();
			subscribers.insert(std::pair<uint64_t, Subscriber>(T::type, Subscriber()));
			Engine_INFO("Registered event: {}", T::GetName());
		}

		template<typename T>
		static void Subscribe(EventFun* answer) {
			if (T::type == UINT64_MAX) Register<T>();
			subscribers[T::type].responses.push_back(answer);
		}

		template<typename T>
		static void SubscribeDyn(EventRedirectFun* redirect_answer, void* this_ptr) {
			if (T::type == UINT64_MAX) Register<T>();
			subscribers[T::type].redirected_responses.push_back(std::pair<void*, EventRedirectFun*>(this_ptr, redirect_answer));
		}


		template<typename T>
		static void Send(T*& ev) {
			T* curr = ev;
			ev = nullptr;
			auto& v = subscribers.find(T::type);
			if (v != subscribers.end()) {
				v->second.queue.push_back((Event*)curr);
			}
			else {
				delete curr;
			}
		}
		
		// ONLY USE THIS IF YOU ARE SENDING A `new EventType(quick_initializer)` style event
		// Else I am not taking ownership of it...
		template<typename T>
		static void SendNew(T* ev) {
			T* curr = ev;
			auto& v = subscribers.find(T::type);
			if (v != subscribers.end()) {
				v->second.queue.push_back((Event*)curr);
			}
			else {
				Engine_WARN("Event {} not registered!", T::GetName());
				delete curr;
			}
		}

		template<typename T>
		static void SendHot(T*& ev) {
			T* curr = ev;
			ev = nullptr;
			auto& v = subscribers.find(T::type);
			if (v != subscribers.end()) {
				for (EventFun* fun : v->second.responses) {
					fun(curr);
				}
				for (std::pair<void*, EventRedirectFun*>& rfun : v->second.redirected_responses)
					rfun.second(curr, rfun.first);
			}
			
			delete curr;
		}

		// Ditto, use with `new Event()` directly, don't know how to enforce it
		template<typename T>
		static void SendHotNew(T* ev) {
			T* curr = ev;
			auto& v = subscribers.find(T::type);
			if (v != subscribers.end()) {
				for (EventFun* fun : v->second.responses) {
					fun(curr);
				}
				for (std::pair<void*, EventRedirectFun*>& rfun : v->second.redirected_responses)
					rfun.second(curr, rfun.first);
			}

			delete curr;
		}
		// Assume unsafe behaviours and save ont he find?

		static void DispatchAll();
		template<typename T>
		static void Dispatch() {

			if (subscribers.find(T::GetStaticType())) {}
		}

		static std::unordered_map<EventID, Subscriber> subscribers;

// Dynamic type is used for batch dispatching
#define EVENT_TYPE_CLASS(name) static uint64_t type; \
								static const char* GetName() { return #name; }

#define EVENT_TYPE_STATIC_DATA(name) uint64_t name::type = UINT64_MAX;
	};


}

