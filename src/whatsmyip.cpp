/*
 * whatsmyip.cpp
 *
 *  Created on: 30.12.2013
 *      Author: Peter Hübel
 */
//#undef NDEBUG
#include <xeno/trace.h>

#include "whatsmyip.h"

#include <xeno/document.h>
#include <xeno/elements.h>
#include <iostream>

namespace xeno {
namespace examples {

using namespace boost::asio;

static const char* const CLIENT_ID = "xeno/examples/whatsmyip";

whatsmyip::whatsmyip(Context& origin)
:	Origin(origin)
,	host("@host", origin, "whatsmyip.de")
,	my_port("@my_port", origin, "8901") // TODO: get port from the space hosting this service
,	content_type("@type", origin, xeno::type::TEXT)
{
	TRACELN("whatsmyip::whatsmyip");
}

whatsmyip::~whatsmyip()
{
	TRACELN("whatsmyip::~whatsmyip");
}

void whatsmyip::invoke(xeno::contact& visitor, xeno::sequens& route)
{
	std::string my_addr;
	if (get_ip_address(my_addr)) {
		if (content_type == xeno::type::TEXT) {
			visitor.content_type(xeno::type::TEXT).content().text(my_addr);
		}
		else if (content_type == "text/html") {
			std::string href("http://"+my_addr);
			if (my_port.c_str() && *my_port.c_str()) {
				href.append(":").append(my_port.c_str());
				my_addr.append(":").append(my_port.c_str());
			}
			make_element(visitor.content_type(xeno::type::HTML).content(), "a").attr("href", href).text(my_addr);
		}
		visitor.status("200 OK");
	}
	else {
		visitor.content_type(xeno::type::TEXT).status("500 Internal Server Error").content().text(my_addr);
	}
}

bool whatsmyip::get_ip_address(std::string& addr) const
{
	ip::tcp::iostream client(host.c_str(), "80");
	client << "GET / HTTP/1.0\r\n";
	client << "Host: " << host.c_str() << "\r\n";
	client << "Accept: " << (content_type.defined() ? content_type.c_str() : "*/*") << "\r\n";
	client << "Accept-Charset: utf-8\r\n";
	client << "Accept-Encoding: identity\r\n";
	client << "User-Agent: " << CLIENT_ID << "\r\n";
	client << "\r\n" << std::flush;
	TRACE("SENT HTTP REQUEST TO [%s:80]\n", host.c_str());
	// TODO: error handling / HTTP parsing
	if (client) {
		std::string line;
		// skip past HTTP headers
		while (std::getline(client, line) && line != "\r")
#ifndef NDEBUG
			std::cerr << " > " << line << std::endl;
#endif
			/* skip */ ;
		// the skip until the <h3> where the IP is
		while (std::getline(client, line) && strncmp(line.c_str(), "<h3>", 4))
#ifndef NDEBUG
			std::cerr << " > " << line << std::endl;
#endif
			/* skip */ ;
		if (line.size()) {
#ifndef NDEBUG
			std::cerr << "FOUND > " << line;
#endif
			const std::size_t end = line.find('<', 4);
			if (end != std::string::npos) {
				// SUCCESS!
				addr.assign(line.substr(4, end-4));
				return true;
			}
		}
		// Catch all
		addr.assign(std::string("could not find address from http://")+host.c_str());
		return false;
	}
	// Catch all
	TRACELN("FAILED!");
	addr.assign(std::string("could not connect to ")+host.c_str());
	return false;
}

} /* namespace examples */
} /* namespace xeno */
