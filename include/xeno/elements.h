#ifndef XENO_ELEMENTS_H_
#define XENO_ELEMENTS_H_

#include "xeno.h"
#include "document.h"

namespace xeno XENO_EXPORT {

inline xeno::context& move_elements(xeno::context& dst, const xeno::contens& src)
{
	xeno::contens nodes(src);
	while (!nodes.empty()) {
		nodes = nodes.skip_until<xeno::element>();
		if (!nodes.empty()) {
			dst.push_back(nodes.head());
			nodes = nodes.tail();
		}
	}
	return dst;
}

template <typename Predicate>
inline xeno::context& move_elements_if(xeno::context& dst, const xeno::contens& src, Predicate predicate)
{
	xeno::contens nodes(src);
	while (!nodes.empty()) {
		nodes = nodes.skip_until<xeno::element>();
		if (!nodes.empty()) {
			if (predicate(nodes.head())) {
				dst.push_back(nodes.head());
			}
			nodes = nodes.tail();
		}
	}
	return dst;
}

template <typename Predicate>
inline xeno::context& find_child_element(xeno::context& src, Predicate predicate)
{
	xeno::contens nodes(src);
	while (!nodes.empty()) {
		nodes = nodes.skip_until<xeno::element>();
		if (!nodes.empty()) {
			if (predicate(nodes.head())) {
				return nodes.head();
			}
			nodes = nodes.tail();
		}
	}
	return src;
}

template <typename Predicate>
inline const xeno::context& find_child_element(const xeno::context& src, Predicate predicate)
{
	xeno::contens nodes(src);
	while (!nodes.empty()) {
		nodes = nodes.skip_until<xeno::element>();
		if (!nodes.empty()) {
			if (predicate(nodes.head())) {
				return nodes.head();
			}
			nodes = nodes.tail();
		}
	}
	return src;
}

//template <>
//inline xeno::context& find_child_element(xeno::context& src, const std::string& name)
//{
//	return xeno::find_child_element<>(src, [&name](const xeno::context& e) {
//		return name == e.qname();
//	});
//}
//
//template <>
//inline const xeno::context& find_child_element(const xeno::context& src, const std::string& name)
//{
//	return find_child_element(src, [&name](const xeno::context& e){
//		return name == e.qname();
//	});
//}
//
//template <>
//inline xeno::context& find_child_element(xeno::context& src, const char* const name)
//{
//	return find_child_element(src, [&name](const xeno::context& e){
//		return 0 == ::strcmp(name,e.qname());
//	});
//}

template <typename ElementAction>
inline xeno::context& for_each_element(xeno::context& s, ElementAction action)
{
	xeno::contens nodes(s);
	while (!nodes.empty()) {
		nodes.skip_until<xeno::element>();
		if (!nodes.empty()) {
			action(nodes.head());
			nodes = nodes.tail();
		}
	}
	return s;
}

template <typename ElementAction>
inline const xeno::context& for_each_element(const xeno::context& s, ElementAction action)
{
	xeno::contens nodes(s);
	while (!nodes.empty()) {
		nodes.skip_until<xeno::element>();
		if (!nodes.empty()) {
			action(nodes.head());
			nodes = nodes.tail();
		}
	}
	return s;
}

inline xeno::context& documentElement(xeno::context& data) {
	xeno::contens children(data);
	children.skip_until<xeno::element>();
	return children.empty() ? data : children.head();
}

inline const xeno::context& documentElement(const xeno::context& data) {
	xeno::contens children(data);
	children.skip_until<xeno::element>();
	return children.empty() ? data : children.head();
}

} // namespace xeno


#endif//XENO_ELEMENTS_H_
