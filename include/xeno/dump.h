/*
 * dump.h
 *
 *  Created on: 06.04.2014
 *      Author: Peter Hübel
 */

#ifndef XENO_DUMP_H_
#define XENO_DUMP_H_

#include <cassert>
#include "xeno.h"

namespace xeno {

	inline void dump(const char prefix[], const context& element, const char postfix[] = "\n") {
		assert(prefix && postfix && "don't pass NULL, dummy!");
		printf("%s<%s/>%s", prefix, element.qname(), postfix);
	}

	inline void dump(const context& element, const char postfix[] = "\n") {
		dump("", element, postfix);
	}

};



#endif /* XENO_DUMP_H_ */
