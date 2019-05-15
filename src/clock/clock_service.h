/*
 * clock_service.h
 *
 *  Created: 20.07.2018
 *   Author: Peter Hübel
 */

#ifndef CLOCK_SERVICE_H_
#define CLOCK_SERVICE_H_

#include <xeno/service.h>

namespace xeno {

namespace test {

class clock_service : public xeno::service<clock_service, const xeno::element> {
public:
	clock_service(Context& origin);
	void invoke(xeno::contact& visitor, xeno::sequens& route);
private:
	xeno::attribute type, offset;
	void animation_css(xeno::document& root, const char* const wrap_tag = 0);
};

} // namespace test

} // namespace xeno

#endif /* CLOCK_SERVICE_H_ */
