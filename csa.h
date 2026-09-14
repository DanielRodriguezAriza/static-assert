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
/* Default fallback mode is enum because typedef may be unreliable if the compiler supports VLAs */
#define CSA_STATIC_ASSERT_FALLBACK_MODE CSA_STATIC_ASSERT_FALLBACK_MODE_ENUM
#endif

#if defined(__cplusplus)

	#if defined(_MSC_VER) && defined(_MSVC_LANG)
		/*
			NOTE:
			Starting at a certain version of MSVC, the compiler defines _MSVC_LANG, which stores the version identifier of the C++ version used to compile
			the code. Basically, this value depends on the /std:c++ version flag, and it contains the value that __cplusplus should contain.
			In versions prior to _MSVC_LANG existing, there was no way to determine what C++ version was being compiled, but that does not matter, since
			back then, MSVC barely had anything above C++98, so static_assert was not a thing.
			
			In the modern versions of MSVC where _MSVC_LANG is defined, the compiler only supports C++14 and above, meaning that there is no way to
			compile in pure C++11, C++98, or any prior-to-standardization versions of C++.
			
			Also note that in some versions of MSVC, the _MSVC_LANG macro has been accidentally defined to contain version codes for future C++ standards.
			For instance, C++17 mode has sometimes defined the code to be C++20 instead. All this does is accidentally enable C++20 features when compiling
			in C++17 mode, which does not affect us in any way for this specific header's purposes, as all versions starting C++11 have static_assert, and
			modern MSVC supports at least C++14, no less.
			
			Knowing this, it becomes clear that this code could be simplified to just assuming that, if we're on MSVC (_MSC_VER is defined) and the _MSVC_LANG
			macro is defiend, then we can just use static_assert directly. But if we're on MSVC, and _MSVC_LANG is not defined, then we must fall back to
			a pre-C++11 trick. This would save us from having to check the version in _MSVC_LANG, which, even if its wrong on this specific version of MSVC,
			it does not matter, because we know it will always be defined to something above C++14, making it reliable enough to know that static_assert
			is available.
			
			For now, this implementation is OK, so I will not change it, but this could be improved upon in the future by other people if you wish to do so.
		*/
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

		/*
			NOTE:
			Prefered fallback mode by default.
			Supported by all standard versions of C and C++, as far as I'm aware.
			Ensures that the condition must be a compiletime known constant, because enum values cannot be assigned a runtime value in C and C++.
		*/
		#define CSA_STATIC_ASSERTI(c, l) enum { CSA_CAT(csa_custom_static_assert_enum_, l) = 1 / (!!(c)) }
		
		#ifdef __COUNTER__
			#define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERTI(c, __COUNTER__)
		#else
			#define CSA_STATIC_ASSERT(c, s) CSA_STATIC_ASSERTI(c, __LINE__)
		#endif
	
	#elif CSA_STATIC_ASSERT_FALLBACK_MODE == CSA_STATIC_ASSERT_FALLBACK_MODE_TYPEDEF
		
		/*
			NOTE:
			Alternative fallback mode.
			Supported by all standard versions of C and C++, as far as I'm aware.
			May accidentally declare a VLA typedef in standards compliant C compilers that support VLAs, and inC++ compilers that offer VLAs as an extension.
			
			WARNING:
			Use this mode with care, as it may define a VLA typedef, completely defeating the purpose of the static assertion.
			In C, typedefs to VLAs can be made, so if the condition is a runtime known value that can be used in a VLA declaration, then this version
			of the static assert implementation will not work properly, as it will act as if the value were assumed to be non zero by default, making
			the check useless, as it is completely bypassed.
			
			The only reason to prefer this mode is if your target compiler does not support the enum trick because the compiler in question is not
			standards compliant, or if you want slightly less verbose error messages.
			
			In short: typedefs to arrays with constant defined sizes will fail to compile if the array size is a negative value, which will make the
			assertion trigger. But if your condition comes from a runtime value that can be used in a VLA definition context, it will be assumed to be
			a positive and non zero value, which will result in the assertion not triggering any warnings or errors regarding a non constant value being used
			as the condition.
		*/
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
