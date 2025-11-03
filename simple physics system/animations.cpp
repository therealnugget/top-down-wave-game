#include "animations.hpp"
#include <stdarg.h>
#include "main.hpp"
#include "debug.hpp"
//int, int, const char *, int, const char *, etc. this is a trade-off against character-efficiency in favour of performance (no need to actually include a dictionary for this.)
CCList Animations::MakeAnimStrs(int numArgs, ...) {
	va_list args;
	va_start(args, numArgs);
#ifdef DEBUG_BUILD
	if (numArgs < 1) ThrowError("number of arguments cannot be less than 1");
#endif
	auto retVal = std::vector<const char*>(numArgs);
	int currentAnim;
#ifdef DEBUG_BUILD
	try {
#endif
		for (int i = 0; i < numArgs * 2; i++) {
			if (i & 1) {
				retVal[currentAnim] = va_arg(args, const char*);
				continue;
			}
			currentAnim = va_arg(args, int);
		}
#ifdef DEBUG_BUILD
	}
	catch (std::exception ex) {
		//i realised there's no way for me to get the type of the 2nd argument, so giving this long-winded error message is probably the most dev-friendly option.
		ThrowError(3, "failed with error: ", ex.what(), "maybe wrong arg inputted for 2nd argument? arg pattern is \"int, int, const char *, int, const char *, etc.\".\n");
	}
#endif
	va_end(args);
	return retVal;
}