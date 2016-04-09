/*
 * libmain.cpp
 *
 *  Created on: 30.10.2013
 *      Author: Peter Hübel
 */

//#undef NDEBUG
#include <xeno/trace.h>
#include <xeno/xeno.h>

#ifdef NDEBUG
#define GINS_XENO_BUILD_TYPE "release"
#else
#define GINS_XENO_BUILD_TYPE "debug"
#endif//NDEBUG

#define GINS_XENO_VERSION "{date='2014-03-21',xeno='" XENO_VERSION "',build='" GINS_XENO_BUILD_TYPE "'}"

//#include <xeno/examples/whatsmyip.h>

namespace gins /* __attribute__((externally_visible)) */ {

extern const char NODEFAULT[] = "???MISSING ATTRIBUTE???";
extern const char ZERO[] = "0";
extern const char ONE[] = "1";
extern const char NEGATIVE_ONE[] = "-1";
extern const char DEFAULT_CLOCK_CYCLE[] = "100";

}

void __attribute__ ((constructor)) libmain()
{
	printf("Loading libgins-netropy.so (" GINS_XENO_VERSION ")\n");

#define SHOW_GINS_CONSTANT(c)\
	TRACE("CONST "#c"='%s'\n", c)

	SHOW_GINS_CONSTANT(gins::NODEFAULT);
	SHOW_GINS_CONSTANT(gins::ZERO);
	SHOW_GINS_CONSTANT(gins::ONE);
	SHOW_GINS_CONSTANT(gins::DEFAULT_CLOCK_CYCLE);

#undef SHOW_GINS_CONSTANT

	// XENO example (declared in two namespaces)
//	xeno::declare<xeno::examples::whatsmyip>("xeno", "whatsmyip");
//	xeno::declare<xeno::examples::whatsmyip>("gins", "whatsmyip");

	printf("Services of libgins-netropy.so (" GINS_XENO_VERSION ") declared\n");
}

void __attribute__ ((destructor)) libexit()
{
	printf("Unloading libgins-netropy.so (" GINS_XENO_VERSION ")\n");

	// XENO example (declared in two namespaces)
//	xeno::nullify<xeno::examples::whatsmyip>("xeno", "whatsmyip");
//	xeno::nullify<xeno::examples::whatsmyip>("gins", "whatsmyip");

	printf("Services of libgins-netropy.so (" GINS_XENO_VERSION ") nullified\n");
}
