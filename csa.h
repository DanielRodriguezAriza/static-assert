#ifndef CSA_LIB_H
#define CSA_LIB_H

#define CSA_DEFINITION_ID_CXX      0
#define CSA_DEFINITION_ID_C11      1
#define CSA_DEFINITION_ID_C23      2
#define CSA_DEFINITION_ID_FALLBACK 3

#define CSA_DEFINITION_STR_CXX      "CSA C++11 static_assert"
#define CSA_DEFINITION_STR_C11      "CSA C11 _Static_assert"
#define CSA_DEFINITION_STR_C23      "CSA C23 static_assert"
#define CSA_DEFINITION_STR_FALLBACK "CSA fallback trick"

#define CSA_STATIC_ASSERT_FALLBACK_MODE_ENUM          0
#define CSA_STATIC_ASSERT_FALLBACK_MODE_TYPEDEF       1
#define CSA_STATIC_ASSERT_FALLBACK_MODE_USER_DEFINED 99

#ifndef CSA_STATIC_ASSERT_FALLBACK_MODE
#define CSA_STATIC_ASSERT_FALLBACK_MODE CSA_STATIC_ASSERT_FALLBACK_MODE_TYPEDEF
#endif

#if defined(__cplusplus)

	#if defined(_MSC_VER) && defined(_MSVC_LANG)
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
	
	#define CSA_DEFINITION_ID CSA_DEFINITION_ID_FALLBACK
	#define CSA_DEFINITION_STR CSA_DEFINITION_STR_FALLBACK
	
	#if CSA_STATIC_ASSERT_FALLBACK_MODE == CSA_STATIC_ASSERT_FALLBACK_MODE_ENUM
	
		#define CSA_CATI(x, y) x##y
		#define CSA_CAT(x, y) CSA_CATI(x, y)

		#define CSA_STATIC_ASSERTI(c, l) enum { CSA_CAT(csa_custom_static_assert_enum_, l) = 1 / (!!(c)) }
		
		#ifdef __COUNTER__
			#define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERTI(c, __COUNTER__)
		#else
			#define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERTI(c, __LINE__)
		#endif
	
	#elif CSA_STATIC_ASSERT_FALLBACK_MODE == CSA_STATIC_ASSERT_FALLBACK_MODE_TYPEDEF
		
		#define CSA_STATIC_ASSERT(c, s) typedef int csa_custom_static_assert_typedef[1 - 2*(!(c))]
	
	#elif CSA_STATIC_ASSERT_FALLBACK_MODE == CSA_STATIC_ASSERT_FALLBACK_MODE_USER_DEFINED
		
		#ifndef CSA_STATIC_ASSERT_USER_DEFINED
			#error "User must define the macro CSA_STATIC_ASSERT_USER_DEFINED(c, s) to offer their own fallback implementation for CSA_STATIC_ASSERT"
		#endif
		
		#define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERT_USER_DEFINED(c, s)
	
	#else
		
		#error "No valid CSA_STATIC_ASSERT_FALLBACK_MODE was selected"
	
	#endif

#endif

#define custom_static_assert(cond, str) CSA_STATIC_ASSERT(cond, str)

#endif
