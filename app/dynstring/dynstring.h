#ifndef HAVE_DYNSTRING_H
#define HAVE_DYNSTRING_H

typedef struct DynString DynString;
struct DynString
{
	char *s;
	size_t size;		// length of the string
	size_t b_size;		//  length of the buffer containing the string
};

#define NaS {NULL, 0, 0}
#define isnas(S) (!(S)->s)

// define highest bit depending on 32 or 64 bit compile

#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) &&  !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
    #define STR_FREEABLE (1ULL << 63)
#else
    #define STR_FREEABLE (1ULL << 31)
#endif

size_t DynStringSize(DynString * s);

DynString * DynStringMalloc(DynString *s, size_t size);
void DynStringClear(DynString *s);
void DynStringRealloc(DynString * s);
void DynStringResize(DynString * s, size_t size);
void DynStringFree(DynString * s);
DynString * DynStringDupStr(DynString *s2, DynString * s);
void DynStringCpyStr(DynString * dest, DynString * src);
char * DynStringToCStr(DynString * s);
void DynStringNCatCStr(DynString * s, size_t len, const char * str);
void DynStringCatCStr(DynString * s, const char * str);
void DynStringCatStr(DynString * s, const DynString * s2);
void DynStringCatCStrs(DynString * s, ...);
void DynStringCatStrs(DynString * s1, ...);
void DynStringPrintf(DynString * s, const char * fmt, ...);

#endif // !HAVE_DYNSTRING_H
