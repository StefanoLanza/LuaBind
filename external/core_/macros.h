#pragma once

/* For compatibility with Clang's __has_builtin() */
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

/**
 * __builtin_expect macros
 */
#if ! defined(HAVE___BUILTIN_EXPECT)
#define __builtin_expect(x, y) (x)
#endif

#ifndef likely
#ifdef HAVE___BUILTIN_EXPECT
#define likely(x)   __builtin_expect(! ! (x), 1)
#define unlikely(x) __builtin_expect(! ! (x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif
#endif

/**
 * Unreachable macro. Useful for suppressing "control reaches end of non-void
 * function" warnings.
 */
#if defined(HAVE___BUILTIN_UNREACHABLE) || __has_builtin(__builtin_unreachable)
#define unreachable(str)         \
	do {                         \
		assert(! str);           \
		__builtin_unreachable(); \
	} while (0)
#elif defined(_MSC_VER)
#define unreachable(str) \
	do {                 \
		assert(! str);   \
		__assume(0);     \
	} while (0)
#else
#define unreachable(str) assert(! str)
#endif

/**
 * Assume macro. Useful for expressing our assumptions to the compiler,
 * typically for purposes of silencing warnings.
 */
#if __has_builtin(__builtin_assume)
#define assume(expr)            \
	do {                        \
		assert(expr);           \
		__builtin_assume(expr); \
	} while (0)
#elif defined HAVE___BUILTIN_UNREACHABLE
#define assume(expr) ((expr) ? ((void)0) : (assert(! "assumption failed"), __builtin_unreachable()))
#elif defined(_MSC_VER)
#define assume(expr) __assume(expr)
#else
#define assume(expr) assert(expr)
#endif

/* Forced function inlining */
/* Note: Clang also sets __GNUC__ (see other cases below) */
#ifndef ALWAYS_INLINE
#if defined(__GNUC__)
#define ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif
#endif

/**
 * UNUSED marks variables (or sometimes functions) that have to be defined,
 * but are sometimes (or always) unused beyond that. A common case is for
 * a function parameter to be used in some build configurations but not others.
 * Another case is fallback vfuncs that don't do anything with their params.
 *
 * Note that this should not be used for identifiers used in `assert()`;
 * see ASSERTED below.
 */
#ifdef HAVE_FUNC_ATTRIBUTE_UNUSED
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

/**
 * Use ASSERTED to indicate that an identifier is unused outside of an `assert()`,
 * so that assert-free builds don't get "unused variable" warnings.
 */
#ifdef NDEBUG
#define ASSERTED UNUSED
#else
#define ASSERTED
#endif
