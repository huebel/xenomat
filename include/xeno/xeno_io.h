/*
 * xeno_io.h
 *
 *  Created on: 08.12.2012
 *  Revised on: 02.03.2020
 *  ^   Author: Peter Hübel
 *  L_> Seriously
 */

#ifndef XENO_IO_H_
#define XENO_IO_H_

#include <cassert>

#include "trace.h"
#include "xeno.h"
#include "document.h"
#include "woetzel.h"
#include "singularity.h"

#include <boost/lexical_cast.hpp>

/* Extend lexical_cast to handle bool */
namespace boost {

    template<>
    inline bool lexical_cast<bool, std::string>(const std::string& arg) {
        std::istringstream ss(arg);
        bool b;
        ss >> std::boolalpha >> b;
        return b;
    }

    template<>
    inline std::string lexical_cast<std::string, bool>(const bool& b) {
        std::ostringstream ss;
        ss << std::boolalpha << b;
        return ss.str();
    }

}

#include <vector>

namespace xeno XENO_NAMESPACE_EXPORT {

// Build a xeno:context from a C++ object tree

struct context_writer: io_object<context_writer> {

	inline context_writer(xeno::element& root)
	:	stack(root)
	{
		scope.push_back(&root);
	}

	inline ~context_writer()
	{
		back();
	}

	template <typename T>
	inline T& apply(const char* name, T& object) const
	{
		xeno::element& target = current().child(name);
		context_writer w(target);
		return w.io_object<context_writer>::apply(object);
	}

	template <typename T>
	inline T& apply(T& object) const
	{
		xeno::element& target = current();
		context_writer w(target);
		return w.io_object<context_writer>::apply(object);
	}

	template <typename T>
	inline const T io_attr(const char* name, const T val) const
	{
		std::string value(boost::lexical_cast<std::string>(val));
		current().attr(name, value);
		return val;
	}

	template <typename T>
	inline const T io_attr_nul(const char* name, const T val) const
	{
		if (!singularity<T>::match(val)) {
			std::string value(boost::lexical_cast<std::string>(val));
			current().attr(name, value);
		}
		return val;
	}

	template <typename T>
	inline const T io_attr_def(const char* name, const T val, const T& /*def*/) const
	{
		std::string value(boost::lexical_cast<std::string>(val));
		current().attr(name, value);
		return val;
	}

	template <typename E, typename IO_ENUM_TRAITS = io_enum_traits<E> >
	const E io_enum(const char* name, const E val) const
	{
		std::string value(IO_ENUM_TRAITS::enum_to_string(val));
		current().attr(name, value);
		return val;
	}

	template <typename E, typename IO_ENUM_TRAITS = io_enum_traits<E> >
	const E io_enum_text(const char* name, const E val) const
	{
		std::string value(IO_ENUM_TRAITS::enum_to_string(val));
		current().child(name).text(value);
		return val;
	}

	template <typename T>
	const T io_text(const T val) const
	{
		std::string value(boost::lexical_cast<std::string>(val));
		current().text(value);
		return val;
	}

	template <typename T>
	const T io_text(const char* name, const T val) const
	{
		std::string value(boost::lexical_cast<std::string>(val));
		current().child(name).text(value);
		return val;
	}

	template <typename T>
	const T io_text_def(const T val, const T def) const
	{
		if (val != def) {
			std::string value(boost::lexical_cast<std::string>(val));
			current().text(value);
		}
		return val;
	}

	template <typename T>
	const T io_text_nul(const T val) const
	{
		if (!singularity<T>::match(val)) {
			std::string value(boost::lexical_cast<std::string>(val));
			current().text(value);
		}
		return val;
	}

	template <typename T>
	T* io_link(const char* name, T* ptr) const
	{
		if (ptr) {
			return &apply(name, *ptr);
		}
		else {
			return 0;
		}
	}

	this_io_t io_part(const char* name) const
	{
		xeno::element& target = current().child(name);
		return this_io_t(target);
	}

	template <typename ForwardIterator>
	void io_list(const char* element_name, const char* container_name,
		ForwardIterator begin, ForwardIterator end) const
	{
//		TRACE("W::io_list(%s,%s,begin=%p,end=%p)\n", element_name, container_name, &*begin, &*end);
		if (container_name) scope.push_back(&current().child(container_name));
		while (begin != end) {
			apply(element_name, *begin++);
		}
		if (container_name) scope.pop_back();
	}

	template <typename Container>
	Container io_list(const char* element_name, const char* container_name,
		Container& source) const
	{
		typename Container::const_iterator begin = std::begin(source);
		const typename Container::const_iterator end = std::end(source);
//		TRACE("W::io_list(%s,%s,[%p|%p])\n", element_name, container_name, &*begin, &*end);
		if (container_name) scope.push_back(&current().child(container_name));
		while (begin != end) {
			apply(element_name, *begin++);
		}
		if (container_name) scope.pop_back();
		return std::move(source);
	}

	xeno::element& current() const
	{
		assert(scope.size());
		return *(scope.back());
	}

	void back()
	{
		assert(scope.size());
		scope.pop_back();
	}

	const xeno::element& stack;
	mutable std::vector<xeno::element*> scope;

};



struct context_reader: io_object<context_reader> {

	context_reader(const xeno::element& root)
	:	stack(root)
	{
//		TRACE("R::context_reader @ %s \n", root.qname());
		scope.push_back(&root);
	}

	template <typename T>
	T& apply(const char* name, T& object) const
	{
		const xeno::element* target = xeno::find_element(current(), name);
//		TRACE("R::apply('%s', object) @ %s %s\n", name, current().qname(), target ? "found" : "FAIL!");
		assert(target);
		context_reader r(*target);
		return r.io_object<context_reader>::apply(object);
	}

	template <typename T>
	T& apply(T& object) const
	{
		const xeno::element* target = &current();
		assert(target);
		context_reader r(*target);
		return r.io_object<context_reader>::apply(object);
	}

	template <typename T>
	T io_attr(const char* name, const T& /*val*/) const
	{
		// TODO: make the IO_ATTR macro provide the @
		std::string attr_name("@");
		attr_name.append(name);
		xeno::attribute attr(attr_name.c_str(), current());
//		TRACE("R::io_attr(@%s,val) @ %s %s [%s]\n", name, current().qname(), attr.defined() ? "found" : "FAIL!", typeid(T).name());
		assert(attr.defined() && !attr.empty());
		T value = boost::lexical_cast<T>(attr.str());
		return value;
	}

	template <typename T>
	const T io_attr_nul(const char* name, const T& /*val*/) const
	{
		std::string attr_name("@");
		attr_name.append(name);
		xeno::attribute attr(attr_name.c_str(), current());

		if (attr.defined()) {
			try {
				T value = boost::lexical_cast<T>(attr.str());
				return value;
			}
			catch (boost::bad_lexical_cast& ex) {
				TRACELN("ERROR: conversion failed, using default");
			}
		}
		return singularity<T>::undefined;
	}

	template <typename T>
	const T io_attr_def(const char* name, const T& /*val*/, const T def) const
	{
		std::string attr_name("@");
		attr_name.append(name);
		xeno::attribute attr(attr_name.c_str(), current());
//		TRACE("R::io_attr_def(@%s,val) @ %s %s\n", name, current().qname(), attr.defined() ? "found" : "FAIL!");
		if (attr.defined() && !attr.empty()) {
			return boost::lexical_cast<T>(attr.str());
		}
		return def;
	}

	template <typename E, typename IO_ENUM_TRAITS = io_enum_traits<E> >
	const E io_enum(const char* name, const E /*val*/) const
	{
		std::string attr_name("@");
		attr_name.append(name);
		xeno::attribute attr(attr_name.c_str(), current());
//		TRACE("R::io_enum(@%s,val) @ %s %s\n", name, current().qname(), attr.defined() ? attr.c_str() : "FAIL!");
		assert(attr.defined() && !attr.empty());
		E value = IO_ENUM_TRAITS::string_to_enum(attr.c_str());
		return value;
	}

	template <typename E, typename IO_ENUM_TRAITS = io_enum_traits<E> >
	const E io_enum_text(const char* name, const E /*val*/) const
	{
		xeno::textvalue text(name, current());
		assert(text.defined() && !text.empty());
		E value = IO_ENUM_TRAITS::string_to_enum(text.c_str());
		return value;
	}

	template <typename T>
	const T io_text(const char* name, const T& /*val*/) const
	{
//		TRACE("R::io_text('%s')\n", name);
		xeno::textvalue text(name, current());
		assert(text.defined() && !text.empty());
		T value = boost::lexical_cast<T>(text.str());
		return value;
	}

	template <typename T>
	const T io_text(const T& /*val*/) const
	{
//		TRACELN("R::io_text(.)");
		xeno::textvalue text(current());
		assert(text.defined() && !text.empty());
		T value = boost::lexical_cast<T>(text.str());
		return value;
	}

	template <typename T>
	const T io_text_def(const T& /*val*/, const T& def) const
	{
//		TRACELN("R::io_text_def");
		xeno::textvalue text(current());
		if (text.defined() && !text.empty()) {
			try {
				T value = boost::lexical_cast<T>(text.str());
				return value;
			}
			catch (boost::bad_lexical_cast& ex) {
				TRACELN("ERROR: conversion failed, using default");
			}
		}
		return def;
	}

	template <typename T>
	const T io_text_nul(const T& /*val*/) const
	{
//		TRACELN("R::io_text_nul");
		xeno::textvalue text(current());
		if (text.defined() && !text.empty()) {
			try {
				T value = boost::lexical_cast<T>(text.str());
				return value;
			}
			catch (boost::bad_lexical_cast& ex) {
				TRACELN("ERROR: conversion failed, using default");
			}
		}
		return singularity<T>::undefined;
	}

	template <typename T>
	inline T* io_link(const char* name, T* ptr) const
	{
		delete ptr;
		const xeno::element* target = xeno::find_element(current(), name);
		if (target && !target->empty()) {
			ptr = static_cast<T*>(static_cast<void*>(new char[sizeof(T)]));
			::memset(ptr, 0, sizeof(T));
			context_reader r(*target);
			return &apply(name, *ptr);
		}
		return 0;
	}

	context_reader io_part(const char* name) const
	{
		const xeno::element* target = xeno::find_element(current(), name);
		assert(target);
		return context_reader(*target);
	}

	template <typename ForwardIterator>
	void io_list(const char* element_name, const char* container_name,
		ForwardIterator begin, const ForwardIterator end)
	{
		xeno::contens list(current());
		while (!list.empty() && begin != end) {
			list.skip_until<xeno::element>();
			if (!list.empty() && !::strcmp(list.head().qname(), element_name)) {
//				TRACE("R::io_list(bound): %s\n", element_name);
				context_reader(static_cast<const xeno::element&>(list.head())).apply(*begin);
				++begin;
			}
			list = list.tail();
		}
	}

	template <typename Container, typename value_type = typename Container::value_type>
	Container io_list(const char* element_name, const char* container_name,
		Container& target) const
	{
		const xeno::element* source = nullptr;
//		TRACE("R::io_list('%s','%s')@%s:\n", element_name, container_name, current().qname());
		if (container_name && (source = xeno::find_element(current(), container_name))) {
			scope.push_back(source);
			assert(!::strcmp(current().qname(), container_name));
		}
		Container container;
		xeno::contens list(current());
		while (!list.empty()) {
			list.skip_until<xeno::element>();
			if (!list.empty() && !::strcmp(list.head().qname(), element_name)) {
//				TRACE("R::io_list(unbound): %s\n", element_name);
				scope.push_back(static_cast<const xeno::element*>(&list.head()));
				container.emplace_back(*this);
				scope.pop_back();
			}
			list = list.tail();
		}
		if (source) scope.pop_back();
//		TRACE("R::io_list found %d elements\n", container.size());
		return std::move(container);
	}

	inline const xeno::element& current() const
	{
//		TRACE("R::current: %s\n", scope.back()->qname());
		assert(scope.size());
		return *(scope.back());
	}

	inline void back() const // should shadow
	{
		assert(scope.size());
		scope.pop_back();
	}

	const xeno::element& stack;
	mutable std::vector<const xeno::element*> scope;

};

} // namespace xeno

#endif /* XENO_IO_H_ */
