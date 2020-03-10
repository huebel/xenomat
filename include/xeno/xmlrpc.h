/*
 * xmlrpc.h
 *
 *  Created on: 07.02.2015
 *      Author: Peter Hübel
 */

#ifndef XENO_XMLRPC_H_
#define XENO_XMLRPC_H_

#include <map>
#include <string>
#include <vector>
#include <type_traits>

#include <xeno/document.h>

#define URDL_HEADER_ONLY
#define URDL_DISABLE_SSL
#include <urdl/url.hpp>
#include <urdl/read_stream.hpp>

#include "xmlrpc_struct_writer.h"

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
	template <typename T>
	void add_member(const char* name, const T& value);
private:
	struct_t(const xeno::element& struct_data);
	std::map<const std::string, value_t> values;
friend
	class value_t;
};

class value_t {
public:
	value_t(const xeno::element& v)
	:	value(v)
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
	std::string c_str() const {
		xeno::textvalue s("string", value, "");
		return s.c_str();
	}
	std::string get_string() const {
		xeno::textvalue s("string", value, "");
		return s.str();
	}
	int get_int() const {
		xeno::textvalue s("int", value, "0");
		return std::atoi(s.c_str());
	}
	bool get_boolean() const {
		xeno::textvalue s("boolean", value);
		assert(s.defined());
		assert(s.str().length()==1);
		return *s.c_str()=='1';
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
//	TRACE("xmlrpc::struct[%s]\n", name);
	return values.at(name);
}

inline
bool struct_t::has_member(const char* name) const {
//	TRACE("xmlrpc::struct has_member(%s)\n", name);
	return values.find(name) != values.end();
}

class method;

class endpoint {
public:
	endpoint(boost::asio::io_service& ios, const std::string& endpoint)
	:	ios(ios)
	,	url(endpoint)
	{}
	boost::system::error_code call(method& m, boost::system::error_code& ec);
private:
	boost::asio::io_service& ios;
	urdl::url url;
};

class method {
public:
	method(const std::string& methodName, size_t stack_size = 1024<<4)
	:	stack(stack_size)
	{
		auto& methodCall = make_xml_document(stack.content(), "methodCall");
		methodCall.elem("methodName").text(methodName);
		params = &methodCall.elem("params");
		fault.clear();
	}
	// simple types
	void add_param(const std::string& value) {
		param_value("string").text(value);
	}
	void add_param(const char* value) {
		param_value("string").text(value);
	}
	void add_param(int value) {
		param_value("i4").text(std::to_string(value));
	}
	void add_param(double value) {
		param_value("double").text(std::to_string(value));
	}
	void add_param(bool value) {
		param_value("boolean").text(value ? "1" : "0");
	}
	// <struct/>
	template <typename Struct, typename = typename std::enable_if<std::is_object<Struct>::value,void>::type>
	void add_param(const Struct& value) {
		xmlrpc_struct_writer(param_value("struct")).apply(value);
	}
	int error_code() const { return fault.faultCode; }
	const std::string& error_string() const { return fault.faultString; }
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
	struct {
		int faultCode;
		std::string faultString;
		void clear() { faultCode=0; faultString.clear(); }
	} fault;
	// Helper functions
	xeno::element& param_value(const char* type) {
		return params->child("param").child("value").child(type);
	}
friend
	class endpoint;
};

inline
boost::system::error_code endpoint::call(method& m, boost::system::error_code& ec) {

	urdl::read_stream is(ios);

	std::ostringstream payload; xeno::xml_output(payload, m.stack.content());
//	TRACE("xmlrpc::endpoint::call - payload:\n");
//	xeno::xml_output(std::cerr, m.stack.content()) << std::endl;

	m.fault.clear();
	ec.clear();

	is.set_option(urdl::http::request_method("POST"));
	is.set_option(urdl::http::request_content_type(xeno::type::XML));
	is.set_option(urdl::http::request_content(payload.str()));

	if (!is.open(url, ec)) {
//		TRACE("xmlrcp::endpoint::call - open url '%s'.\n", url.to_string().c_str());
		std::string returned_content(is.content_length(), 0);
		boost::asio::read(is, boost::asio::buffer(&returned_content[0], returned_content.size()));
//		std::cout << std::endl << returned_content << std::endl << std::endl;
		if (xeno::xml_parse(m.stack.content(), returned_content)) {
//			m.dump(std::cout);
			m.params = xeno::find_element(m.stack.content(), "methodResponse/params");
			if (!m.params) {
				auto* fault = xeno::find_element(m.stack.content(), "methodResponse/fault/value");
				if (fault) {
					struct_t f = value_t(*fault).get_struct();
					m.fault.faultCode = f["faultCode"].get_int();
					m.fault.faultString = f["faultString"].get_string();
				}
			}
		}
		else {
			TRACE("xmlrpc::endpoint::call - failed to parse response.\n");
			m.params = nullptr;
		}
		return ec;
	}
	else {
		TRACE("xmlrcp::endpoint::call - could not open url '%s'?\n", ec.message().c_str());
		m.params = nullptr;
		return ec;
	}
}


} // namespace xmlrpc
} // namespace xeno

#endif /* XENO_XMLRPC_H_ */
