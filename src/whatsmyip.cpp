/*
 * whatsmyip.cpp
 *
 *  Created on: 30.12.2013
 *      Author: Peter Hübel
 */

//#undef NDEBUG
//#define NDEBUG

#include <xeno/trace.h>
#include <xeno/document.h>
#include <xeno/elements.h>

#include <iostream>

#include "whatsmyip.h"

namespace xeno {

XENO_REGISTER(test, ip, whatsmyip);

namespace test {

using namespace boost::asio;

static const char* const CLIENT_ID = "xeno/examples/whatsmyip";

whatsmyip::whatsmyip(Context& origin)
:	Origin(origin)
,	host("@host", origin, "whatsmyip.de")
,	my_port("@my_port", origin, "8901")
,	content_type("@type", origin, xeno::type::TEXT)
{
	TRACE("whatsmyip(%p)::whatsmyip\n", this);
}

whatsmyip::~whatsmyip()
{
	TRACE("whatsmyip(%p)::~whatsmyip\n", this);
}

void whatsmyip::invoke(contact& visitor, sequens& route)
{
	TRACE("whatsmyip(%p)::invoke\n", this);
	std::string my_addr;
	if (get_ip_address(my_addr)) {
		if (content_type == xeno::type::TEXT) {
			xeno::make_text(visitor.content(), my_addr.c_str());
			visitor.status("200 OK").content_type(xeno::type::TEXT);
		}
		else if (content_type == "text/html") {
			std::string href("http://"+my_addr);
			if (my_port.c_str() && *my_port.c_str()) {
				href.append(":").append(my_port.c_str());
				my_addr.append(":").append(my_port.c_str());
			}
			visitor.status("200 OK");
			make_element(visitor.content_type(xeno::type::HTML).content(), "a").attr("href", href).text(my_addr);
		}
	}
	else {
		visitor.blame("500 Internal Server Error");
		throw my_addr.c_str();
	}
}

bool whatsmyip::get_ip_address(std::string& addr) const
{
	TRACE("whatsmyip(%p)::get_ip_address HOST:PORT [%s:80]\n", this, host.c_str());
	boost::asio::ip::tcp::iostream client("whatsmyip.de", "80");
	// std::iostream client(0);
	TRACE("BUILD HTTP REQUEST...\n");
	if (client) {
		TRACELN("BUILD HTTP REQUEST...");
		client << "GET / HTTP/1.1\r\n";
		client << "Host: " << host.c_str() << "\r\n";
		client << "Accept: " << (content_type.defined() ? content_type.c_str() : "*/*") << "\r\n";
		client << "Accept-Charset: utf-8\r\n";
		client << "Accept-Encoding: identity\r\n";
		client << "User-Agent: " << CLIENT_ID << "\r\n";
		client << "\r\n" << std::flush;
		TRACE("SENT HTTP REQUEST TO [%s:80]\n", host.c_str());
		// TODO: error handling / HTTP parsing
		if (client) {
			TRACELN("ABOUT TO PARSE LINES...");
			std::string line;
			// skip past HTTP headers
			while (std::getline(client, line) && line != "\r")
#ifndef NDEBUG
				std::cerr << " > " << line << std::endl;
#endif
				/* skip */ ;
			// the skip until the <h3> where the IP is
			while (std::getline(client, line) && !strstr(line.c_str(), "post-content"))
#ifndef NDEBUG
				std::cerr << " > " << line << std::endl;
#endif
				/* skip */;
			TRACELN("PARSED LINES...");
			if (line.size() && std::getline(client, line)) {
#ifndef NDEBUG
				std::cerr << "FOUND > " << line;
#endif
				std::size_t end = line.find("IP Adress: ");
				if (end != std::string::npos) {
					// SUCCESS!
					TRACELN("\nSUCCESS!");
					addr.assign(line.substr(end+11));
					end = addr.find('<');
					addr.erase(end);
					return true;
				}
			}
			// Catch all
			TRACELN("PARSE FAILED!");
			addr.assign(std::string("could not find address in page http://")+host.c_str());
			return false;
		}
	}
	// Catch all
	TRACELN("FAILED!");
	addr.assign(std::string("could not connect to ")+host.str());
	return false;
}

} /* namespace test */
} /* namespace xeno */
