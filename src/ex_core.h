

#ifndef EX_CORE_H
#define EX_CORE_H

#define _NEW

#include "stddef.h"

extern "C" {

	void* memcpy(void* dst, const void* src, unsigned int sz);

	void *malloc(size_t size);
	void *realloc(void *ptr, size_t size);
	void free(void* p);

	void printf(const char* fmt, ...);
	int sprintf(char *str, const char *format, ...);
	
	void eat_shit_and_die();
}

namespace std {
	inline void __throw_bad_alloc() {
		printf("Bad alloc!\n");
		eat_shit_and_die();
	}
}

inline void* operator new (size_t sz) {
	return malloc(sz);
}

/* placement new */
inline void* operator new (size_t sz, void* p) {
	return p;
}

inline void operator delete(void* p) {
	free(p);
}

extern "C" void _assert(const char *__assertion, const char *__file, int __line);
#define assert(X) if(!(X)) _assert(#X, __FILE__, __LINE__);

#endif // EX_CORE_H