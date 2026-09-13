#ifndef CSA_LIB_H
#define CSA_LIB_H

#define CSA_DEFINITION_ID_CXX 0
#define CSA_DEFINITION_ID_C11 1
#define CSA_DEFINITION_ID_C23 2
#define CSA_DEFINITION_ID_FBT 3

#define CSA_DEFINITION_STR_CXX "CSA C++11 static_assert"
#define CSA_DEFINITION_STR_C11 "CSA C11 _Static_assert"
#define CSA_DEFINITION_STR_C23 "CSA C23 static_assert"
#define CSA_DEFINITION_STR_FBT "CSA fallback enum trick"

#if defined(__cplusplus)

    #if defined(_MSC_VER) && defined(_MSVC_LANG)
		/* Actually kinda useless nowadays because MSVC does not have compilation flags for anything below C++14, so this is only useful if you are compiling with an ancient MSVC version. In that case, this should be useful I suppose, but even then, anything that old would not support C++11 anyways, so we'd jump straight to the enum fallback trick implementation */
        #define CSA_CXX_VERSION _MSVC_LANG
    #else
        #define CSA_CXX_VERSION __cplusplus
    #endif

    #if CSA_CXX_VERSION >= 201103L
        #define CSA_STATIC_ASSERT(c, s) static_assert(c, s)
        #define CSA_DEFINITION_ID CSA_DEFINITION_ID_CXX
        #define CSA_DEFINITION_STR CSA_DEFINITION_STR_CXX
    #endif

#else

    #if defined(__STDC_VERSION__)
        #if __STDC_VERSION__ >= 202311L
            #define CSA_STATIC_ASSERT(c, s) static_assert(c, s)
            #define CSA_DEFINITION_ID CSA_DEFINITION_ID_C23
            #define CSA_DEFINITION_STR CSA_DEFINITION_STR_C23
        #elif __STDC_VERSION__ >= 201112L
            #define CSA_STATIC_ASSERT(c, s) _Static_assert(c, s)
            #define CSA_DEFINITION_ID CSA_DEFINITION_ID_C11
            #define CSA_DEFINITION_STR CSA_DEFINITION_STR_C11
        #endif
    #endif

#endif

#if !defined(CSA_STATIC_ASSERT)
    
    #define CSA_CATI(x, y) x##y
    #define CSA_CAT(x, y) CSA_CATI(x, y)

    #define CSA_STATIC_ASSERTI(c, l) enum { CSA_CAT(csa_custom_static_assert_at_, l) = 1 / (!!(c)) }
    #define CSA_DEFINITION_ID CSA_DEFINITION_ID_FBT
    #define CSA_DEFINITION_STR CSA_DEFINITION_STR_FBT
    
    #ifdef __COUNTER__
        #define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERTI(c, __COUNTER__)
    #else
        #define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERTI(c, __LINE__)
    #endif

#endif

#define custom_static_assert(cond, str) CSA_STATIC_ASSERT(cond, str)

#endif
