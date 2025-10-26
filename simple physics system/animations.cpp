#include "animations.hpp"
#include <stdarg.h>
#include "linkedList.hpp"
#include "main.hpp"
//int, int, const char *, int, const char *, etc.
StrList Animations::MakeAnimStrs(int numElems, ...) {
	va_list args;
	va_start(args, numElems);
	auto retVal = std::vector<const char*>(numElems / 2);
	bool iOdd;
	union PtrInt {
		int i;
		const char* ptr;
	} ptrInt;
#ifdef DEBUG_BUILD
	try {
#endif
		for (int i = 0; i < numElems; i++) {
			iOdd = static_cast<bool>(i & 1);
			if (iOdd) {
				ptrInt.ptr = va_arg(args, const char*);
			}
			else {
				ptrInt.i = va_arg(args, int);
			}
		}
#ifdef DEBUG_BUILD
	}
	catch (std::exception ex) {
		//i realised there's no way for me to get the type of the 2nd argument, so giving this long-winded error message is probably the most dev-friendly option.
		ThrowError(3, "failed with error: ", ex.what(), "maybe wrong arg inputted for 2nd argument? arg pattern is \"int, int, const char *, int, const char *, etc.\".\n");
	}
#endif
	va_end(args);
	return StrList();
}