#include "animations.hpp"
#include <stdarg.h>
#include "linkedList.hpp"
#ifdef DEBUG_BUILD
#include <typeinfo>
#endif
//int, int, const char *, int, const char *, etc.
StrList Animations::MakeAnimStrs(int numElems, ...) {
	va_list args;
	va_start(args, numElems);
	auto retVal = std::vector<const char*>(numElems / 2);
	bool iOdd;
	int retValInd;
	for (int i = 0; i < numElems; i++) {
		iOdd = static_cast<bool>(i & 1);

#ifdef DEBUG_BUILD
		if (iOdd ? typeid(va_arg(args, const char*)).hash_code() : typeid(va_arg(args, int)).hash_code() != !iOdd * typeid(int).hash_code() + iOdd * typeid(const char*).hash_code()) ThrowError("wrong type inputted. should be inputted as shown on line 7.");
#endif
		if (iOdd) {

		}
		else {
			retValInd = va_arg(args, int);
		}
	}
	va_end(args);
	return StrList();//TODO: finish making this func
}