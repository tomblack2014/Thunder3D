#ifndef _COMMON_H
#define _COMMON_H

#if defined(_MSC_VER) && (defined(DEBUG) || defined(_DEBUG))
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#define LOGE(expr) LogError(#expr, __FILE__, __LINE__)
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define ChkLeak _CrtDumpMemoryLeaks()
#define ChkMem  _ASSERTE( _CrtCheckMemory( ) )
#else
#include <cstdlib>

#define _ASSERT(expr)
#define _ASSERTE(expr)

#define LOGE(expr)
#undef _CRTDBG_MAP_ALLOC
#define ChkLeak
#define ChkMem
#endif // _CRT_DEBUG

#ifdef _MSC_VER
#include <sal.h>
#else
#define _In_
#define _Out_
#define _Inout_
#define _In_opt_
#define _Outptr_
#endif // _MSC_VER

#include <unknwn.h>

template <class Interface>
inline void SafeRelease(_Inout_ Interface** interfaceToRelease)
{
	if(*interfaceToRelease != NULL){
		(*interfaceToRelease)->Release();
		(*interfaceToRelease) = NULL;
	}
}

template <class InterfaceA, class InterfaceB>
inline bool Equal(_In_opt_ InterfaceA* a, _In_opt_ InterfaceB* b)
{
	if(a == NULL || b == NULL){
		return a == b;
	} else{
		IUnknown* ua = NULL;
		IUnknown* ub = NULL;
		a->QueryInterface(IID_IUnknown, (void**)&ua);
		b->QueryInterface(IID_IUnknown, (void**)&ub);
		bool ret = (ua == ub);
		SafeRelease(&ua);
		SafeRelease(&ub);
		return ret;
	}
}

void LogError(const char* str, const char* file, int line);

#endif