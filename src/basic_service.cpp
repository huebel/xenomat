/*
 * basic_service.cpp
 *
 *  Created on: 20.10.2013
 *      Author: Peter Hübel
 */

#include <xeno/trace.h>
#include <xeno/document.h>

#include "basic_service.h"

namespace xeno {

using basic_service = examples::basic_service;
XENO_REGISTER(xeno, basic, basic_service);

namespace examples {

void basic_service::invoke(contact& visitor, sequens& route)
{
	TRACELN("basic_service::invoke");
	if (route.empty()) {
		visitor.status("204 No Content");
	}
	else {
		xml_preample(visitor);
		element& root = visitor.content().element().child("basic");
		while (!route.empty()) {
			root.elem(route.heading());
			route = route.tail();
		}
		visitor.status("200 OK");
	}
}

} // namespace examples
} // namespace xeno



