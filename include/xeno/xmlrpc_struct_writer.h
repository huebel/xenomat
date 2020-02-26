#ifndef XMLRPC_STRUCT_WRITER_H_
#define XMLRPC_STRUCT_WRITER_H_

#include <xeno/trace.h>
#include <xeno/xeno_io.h>

struct xmlrpc_struct_writer: xeno::io_object<xmlrpc_struct_writer>
{
	xmlrpc_struct_writer(xeno::element& struct_root)
	:	stack(struct_root)
	{
		assert(!strcmp(struct_root.qname(), "struct") && "xmlrpc_struct_writer must be placed on a 'struct' element");
	}

//	template <typename T>
//	inline const T& io_attr(const char* name, const T& val)
//	{
//		std::string value(boost::lexical_cast<std::string>(val));
//		current().attr(name, value);
//		return val;
//	}
//
//	template <typename T>
//	inline const T& io_attr_nul(const char* name, const T& val)
//	{
//		if (!xeno::singularity<T>::match(val)) {
//			std::string value(boost::lexical_cast<std::string>(val));
//			current().attr(name, value);
//		}
//		return val;
//	}
//
//	template <typename T>
//	inline const T& io_attr_def(const char* name, const T& val, const T& /*def*/)
//	{
//		std::string value(boost::lexical_cast<std::string>(val));
//		current().attr(name, value);
//		return val;
//	}
//
//	const std::string& io_attr(const char* name, const std::string& str)
//	{
//		current().attr(name, str);
//		return str;
//	}
//
//	const std::string& io_attr_nul(const char* name, const std::string& str)
//	{
//		if (!xeno::singularity<std::string>::match(str)) {
//			current().attr(name, str);
//		}
//		return str;
//	}
//
//	const std::string& io_attr_def(const char* name, const std::string& str, const std::string& /*def*/)
//	{
//		current().attr(name, str);
//		return str;
//	}
//
//	template <typename E, typename IO_ENUM_TRAITS = xeno::io_enum_traits<E> >
//	const E& io_enum(const char* name, const E& val)
//	{
//		std::string value(IO_ENUM_TRAITS::enum_to_string(val));
//		current().attr(name, value);
//		return val;
//	}
//
//	template <typename E, typename IO_ENUM_TRAITS = xeno::io_enum_traits<E> >
//	const E io_enum_text(const char* name, const E& val)
//	{
//		TRACE("xmlrpc_struct_writer::io_enum_text - %s [tag=%d]\n", name, tag);
//		CONFD_SET_TAG_ENUM_VALUE(next(), tag, val);
//		return val;
//	}
//
//	template <typename T>
//	const T& io_text(const T& val)
//	{
//		std::string value(boost::lexical_cast<std::string>(val));
//		current().text(value);
//		return val;
//	}

	template <typename T>
	const T& io_text(const char* name, const T& val)
	{
//		TRACE("xmlrpc_struct_writer::io_text - %s\n", name);
		auto& member = stack.child("member");
		member.child("name").text(name);
		io_set_tag(member.child("value"), val);
		return val;
	}

	void io_set_tag(xeno::element& tag, const std::string& val) {
		tag.child("string").text(val);
	}

	void io_set_tag(xeno::element& tag, int32_t val) {
		tag.child("i4").text(std::to_string(val));
	}

	void io_set_tag(xeno::element& tag, bool val) {
		tag.child("boolean").text(val ? "1" : "0");
	}


//	template <typename T>
//	const T& io_text_def(const T& val, const T& def)
//	{
//		if (val != def) {
//			std::string value(boost::lexical_cast<std::string>(val));
//			current().text(value);
//		}
//		return val;
//	}
//
//	template <typename T>
//	const T& io_text_nul(const T& val)
//	{
//		if (!xeno::singularity<T>::match(val)) {
//			std::string value(boost::lexical_cast<std::string>(val));
//			current().text(value);
//		}
//		return val;
//	}

//	template <typename Container>
//	Container io_list(const char* element_name, const char* container_name, Container& source)
//	{
//		typename Container::const_iterator begin = std::begin(source);
//		const typename Container::const_iterator end = std::end(source);
//		TRACE("xmlrpc_struct_writer::io_list(%s,%s,[%p|%p])\n", element_name, container_name, &*begin, &*end);
//		const u_int32_t container_tag = name2tag(container_name);
//		CONFD_SET_TAG_XMLBEGIN(next(), container_tag, 0);
//		while (begin != end) {
//			apply(element_name, *begin++);
//		}
//		CONFD_SET_TAG_XMLEND(next(), container_tag, 0);
//		TRACE("xmlrpc_struct_writer::io_list(container=%s) - done\n", container_name);
//		return std::move(source);
//	}

private:

	xeno::element& stack;

};




#endif /* XMLRPC_STRUCT_WRITER_H_ */
