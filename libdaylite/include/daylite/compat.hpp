#ifndef _DAYLITE_COMPAT_HPP_
#define _DAYLITE_COMPAT_HPP_

#ifdef _MSC_VER
#define D_INLINE __inline
#ifdef BUILD_DAYLITE_DLL
#define DLL_EXPORT __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define DLL_EXPORT __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#else
#define D_INLINE inline
#define DLL_EXPORT
#endif

#endif