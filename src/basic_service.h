/*
 * basic_service.h
 *
 *  Created on: 20.10.2013
 *      Author: Peter Hübel
 */

#ifndef BASIC_SERVICE_H_
#define BASIC_SERVICE_H_

#include <xeno/service.h>

namespace xeno {
namespace examples {

class basic_service : public xeno::service<basic_service, const xeno::element> {
public:
	// The service has a complete context for initialisation,
	// the origin is 'const' so we may *not* make selfattribution
	basic_service(Context& origin)
	:	Origin(origin)
//	,	origin(origin)
	{
	}
	// when we return, we know that the cpu must flush the cache soon anyway, so we force it to do it now.
	// hence the retur type, giving the context to use for further processing (mostly the same visitor, just a frame above the transport)
	// once you start serving, you serve.
	// the machine gives you the context of the visitor,
	// you may inspect it, alter it, and you may hand over a extend context
	void invoke(xeno::contact& visitor, xeno::sequens& route);
//	// the generator function - has a default implementation, which invokes a constructor taking a context as argument
//	static xeno::action* create(xeno::context& location)
//	{
//		return new basic_service(location);
//	}
private:
//	const xeno::context& origin;
};

} /* namespace examples */
} /* namespace xeno */

#endif /* BASIC_SERVICE_H_ */
