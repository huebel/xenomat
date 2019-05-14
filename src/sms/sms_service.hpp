/*
 * basic_service.hpp
 *
 *  Created on: 12.05.2019
 *      Author: Peter Hübel
 */

#ifndef SMS_SERVICE_HPP_
#define SMS_SERVICE_HPP_

#include <xeno/service.h>

namespace macbook {

using namespace xeno;

class sms_service : public xeno::service<sms_service, const xeno::element> {
public:

	// The service has a complete context for initialisation,
	// the origin is 'const' so this service can *not* do any
	// selfattribution.
	sms_service(Context& origin);

	// The destructor releases the accelerometer again.
	~sms_service();

	// The service is invoked via this call. See the implementation
	// for an understanding of the "visitor" and its "route".
	void invoke(xeno::contact& visitor, xeno::sequens& route);

	// The generator function 'create' has a default implementation,
	// which invokes a constructor taking a context as argument.
	// Overwrite this function, if you need to decide before instan

//	static xeno::action* create(xeno::context& location)
//	{
//		return new sms_service(location);
//	}

private:
	attribute type;
};

} /* namespace macbook */

#endif /* SMS_SERVICE_HPP_ */
