/*
 * document.h
 *
 *  Created on: 23.10.2013
 *      Author: Peter Hübel
 */

#ifndef XENO_DOCUMENT_H_
#define XENO_DOCUMENT_H_

#ifndef XENO_H
#include "xeno.h"
#endif

namespace xeno XENO_EXPORT {

class XENO_EXPORT element;

context& xml_preample(context& document, const char* stylesheet = 0);

element& make_element(context& document, const char* qname);

element& make_attribute(element& element, const char* name, const char* value);

context& make_text_element(context& document, const char* qname, const char* value);

document& make_text(document& document, const char* value);

element& make_text(element& context, const char* value);

const element* find_element(const context& root, const char* qname);
element* find_element(context& root, const char* qname);

bool local_call(const context& origin, const std::string& href);

class element : public context {
public:
	inline element& child(const std::string& qname) {
		return make_element(*this, qname.c_str());
	}
	inline element& attribute(const std::string& name, const std::string& value) {
		return make_attribute(*this, name.c_str(), value.c_str());
	}
	inline element& attr(const std::string& name, const std::string& value) {
		return attribute(name, value);
	}
	inline element& text(const std::string& value) {
		return make_text(*this, value.c_str());
	}
	element& text_element(const std::string& element, const std::string& value) {
		make_text_element(*this, element.c_str(), value.c_str());
		return *this;
	}
};

class XENO_EXPORT document : public context {
public:
	element& root();
	const element& root() const;
	document& text(const std::string& text);
	const std::string& text() const;
};

class XENO_EXPORT local_context {
public:
	local_context(std::size_t sz);
	~local_context();
	document& content() { return local; }
private:
	document& local;
};


inline element& xml_document(context& context, const char* qname, const char* stylesheet = 0)
{
	return make_element(xml_preample(context, stylesheet), qname);
}

// OUTPUT

std::ostream& json_output(std::ostream& os, const element& node);

} /* namespace xeno */

#endif /* XENO_DOCUMENT_H_ */
