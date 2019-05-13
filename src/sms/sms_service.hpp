/*
 * basic_service.h
 *
 *  Created on: 12.05.2019
 *      Author: Peter Hübel
 */

#ifndef BASIC_SERVICE_H_
#define BASIC_SERVICE_H_

#include <xeno/service.h>

namespace xeno {
namespace test {

class basic_service : public xeno::service<basic_service, const xeno::element> {
public:

	// The service has a complete context for initialisation,
	// the origin is 'const' so this service can *not* do any
	// selfattribution.
	basic_service(Context& origin)
	:	Origin(origin)
	,	type("@type", origin, type::XML)
	{
	}

	// The service is invoked via this call. See the implementation
	// for an understanding of the "visitor" and its "route".
	void invoke(xeno::contact& visitor, xeno::sequens& route);

	// The generator function 'create' has a default implementation,
	// which invokes a constructor taking a context as argument.
	// Overwrite this function, if you need to decide before instan

//	static xeno::action* create(xeno::context& location)
//	{
//		return new basic_service(location);
//	}

private:
	attribute type;
};

} /* namespace test */
} /* namespace xeno */

#endif /* BASIC_SERVICE_H_ */
