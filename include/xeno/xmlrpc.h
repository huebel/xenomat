/*
 * xmlrpc.h
 *
 *  Created on: 07.02.2015
 *      Author: Peter Hübel
 */

#ifndef XENO_XMLRPC_H_
#define XENO_XMLRPC_H_

#include <string>
#include <vector>
#include <xeno/document.h>
#define URDL_HEADER_ONLY
#define URDL_DISABLE_SSL
#include <urdl/url.hpp>
#include <urdl/read_stream.hpp>

namespace xeno {
namespace xmlrpc {

class method;
class value_t;
class struct_t;

class array_t {
public:
	size_t size() const { return data.size(); }
	value_t operator[](size_t index) const;
private:
	array_t(const xeno::element& array_data)
	{
		assert(!strcmp(array_data.qname(), "data"));
		xeno::contens items(array_data);
		while (!items.skip_until<xeno::element>().empty()) {
			data.push_back(static_cast<const xeno::element*>(&items.head()));
			items = items.tail();
		}
	}
	std::vector<const xeno::element*> data;
friend
	class value_t;
};

class struct_t {
public:
	value_t operator[](const char* name) const;
	bool has_member(const char* name) const;
private:
	struct_t(const xeno::element& struct_data);
	std::map<const std::string, value_t> values;
friend
	class value_t;
};

class value_t {
public:
	value_t(const xeno::element& v)
	: value(v)
	{
		assert(!strcmp(v.qname(), "value"));
	}
	array_t get_array() const
	{
		auto* _array = find_element(value, "array/data");
		assert(_array);
		return array_t(*_array);
	}
	struct_t get_struct() const {
		auto* _struct = find_element(value, "struct");
		assert(_struct);
		return struct_t(*_struct);
	}
	std::string get_string() const {
		xeno::textvalue s("string", value);
		assert(s.defined());
		return s.str();
	}
private:
	const xeno::element& value;
};


inline
value_t array_t::operator[](size_t index) const {
	if (index < data.size()) return *data[index];
	throw std::range_error("xmlrpc::array: index out of bounds");
}


inline
struct_t::struct_t(const xeno::element& struct_data)
{
	assert(!strcmp(struct_data.qname(), "struct"));
	xeno::contens members(struct_data);
	while (!members.skip_until<xeno::element>().empty()) {
		auto& member = members.head();
		assert(!strcmp(member.qname(), "member"));
		auto* name = find_element(member, "name");
		auto* value = find_element(member, "value");
		if (name && value) {
			values.insert({
				xeno::textvalue("name", member).c_str(),
				value_t(*value)
			});
		}
		members = members.tail();
	}
}

inline
value_t struct_t::operator[](const char* name) const {
	TRACE("xmlrpc::struct[%s]\n", name);
	return values.at(name);
}

inline
bool struct_t::has_member(const char* name) const {
	TRACE("xmlrpc::struct has_member(%s)\n", name);
	return values.find(name) != values.end();
}

class method;

class endpoint {
public:
	endpoint(boost::asio::io_service& ios, const std::string& endpoint)
	:	ios(ios)
	,	url(endpoint)
	{}
	void call(method& m, boost::system::error_code& ec);
private:
	boost::asio::io_service& ios;
	urdl::url url;
};

class method {
public:
	method(const std::string& methodName)
	:	stack(1024<<4)
	{
		auto& methodCall = make_xml_document(stack.content(), "methodCall");
		methodCall.elem("methodName").text(methodName);
		params = &methodCall.elem("params");
	}
	template <typename T>
	void add_param(const T& value) {
		add_value(params->child("param").child("value"), value);
	}
	void add_value(xeno::element& param, const std::string& value) {
		param.child("string").text(value);
	}
	void add_value(xeno::element& param, int value) {
		param.child("i4").text(std::to_string(value));
	}
	void add_value(xeno::element& param, bool value) {
		param.child("boolean").text(value ? "1" : "0");
	}
	value_t get_result() {
		assert(params);
		auto* param = xeno::find_element(*params, "param/value");
		assert(param);
		return value_t(*param);
	}
	std::ostream& dump(std::ostream& os) {
		return xeno::xml_output(os, stack.content()) << std::endl;
	}
private:
	xeno::local_context stack;
	xeno::element* params;
friend
	class endpoint;
};

inline
void endpoint::call(method& m, boost::system::error_code& ec) {

	urdl::read_stream is(ios);

	std::ostringstream payload; xeno::xml_output(payload, m.stack.content());

//	TRACE("%s: payload:\n%s\n", __func__, payload.str().c_str());

	is.set_option(urdl::http::request_method("POST"));
	is.set_option(urdl::http::request_content_type(xeno::type::XML));
	is.set_option(urdl::http::request_content(payload.str()));

	if (!is.open(url, ec)) {
		TRACE("open url %s\n", url.to_string().c_str());
		std::string returned_content(is.content_length(), 0);
		boost::asio::read(is, boost::asio::buffer(&returned_content[0], returned_content.size()));
		std::cout << std::endl << returned_content << std::endl << std::endl;
		if (xeno::xml_parse(m.stack.content(), returned_content)) {
			m.dump(std::cout);
			m.params = xeno::find_element(m.stack.content(), "methodResponse/params");
		}
		else {
			m.params = nullptr;
		}
	}
	else {
		TRACE("could not open url? %s\n", ec.message().c_str());
		m.params = nullptr;
	}
}


} // namespace xmlrpc
} // namespace xeno

#endif /* XENO_XMLRPC_H_ */
