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

	template <typename T>
	const T io_text(const char* name, const T val) const
	{
//		TRACE("xmlrpc_struct_writer::io_text - %s\n", name);
		auto& member = stack.child("member");
		member.child("name").text(name);
		io_set_tag(member.child("value"), val);
		return val;
	}

	template <typename Container>
	Container io_list(const char* element_name, const char* container_name,
		Container& source) const
	{
		auto& member = stack.child("member");
		member.child("name").text(element_name);
		auto& array_data = member.child("value").child("array").child("data");
		typename Container::const_iterator begin = std::begin(source);
		const typename Container::const_iterator end = std::end(source);
		while (begin != end) {
			io_set_tag(array_data.child("value"), *begin++);
		}
		return std::move(source);
	}

private:

	void io_set_tag(xeno::element& tag, const std::string& val) const {
		tag.child("string").text(val);
	}

	void io_set_tag(xeno::element& tag, int32_t val) const {
		tag.child("i4").text(std::to_string(val));
	}

	void io_set_tag(xeno::element& tag, bool val) const {
		tag.child("boolean").text(val ? "1" : "0");
	}

	xeno::element& stack;

};


#endif /* XMLRPC_STRUCT_WRITER_H_ */
