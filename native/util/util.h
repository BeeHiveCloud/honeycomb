#ifndef INCLUDE_UTIL_H_
#define INCLUDE_UTIL_H_

#ifdef __cplusplus
# define HC_BEGIN_DECL extern "C" {
# define HC_END_DECL	}
#else
 /** Start declarations in C mode */
# define HC_BEGIN_DECL /* empty */
 /** End declarations in C mode */
# define HC_END_DECL	/* empty */
#endif

/** Declare a public function exported for application use. */
#if __GNUC__ >= 4
# define HC_EXTERN(type) extern \
			 __attribute__((visibility("default"))) \
			 type
#elif defined(_MSC_VER)
# define HC_EXTERN(type) __declspec(dllexport) type
#else
# define HC_EXTERN(type) extern type
#endif

#endif
