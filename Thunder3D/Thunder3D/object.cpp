#include "object.h"

// {B18B8A7B-2055-4CAA-99FD-9CD53AFD6FF9}
const GUID IObject::IID_IObject =
{0xb18b8a7b, 0x2055, 0x4caa, {0x99, 0xfd, 0x9c, 0xd5, 0x3a, 0xfd, 0x6f, 0xf9}};

Object::Object() :
refcnt(0),
pos()
{
}

Object::~Object(){}

ULONG Object::AddRef(){
	refcnt ++;
	return refcnt;
}

ULONG Object::Release(){
	-- refcnt;
	if(refcnt == 0){
		delete this;
		return 0;
	} else{
		return refcnt;
	}
}

HRESULT Object::QueryInterface(_In_ const IID&id, _Outptr_ void**ppv){
	if(id == IID_IUnknown){
		*ppv = static_cast<IUnknown*>(this);
	} else if(id == IID_IObject){
		*ppv = static_cast<IObject*>(this);
	} else{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

Vec4f Object::GetPos(){
	return pos;
}

void Object::Frame(_In_ float /*interval*/){
}

void Object::Render(){
}

void* Object::operator[](_In_opt_ const std::string& name){
	if(name == "pos"){
		return &pos;
	}else{
		return NULL;
	}
}