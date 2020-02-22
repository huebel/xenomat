/*
 * document.h
 *
 *  Created on: 23.10.2013
 *      Author: Peter Hübel
 */

#ifndef XENO_DOCUMENT_H_
#define XENO_DOCUMENT_H_

#ifndef XENO_H_
#include "xeno.h"
#endif

namespace xeno XENO_NAMESPACE_EXPORT {

struct XENO_EXPORT element;

context& make_xml_preample(context& document, const char* stylesheet = 0, const char* style_type = 0);

element& make_element(context& document, const char* qname) XENO_EXPORT;

element& make_attribute(element& element, const char* name, const char* value) XENO_EXPORT;

context& make_text_element(context& document, const char* qname, const char* value) XENO_EXPORT;

document& make_text(document& document, const char* value) XENO_EXPORT;
document& make_text(document& document, const char* buffer, size_t buflen) XENO_EXPORT;
document& make_text(document& document, const char* begin, const char* end) XENO_EXPORT;

element& make_text(element& context, const char* value) XENO_EXPORT;
element& make_text(element& origin, const char* buffer, size_t buflen) XENO_EXPORT;
element& make_text(element& origin, const char* begin, const char* end) XENO_EXPORT;

const element* find_element(const context& root, const char* qname) XENO_EXPORT;
element* find_element(context& root, const char* qname) XENO_EXPORT;

inline const element* find_element(const context& root, const std::string& qname) {
  return find_element(root, qname.c_str()); // @suppress("Use different overload that can take std::string directly.")
  // As the suppression says: CharWars is suggesting infinite recursion... :-P
}
inline element* find_element(context& root, const std::string& qname) {
  return find_element(root, qname.c_str()); // @suppress("Use different overload that can take std::string directly.")
  // As the suppression says: CharWars is suggesting infinite recursion... :-P
}

bool local_call(const context& origin, const std::string& href) XENO_EXPORT;

struct XENO_EXPORT element : public context {
	inline element& child(const std::string& qname) {
		return make_element(*this, qname.c_str());
	}
	inline element& elem(const std::string& qname) {
	    return child(qname);
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
    inline element& text(const char* buffer, size_t buflen) {
        return make_text(*this, buffer, buflen);
    }
	inline element& text_element(const std::string& qname, const std::string& text) {
		make_text_element(*this, qname.c_str(), text.c_str());
		return *this;
	}
	inline element& text_element(const char* qname, const char* text) {
		make_text_element(*this, qname, text);
		return *this;
	}
};

struct XENO_EXPORT document : public context {
	xeno::element& element();
	const xeno::element& element() const;
	document& text(const std::string& text);
	const std::string& text() const;
};

struct XENO_EXPORT local_context {
	local_context(std::size_t sz);
	~local_context();
	document& content() { return local; }
private:
	document& local;
};


inline element& make_xml_document(context& context, const char* document_element_name, const char* stylesheet = 0)
{
	return make_element(make_xml_preample(context, stylesheet), document_element_name);
}

inline element& make_xml_document(context& context, const std::string& document_element_name)
{
	return make_element(make_xml_preample(context), document_element_name.c_str());
}

bool xml_parse(context& context, const std::string& source) XENO_EXPORT;

// OUTPUT

size_t xml_length(const context& node, bool include_root = true) XENO_EXPORT;
std::ostream& xml_output(std::ostream& os, const context& node) XENO_EXPORT;

std::ostream& json_output(std::ostream& os, const context& node) XENO_EXPORT;

} /* namespace xeno */

#endif /* XENO_DOCUMENT_H_ */
