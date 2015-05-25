#ifndef _DAYLITE_COMPAT_HPP_
#define _DAYLITE_COMPAT_HPP_

#ifdef _MSC_VER
#define D_INLINE __inline
#define DLL_EXPORT __declspec(dllexport)
#else
#define D_INLINE inline
#define DLL_EXPORT
#endif

#endif