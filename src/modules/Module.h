#pragma once
#include <src/helpers/Globals.h>
#include <vector>
#include <memory>
#include <src/modules/EventSystem/Event.h>

class Application;

class Module
{
private :
	bool enabled;

public:
	const char* name = "noname";

	Module() {}
	Module(const char* _name, bool start_enabled = true) : name(_name)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}
	virtual bool IsStaticModule() { return false; }

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec) {}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void Save(JSON_Object* obj) {};
	virtual void Load(JSON_Object* obj) {};
};