/*
 * xeno.h
 *
 *  Created on: 20.10.2013
 *      Author: Peter Hübel
 */

#ifndef XENO_H_
#define XENO_H_

#include <cstdio> // for the macros
#include <string>
#include <cstring>

#include <iostream>

// This is the version
#define XENO_VERSION "2.8.1-CE" // @suppress("Obsolete object-like macro")

// This is the platform
#if defined(XENO_WINDOWS)
#define XENO_PLATFORM "Windows" // @suppress("Obsolete object-like macro")
#elif defined(XENO_DARWIN)
#define XENO_PLATFORM "Mac OSX" // @suppress("Obsolete object-like macro")
#else
#define XENO_PLATFORM "Linux" // @suppress("Obsolete object-like macro")
#endif


/**
 *  TODO: for letting templates serve a structure for accessing on compile time
 *
 *

 namespace xeno {

 typedef const char* str;

 template <char car, const char CDR[]>
 struct seq : seq<CDR[0], CDR+1> {

 };

 }//namespace xeno

 *
**/

// Export all symbols in 'xeno' namespace
#if !defined(XENO_EXPORT)

#if defined(XENO_CORE_BUILD)

#ifdef XENO_DARWIN
#define XENO_EXPORT __attribute__ ((weak)) __attribute__ ((visibility ("default")))
#else
#define XENO_EXPORT __attribute__ ((visibility ("default")))
#endif

#else

#ifdef XENO_DARWIN
#define XENO_EXPORT /* __attribute__ ((visibility ("hidden"))) */
#else
#define XENO_EXPORT /* __attribute__ ((visibility ("hidden"))) */
#endif

#endif

#endif

#ifdef XENO_DARWIN
#define XENO_NAMESPACE_EXPORT
#else
#define XENO_NAMESPACE_EXPORT XENO_EXPORT
#endif

namespace xeno XENO_NAMESPACE_EXPORT {

//// Constants for MIME types
//extern const char * const NOP;

// Constants for MIME types
// @suppress("Use std::array instead of C-Array.")
struct XENO_EXPORT type {
	XENO_EXPORT static const char ANY[];
	XENO_EXPORT static const char TEXT[];
	XENO_EXPORT static const char CSS[];
	XENO_EXPORT static const char XSL[];
	XENO_EXPORT static const char XML[];
	XENO_EXPORT static const char HTML[];
	XENO_EXPORT static const char SVG[];
	XENO_EXPORT static const char ECMA[];
	XENO_EXPORT static const char BLOB[];
};

struct XENO_EXPORT action;

struct XENO_EXPORT contens;
// like a <list><a/><b/><c/></list>, has a size of 4 cells.

struct XENO_EXPORT sequens;
// is like a list too, but ordered in both depth and width
// like <list><a><b><c/></b></a></list> still only 4 cells.

struct XENO_EXPORT contact;

struct XENO_EXPORT context
{
	// type of function for generating services
	typedef action* (*creator)(context& origin);
	// the qualified name, like it is described in the XML of the blueprint
	const char* qname() const;
	// contens
	bool empty() const;
	context& push_back(const context& source);
	context& push_back(const contens& source);
	context& push_back(const sequens& source);
	// Not copy- or assignable
	context() {};
	context(const context&) = delete;
	const context& operator=(const context&) = delete;
};

struct XENO_EXPORT contens
{
	// This structure can enumerate the direct descendant contexts (plural!?) of its origin.
	// The origin of a 'contens' (here implemented as 'this_context') is the 'context' which
	// wraps it.
	explicit contens(const context& origin);
	inline contens(const contens& rhs)
	:	this_context(rhs.this_context)
	,	next_sibling(rhs.next_sibling)
	{}
	inline bool empty() const { return next_sibling == end(); }
	context& head();
	const context& head() const;
	contens tail() const;
    template <typename CONTEXT>
    contens& skip_while();
	template <typename CONTEXT>
	contens& skip_until();
private:
	const context* end() const;
	const context* this_context;
	mutable const context* next_sibling;
friend
	struct context;
};

struct XENO_EXPORT element;

template <>
xeno::contens& xeno::contens::skip_until<xeno::element>();

struct XENO_EXPORT sequens
{
	explicit sequens(const context& origin);
	bool empty() const { return first == last; }
	const std::string& heading() const;
	bool heading(const std::string& next) const;
	const context& head() const;
	sequens tail() const;
	std::size_t length() const;
// TODO: mutators
//	sequens& push_back(const std::string& next);
//	sequens& push_back(const sequens& tail);
private:
	sequens(const context* first, const context* last);
	const context* first;
	const context* last;
};

struct XENO_EXPORT context_object
{
	operator const std::string&() const; // will throw if !defined
	operator std::string&(); // ditto
	inline const std::string str() const {
		return c_str();
	}
	const char* c_str() const;
	inline bool defined() const {
		return _value != 0;
	}
	bool empty() const;
	inline bool operator==(const char* value) const {
		const char* v = c_str();
		return v ? !strcmp(v, value) : false;
	}
	inline bool operator!=(const char* value) const {
		return !operator==(value);
	}
protected:
	context_object(const char* def) : _value(0), _default(def) {}
	context_object(const context_object&) = delete;
	context_object(const context_object&&) = delete;
	context* _value;
	const char* const _default;
};

inline std::ostream& operator<<(std::ostream& os, const context_object& object) {
  return os << object.c_str(); // @suppress("Use different overload that can take std::string directly.")
}

inline bool operator==(const std::string& str, const context_object& object) {
	return object.operator==(str.c_str());
}

inline bool operator==(const context_object& object, const std::string& str) {
	return object.operator==(str.c_str());
}

inline bool operator!=(const std::string& str, const context_object& object) {
	return object.operator!=(str.c_str());
}

inline bool operator!=(const context_object& object, const std::string& str) {
	return object.operator!=(str.c_str());
}

struct XENO_EXPORT attribute : context_object {

	inline attribute(const char path[], const context& origin, const char* def = 0)
	:	context_object(def)
	{
		init(path, origin);
	}

	attribute& assign(const std::string& value);

protected:

	void init(const char* path, const context& origin);

};

struct XENO_EXPORT textvalue : context_object
{
	inline textvalue(const context& origin, const char* def = 0)
	:	context_object(def)
	{
		init(origin);
	}
	inline textvalue(const char path[], const context& origin, const char* def = 0)
	:	context_object(def)
	{
		init(path, origin);
	}
	textvalue& assign(const std::string& value);
	textvalue& append(const std::string& value);
	void clear();
protected:
	void init(const context& origin);
	void init(const char* path, const context& origin);
};

struct XENO_EXPORT document;

struct XENO_EXPORT contact : context
{
    // Test contact payload capabilities
	bool will_accept(const char* type) const;
	bool can_deliver(const char* type) const;
	bool will_destroy() const;
	// Error handling (very simple: blame the stranger :-)
	void blame(const char* status_code = 0);
	// Release the contact immediately
	void yield();
	// The origin of a contact is actually the transport layer
	const context& origin() const;
	// Inspect payload (will read_stream_data on demand)
	document& content();
	// Payload construction
	bool parse(const std::string& xml_source);
	contact& text(const char* string);
	contact& xml(const context& source);
	int content_length() const;
	// Get attributes for request (Content-Type and the HTTP Method are handled explicit)
	const char* attr(const char* name) const;
	bool sent_from(const char* user_agent) const;
	// I/O with the clients source
	bool read_stream_data(char* data, int size) const;
	std::streambuf* rdbuf() const;
	// Set attributes for reply (Content-Type and the HTTP Status Code are handled explicit)
	contact& attr(const char* name, const char* value);
	// NOTE: Set the content_type of a reply BEFORE providing the content,
	// so that a call to document() later on may put a DOCTYPE in the right place
	contact& content_type(const char* type);
	contact& status(const char* status);
	// Convenience methods for plain text documents
	template <const char* const TEXT, const char* const TYPE = xeno::type::TEXT>
	inline contact& accept_static_text(const char* status) {
		return content_type(TYPE).text(TEXT).status(status);
	}
	template <const char* const XML>
	inline contact& accept_static_xml(const char* status) {
		return accept_static_text<XML,xeno::type::XML>(status);
	}
	template <const char* const SCRIPT>
	inline contact& accept_static_script(const char* status) {
		return accept_static_text<SCRIPT,xeno::type::ECMA>(status);
	}
};

} // namespace xeno

#endif /* XENO_H_ */
