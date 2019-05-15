/*
 * clock_service.cpp
 *
 *  Created on: 20.07.2018
 *      Author: Peter Hübel
 */

//#undef NDEBUG
//#define _NETROPY_DUMP

#include <xeno/trace.h>
#include <xeno/document.h>
#include <xeno/elements.h>

#define INCBIN_OUTPUT_SECTION ".data"
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX
#include "incbin.h"
INCBIN(nojazz_swiss_clock, "../src/clock/nojazz_swiss_clock.svg"); // @suppress("Un- or ill-initialized variables")
INCBIN(station_clock, "../src/clock/station_clock.svg"); // @suppress("Un- or ill-initialized variables")

// A reinterpret_cast is needed
#define INCBIN_STRING(x) std::string(reinterpret_cast<const char*>(x##_data), x##_size)

#include "clock_service.h"

namespace xeno {

typedef test::clock_service clock_service;
XENO_REGISTER(test, clock, clock_service);

namespace test {

clock_service::clock_service(Context& origin)
: Origin(origin)
,	type("@type", origin)
,	offset("@offset", origin, "0")
{
  // Validate that offset (if defined) contains an integer
  // in the range -43200 to +43200 (that is +/- 12 hours).
  if (offset.defined()) {
  	int seconds = 65535;
    int result = sscanf(offset.c_str(), "%d", &seconds);
    if (result != 1 || seconds < -43200 || seconds > 43200) {
      throw std::invalid_argument("xeno:clock: offset must be an integer in the range +/- 43200 (seconds)");
    }
  }
}

static const char FULL_CLOCK_SVG_IMAGE[] =
	"<svg viewBox='0 0 300 300' xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'>"
	"<image x='15' y='15' width='270' height='270' xlink:href='station_clock.svg'/>"
	"<image x='0' y='0' width='300' height='300' xlink:href='nojazz_swiss_clock.svg'/>"
	"</svg>";

void clock_service::invoke(contact& visitor, sequens& route)
{
  TRACE("clock_service::invoke: %s\n", route.empty() ? "<clock>" : route.heading().c_str());

  if (!route.empty() && route.heading("clock")) route = route.tail();

  if (route.empty()) {
	visitor.accept_static_text<test::FULL_CLOCK_SVG_IMAGE, xeno::type::SVG>("200 OK");
    visitor.content_type(type.defined() ? type.c_str() : xeno::type::SVG).status("200 OK");
  }
  else if (route.heading("nojazz_swiss_clock.svg")) {
    xeno::document& doc = xeno::make_text(visitor.content(), reinterpret_cast<const char*>(nojazz_swiss_clock_data), nojazz_swiss_clock_size-14);
    animation_css(doc, "style");
    const xeno::context& style = xeno::find_child_element(origin(), "style");
    if (&style != &origin()) {
    	TRACELN("Adding some styles...");
    	doc.push_back(style);
    }
    else TRACELN("No styles were added in this clock...");
    xeno::make_text(doc, "</defs></svg>");
    visitor.content_type(xeno::type::SVG).status("200 OK");
  }
  else if (route.heading("station_clock.svg")) {
	visitor.content_type(xeno::type::SVG).status("200 OK").text(INCBIN_STRING(station_clock).c_str());
  }
  else if (route.heading("sync")) {
    route = route.tail();
    animation_css(visitor.content(), route.empty() ? 0 : route.heading().c_str());
    visitor.content_type(xeno::type::CSS).status("200 OK");
  }
  else {
	visitor.blame();
  }
}

void clock_service::animation_css(document& root, const char* const wrap_tag)
{
  std::time_t t = std::time(0); // get time now
  struct tm* now = localtime(&t);
  t = 60 * (60 * (now->tm_hour % 12) + now->tm_min) + now->tm_sec;
  if (offset.defined()) t = (86400 + t + std::atoi(offset.c_str())) % 86400;
  const unsigned hrs = t % (12 * 60 * 60), min = t % (60 * 60), sec = t % (60);
  static char const TIME_FORMAT[] = "#hrs{animation-delay:-%us;-webkit-animation-delay:-%us;}#min{animation-delay:-%us;-webkit-animation-delay:-%us;}#sec,#bouncer{animation-delay:-%us;-webkit-animation-delay:-%us;}";
  char buffer[sizeof (TIME_FORMAT) + (2 * 3 * 5)]; // @suppress("Un- or ill-initialized variables")
  int const buflen = ::sprintf(buffer, TIME_FORMAT, hrs, hrs, min, min, sec, sec);
  TRACE("Buffer: "); TRACELN(buffer);
  if (wrap_tag)
    root.element().child(wrap_tag).attr("type", xeno::type::CSS).text(buffer, buflen);
  else
    root.element().text(buffer, buflen);
}

} // namespace test

} // namespace xeno
