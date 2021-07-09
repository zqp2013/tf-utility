#ifndef EXPORT_H_
#define EXPORT_H_

#ifdef _WIN32
    #ifdef TF_EXPORTS
    #define TF_EXT_CLASS __declspec(dllexport)
    #elif defined TF_LIB
    #define TF_EXT_CLASS
    #else
    #define TF_EXT_CLASS __declspec(dllimport)
    #endif
#else
    #ifdef TF_EXPORTS
    #define TF_EXT_CLASS __attribute__((visibility("default")))
    #elif defined TF_LIB
    #define TF_EXT_CLASS
    #else
	#define TF_EXT_CLASS
	#endif
#endif

#endif // EXPORT_H_
