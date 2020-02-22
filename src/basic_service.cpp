/*
 * basic_service.cpp
 *
 *  Created on: 12.05.2019
 *      Author: Peter Hübel
 */

#include <xeno/trace.h>
#include <xeno/document.h>

#include "basic_service.h"

namespace xeno {

using basic_service = test::basic_service;
XENO_REGISTER(test, basic, basic_service);

namespace test {

void basic_service::invoke(contact& visitor, sequens& route)
{
	TRACELN("basic_service::invoke");
	if (route.empty()) {
		visitor.blame();
	}
	else {
		if (!type.defined()) make_xml_preample(visitor);
		contens initial_route(route.head());
		element& root = visitor.content().element().child(route.heading());
		route = route.tail();
		while (!route.empty()) {
			if (route.heading("fuck")) {
				root.text("Well f*ck a duck... No rude language, if you please!");
				break;
			}
			root.elem(route.heading());
			route = route.tail();
		}
		root.push_back(initial_route);
		visitor.content_type(type.c_str()).status("200 OK");
	}
}

} // namespace test
} // namespace xeno
