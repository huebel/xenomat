/*
 * whatsmyip.h
 *
 *  Created on: 30.12.2013
 *      Author: Peter Hübel
 */

#ifndef WHATSMYIP_H_
#define WHATSMYIP_H_

#include <xeno/service.h>

namespace xeno {
namespace test {

class whatsmyip : public xeno::service<whatsmyip, const xeno::element>
{
public:
	whatsmyip(Context& origin);
	virtual ~whatsmyip();
	virtual void invoke(xeno::contact& visitor, xeno::sequens& route);
private:
	// state
	const xeno::attribute host, my_port, content_type;
	// helpers
	bool get_ip_address(std::string& addr) const;
};

} /* namespace test */
} /* namespace xeno */

#endif /* WHATSMYIP_H_ */
