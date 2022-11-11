#include "EnginePCH.h"
#include "ECS_Events.h"
#include "ECS.h"

using namespace Engine;

EVENT_TYPE_STATIC_DATA(NewEntity_Event);
DEF_DYN_MEMBER_EV_FUNS(NewEntity_Event, ECS, AddEntity_EventFun) {
	if (ev->entity_copy == UINT64_MAX)
		AddEntity(ev->parent_id);
	else {
		Entity* copy = GetEntity(ev->entity_copy);
		if (copy != nullptr) AddCopyEntity(*copy, ev->parent_id);
	}
}}

EVENT_TYPE_STATIC_DATA(DeleteEntity_Event);
DEF_DYN_MEMBER_EV_FUNS(DeleteEntity_Event, ECS, DeleteEntity_EventFun) {
	DeleteEntity(ev->entity_id);
}}

EVENT_TYPE_STATIC_DATA(ModifyEntity_Event);
DEF_DYN_MEMBER_EV_FUNS(ModifyEntity_Event, ECS, ModifyEntity_EventFun) {
	Entity* get = GetEntity(ev->cpy.id);
	if(get != nullptr) *get = ev->cpy;
	else {
		DeleteEntity_Event* del_ev = new DeleteEntity_Event(ev->cpy.id);
		Events::Send(del_ev);
	}
}}

EVENT_TYPE_STATIC_DATA(AddComponent_Event);
DEF_DYN_MEMBER_EV_FUNS(AddComponent_Event, ECS, AddComponent_EventFun) {
	Entity* e = GetEntity(ev->parent_id);
	CID temp(ev->copy_component_id);
	temp.parent_id = ev->parent_id;
	temp.ctype = ev->component_type;

	// TODO: Should the ECS AddComponent functions take care of setting in the entity as components?
	if (ev->copy_component_id != UINT64_MAX)
		e->components.push_back(AddCopyComponentGeneric(temp, temp.parent_id));
	else
		e->components.push_back(AddComponentGeneric(temp.ctype, temp.parent_id));
}}

EVENT_TYPE_STATIC_DATA(DeleteComponent_Event);
DEF_DYN_MEMBER_EV_FUNS(DeleteComponent_Event, ECS, DeleteComponent_EventFun) {
	DeleteComponent(ev->cid);
}}


EVENT_TYPE_STATIC_DATA(LOADJSONPrefab_Event);
DEF_DYN_MEMBER_EV_FUNS(LOADJSONPrefab_Event, ECS, LOADJSONPrefab_EventFun) {
	DeserializePrefab(ev->json.value);
}}

#include "DearIMGui/Windows/EntityHierarchy/EntityHierarchy.h"

DEF_DYN_MEMBER_EV_FUNS(RequestEntities_Event, ECS, RequestEntities_EventFun) {
	Events::SendHotNew(new ProvideEntities_Event(this));
}}

EVENT_TYPE_STATIC_DATA(RequestSingleEntity_Event);
DEF_DYN_MEMBER_EV_FUNS(RequestSingleEntity_Event, ECS, RequestSingleEntity_EventFun) {
	Entity* ret = GetEntity(ev->entity.id);
	if (ret != nullptr) ev->entity = *ret;
}}

EVENT_TYPE_STATIC_DATA(RequestEntityComponents_Event);
DEF_DYN_MEMBER_EV_FUNS(RequestEntityComponents_Event, ECS, RequestEntityComponents_EventFun) {
	ev->components.clear();
	Entity* ret = GetEntity(ev->entity_id);
	ev->components.clear();
	if (ret != nullptr) {
		for (int i = 0; i < ret->components.size(); ++i) {
			Component* c = GetComponentGeneric(ret->components[i]);
			ev->components.emplace_back(c);
		}
	}
}}