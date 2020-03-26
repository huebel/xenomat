#ifndef XMLRPC_STRUCT_WRITER_H_
#define XMLRPC_STRUCT_WRITER_H_

#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include <xeno/trace.h>
#include <xeno/xeno_io.h>

namespace xeno {
namespace xmlrpc {

template <typename Container>
struct is_container : std::false_type { };

template <typename... Ts> struct is_container<std::set<Ts...> > : std::true_type { };
template <typename... Ts> struct is_container<std::list<Ts...> > : std::true_type { };
template <typename... Ts> struct is_container<std::vector<Ts...> > : std::true_type { };

template <typename T>
struct is_simple : std::integral_constant<
  bool,
  std::is_arithmetic<T>::value ||
  std::is_same<T,std::string>::value
> { };


template <typename T>
struct use_dashes : std::false_type {};

// An io_object for serializing C++ to XML-RPC

struct xmlrpc_struct_writer: xeno::io_object<xmlrpc_struct_writer>
{
	xmlrpc_struct_writer(xeno::element& struct_root, bool use_dashes = false)
	:	stack(struct_root)
	,	underscores_to_dashes(use_dashes)
	{
		assert(!strcmp(struct_root.qname(), "struct") && "xmlrpc_struct_writer must be placed on a 'struct' element");
	}

	template <typename T>
	inline
	const T io_text(const char* name, const T val) const
	{
//		TRACE("xmlrpc_struct_writer::io_text - %s\n", name);
		auto& member = stack.child("member");
		member.child("name").text(rpc_name(name));
		io_set_tag(member.child("value"), val);
		return val;
	}

	template <typename T>
	inline
	const T io_attr(const char* name, const T val) const
	{
		return io_text(name, val);
	}

	template <typename E>
	inline
	const E io_enum(const char* name, const E val) const
	{
		return static_cast<E>(io_text(name, static_cast<int>(val)));
	}


	template <typename Container>
	inline
	typename std::enable_if<is_container<Container>::value && is_simple<typename Container::value_type>::value,Container>::type
	io_list(const char* element_name, const char* container_name,
			const Container& source) const
	{
		auto& member = stack.child("member");
		member.child("name").text(rpc_name(element_name));
		auto& array_data = member.child("value").child("array").child("data");
		typename Container::const_iterator begin = std::begin(source);
		const typename Container::const_iterator end = std::end(source);
		while (begin != end) {
			io_set_tag(array_data.child("value"), *begin++);
		}
		return std::move(source);
	}

	template <typename Container>
	inline
	typename std::enable_if<is_container<Container>::value && !is_simple<typename Container::value_type>::value,Container>::type
	io_list(const char* element_name, const char* container_name,
		const Container& source) const
	{
		auto& member = stack.child("member");
		member.child("name").text(rpc_name(element_name));
		auto& array_data = member.child("value").child("array").child("data");
		typename Container::const_iterator begin = std::begin(source);
		const typename Container::const_iterator end = std::end(source);
		while (begin != end) {
			this_io_t(array_data.child("value").child("struct"),
					use_dashes<typename Container::value_type>::value).apply(*begin++);
		}
		return std::move(source);
	}

	template <typename T>
	xmlrpc_struct_writer io_part(const char* name, const T& /* part_object */) const {
		auto& member = stack.child("member");
		member.child("name").text(rpc_name(name));
		auto& struct_data = member.child("value").child("struct");
		return xmlrpc_struct_writer(struct_data, use_dashes<T>::value);
	}

private:

	void io_set_tag(xeno::element& tag, const std::string& val) const {
		tag.child("string").text(val);
	}

	void io_set_tag(xeno::element& tag, int32_t val) const {
		tag.child("i4").text(std::to_string(val));
	}

	void io_set_tag(xeno::element& tag, unsigned val) const {
		tag.child("unsigned").text(std::to_string(val));
	}

	void io_set_tag(xeno::element& tag, double val) const {
		tag.child("double").text(std::to_string(val));
	}

	void io_set_tag(xeno::element& tag, bool val) const {
		tag.child("boolean").text(val ? "1" : "0");
	}

	xeno::element& stack;
	const bool underscores_to_dashes;

	inline
	std::string rpc_name(const char* name) const {
		if (underscores_to_dashes) {
			std::string tmp(name);
			std::replace(tmp.begin(), tmp.end(), '_', '-');
			return tmp;
		}
		else {
			return std::string(name);
		}
	}

};

} // namespace xmlrpc
} // namespace xeno

#endif /* XMLRPC_STRUCT_WRITER_H_ */
