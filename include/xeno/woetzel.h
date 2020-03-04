/*
 * woetzel.h
 *
 *  Created on: 09.12.2012
 *      Author: Peter Hübel
 */

#ifndef WOETZEL_H_
#define WOETZEL_H_

#include <string>
#include <type_traits>

#include <boost/lexical_cast.hpp>

#include "xeno.h"

// CORE

namespace xeno XENO_NAMESPACE_EXPORT {

struct woetzel {};

template <class IO>
struct io_object : woetzel {

	typedef IO this_io_t;

	template<typename ObjectType>
	inline ObjectType& apply(ObjectType& object) const
	{
		std::allocator<ObjectType>().construct(
			&object, static_cast<const this_io_t&>(*this)
		);
		return object;
	}

	template<typename ObjectType>
	inline const ObjectType& apply(const ObjectType& object) const
	{
		std::allocator<ObjectType>().construct(
			&const_cast<ObjectType&>(object),
			static_cast<const this_io_t&>(*this)
		);
		return object;
	}

	template<typename ObjectType>
	inline ObjectType* apply(ObjectType* pointer) const
	{
		std::allocator<ObjectType>().construct(
			pointer, static_cast<const this_io_t&>(*this)
		);
		return pointer;
	}

	template <typename Container>
	inline Container io_std(Container c) const
	{
		return std::move(c);
	}

};

/**
 *  This template is used for mapping C++ enumerations to text
 *	in this case std::string.
 */

template <typename ENUM, typename TEXT = std::string>
struct io_enum_traits {
	typedef TEXT Text;
	typedef ENUM Enum;
	static inline const Text enum_to_string(const Enum e) {
		return boost::lexical_cast<Text>(static_cast<int>(e));
	}
	static inline bool enum_to_string(const Enum e, Text& text) {
		return text = boost::lexical_cast<Text>(static_cast<int>(e)), true;
	}
	static inline const Enum string_to_enum(const Text& s) {
		return static_cast<Enum>(boost::lexical_cast<int>(s));
	}
};

#define IO_CLASS(T) template<typename IO,\
		typename = typename std::enable_if<std::is_base_of<xeno::woetzel,IO>::value,IO>::type>\
		T(const IO& io)
#define IO_CLASS_IMPL(T) template<typename IO, typename WOETZEL_IO_DEFAULT>\
		T::T(const IO& io)

#define IO_BASE(B) 		    B(io)


#define IO_ATTR(m)          m(io.io_attr(#m,*(&m)))
#define IO_ATTR_NUL(m)      m(io.io_attr_nul(#m,*(&m)))
#define IO_ATTR_DEF(m,val)  m(io.io_attr_def(#m,*(&m),val))
#define IO_ATTR_OPT(m)      m(io.io_attr_def(#m,*(&m),*(&m)))
// For IO_ENUM you can make a specialisation of the io_enum_traits template above
#define IO_ENUM(m)          m(io.io_enum(#m,*(&m)))
#define IO_ENUM_TEXT(m)     m(io.io_enum_text(#m,*(&m)))
#define IO_TEXT(m)          m(io.io_text(#m,*(&m)))
#define IO_INNER(m)         m(io.io_text(*(&m)))
#define IO_INNER_NUL(m)     m(io.io_text_nul(*(&m)))
#define IO_INNER_DEF(m,val) m(io.io_text_def(*(&m),val))
#define IO_LINK(m)          m(io.io_link(#m,*(&m)))
#define IO_PART(m)          m(io.io_part(#m))
#define IO_LIST(e,c)        c(std::move(io.io_list(#e,#c,c)))
#define IO_ANON_LIST(c)  	c(std::move(io.io_list(#c,nullptr,c)))

#define IO_INIT {

#define IO_ARRAY(e,c)       io.io_list(#e, #c, &c[0], c+(sizeof(c)/sizeof(c[0])));

#define IO_END }


} // namespace xeno


#endif /* WOETZEL_H_ */
