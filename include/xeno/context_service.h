/*
 * context_service.h
 *
 *  Created on: 15.11.2018
 *      Author: Peter Hübel
 */

#ifndef XENO_CONTEXT_SERVICE_H_
#define XENO_CONTEXT_SERVICE_H_

#include "service.h"
#include "xeno_io.h"

namespace xeno XENO_NAMESPACE_EXPORT {

template <class SERVICE = xeno::action, class CONTEXT = xeno::context>
class XENO_EXPORT context_service : public service<SERVICE,CONTEXT> {
public:
	inline context_service(Context& ctx)
	:	origin_(ctx)
	{
	}

	// How to make a service
	static inline action* create(Context& origin)
	{
		context_reader r(origin);
		return new Service(r);
	}

	template <class SVC = Origin::Service, class CTX = Origin::Context>
	static inline action* create(CTX& origin)
	{
		return create(static_cast<Context&>(origin));
	}
};

} // namespace xeno

#endif//XENO_CONTEXT_SERVICE_H_
