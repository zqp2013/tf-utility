#ifndef EXPORT_H_
#define EXPORT_H_

#ifdef _WIN32
    #ifdef CXX_EXPORTS
    #define CXX_EXT_CLASS __declspec(dllexport)
    #elif defined CXX_LIB
    #define CXX_EXT_CLASS
    #else
    #define CXX_EXT_CLASS __declspec(dllimport)
    #endif
#else
    #ifdef CXX_EXPORTS
    #define CXX_EXT_CLASS __attribute__((visibility("default")))
    #elif defined CXX_LIB
    #define CXX_EXT_CLASS
    #else
	#define CXX_EXT_CLASS
	#endif
#endif

#endif // EXPORT_H_