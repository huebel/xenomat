/*
 * woetzel.h
 *
 *  Created on: 09.12.2012
 *      Author: Peter Hübel
 */

#ifndef WOETZEL_H_
#define WOETZEL_H_

#include <string>

#include <boost/lexical_cast.hpp>

#include "xeno.h"

// CORE

namespace xeno XENO_NAMESPACE_EXPORT {

template <class IO>
struct io_object {

	typedef IO this_io_t;

	template<typename ObjectType>
	inline ObjectType& apply(ObjectType& object)
	{
		return *new (&object) ObjectType(static_cast<this_io_t&>(*this));
	}

	template<typename ObjectType>
	inline const ObjectType& apply(const ObjectType& object)
	{
		return *new (const_cast<ObjectType*>(&object)) ObjectType(static_cast<this_io_t&>(*this));
	}

	template<typename ObjectType>
	inline ObjectType* apply(ObjectType* pointer)
	{
		return new (pointer) ObjectType(static_cast<IO&>(*this));
	}

//	void back() {}

	template <typename Container>
	inline Container io_std(Container c)
	{
		return c;
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
	static inline const Text enum_to_string(const Enum& e) {
		return boost::lexical_cast<TEXT>(static_cast<int>(e));
	}
	static inline bool enum_to_string(const Enum& e, Text& text) {
		return text = boost::lexical_cast<TEXT>(static_cast<int>(e)), true;
	}
	static inline const Enum string_to_enum(const Text& s) {
		return static_cast<Enum>(boost::lexical_cast<int>(s));
	}
};

#define IO_CLASS(T) template<typename IO> T(IO& io)
#define IO_CLASS_IMPL(T) template<typename IO> T::T(IO& io)

#define IO_BASE(B) 		    B(io)

#define IO_ATTR(m)          m(io.io_attr(#m,m))
#define IO_ATTR_NUL(m)      m(io.io_attr_nul(#m,m))
#define IO_ATTR_DEF(m,val)  m(io.io_attr_def(#m,m,val))
#define IO_ATTR_OPT(m)      m(io.io_attr_def(#m,m,m))
// For IO_ENUM you can make a specialisation of the io_enum_traits template above
#define IO_ENUM(m)          m(io.io_enum(#m,m))
#define IO_TEXT(m)          m(io.io_text(#m,m))
#define IO_INNER(m)         m(io.io_text(m))
#define IO_INNER_NUL(m)     m(io.io_text_nul(m))
#define IO_INNER_DEF(m,val) m(io.io_text_def(m,val))
#define IO_LINK(m)          m(io.io_link(#m,m))
#define IO_PART(m)          m(io.io_part(#m,m))
//#define IO_STD(c)  	        c(io.io_std(c))
#define IO_VECTOR(e,c)      c(std::move(io.io_list(#e,#c, c)))
#define IO_LIST(e,c)        IO_VECTOR(e,c)

#define IO_INIT {

#define IO_ARRAY(e,c)       io.io_list(#e, #c, &c[0], c+(sizeof(c)/sizeof(c[0])));

#define IO_END }


} // namespace xeno

#endif /* WOETZEL_H_ */
