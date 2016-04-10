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

#include <boost/noncopyable.hpp>

// This is the version

#define XENO_VERSION "2.4.8"

// Export all symbols in 'xeno' namespace
#define XENO_EXPORT __attribute__ ((visibility ("default")))

namespace xeno XENO_EXPORT {

// Constants for MIME types
extern const char * const NOP;

// Constants for MIME types
struct XENO_EXPORT type {
	static const char ANY[];
	static const char TEXT[];
	static const char CSS[];
	static const char XML[];
	static const char HTML[];
	static const char SVG[];
	static const char ECMA[];
	static const char BLOB[];
};

struct XENO_EXPORT action; // rename to reactor

struct XENO_EXPORT client; // rename to contact

struct XENO_EXPORT context : public boost::noncopyable
{
	// type of function for generating services
	typedef action* (*creator)(context& origin);
	// the qualified name, like it is described in the XML of the blueprint
	const char* qname() const;
	//
	bool empty() const;
	context& push_back(const context& source);
};

struct XENO_EXPORT contens
{
	// This structure can enumerate the direct decendent contexts (plural!?) of its origin
	// The origin of a sequence is the structure that wraps the sequence.
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
	contens& skip_until();
private:
	const context* end() const;
	const context* this_context;
	mutable const context* next_sibling;
};

struct XENO_EXPORT element;

template <>
xeno::contens& xeno::contens::skip_until<xeno::element>();

struct XENO_EXPORT sequens
{
	explicit sequens(const context& origin);
	bool empty() const { return first == last; }
	const std::string& next() const;
	std::size_t length() const;
	const context& head() const;
	sequens tail() const;
private:
	sequens(const context* first, const context* last);
	const context* first;
	const context* last;
};

struct XENO_EXPORT context_object : boost::noncopyable
{
	operator const std::string&() const;
	operator std::string&();
	inline const std::string& str() const {
		return operator const std::string&();
	}
	const char* c_str() const;
	bool defined() const {
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
	context* _value;
	const char* const _default;
};

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


struct XENO_EXPORT textvalue : context_object {

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

struct XENO_EXPORT client : context {
	// Test client payload capabilites
	bool will_accept(const char* type) const;
	bool can_deliver(const char* type) const;
	bool will_destroy() const;
	// Error handling (very simple: blame the stranger :-)
	void blame();
	// Inspect payload (will read_stream_data on demand)
	document& content();
	// Payload construction
	client& text(const char* string);
	client& xml(const context& source);
	int content_length() const;
	// Get attributes for request (Content-Type and the HTTP Method are handled explicit)
	const char* attr(const char* name) const;
	bool sent_from(const char* user_agent) const;
	// I/O with the clients source
	bool read_stream_data(char* data, int size) const;
	// Set attributes for reply (Content-Type and the HTTP Status Code are handled explicit)
	client& attr(const char* name, const char* value);
	// NOTE: Set the content_type of a reply BEFORE providing the content,
	// so that a call to document() later on may put a DOCTYPE in the right place
	client& content_type(const char* type);
	client& status(const char* status);
	// Convenience methods for plain text documents
	template <const char* TEXT, const char* TYPE = xeno::type::TEXT>
	inline client& accept_static_text(const char* status) {
		return content_type(TYPE).text(TEXT).status(status);
	}
	template <const char* XML>
	inline client& accept_static_xml(const char* status) {
		return accept_static_text<XML,xeno::type::XML>(status);
	}
	template <const char* SCRIPT>
	inline client& accept_static_script(const char* status) {
		return accept_static_text<SCRIPT,xeno::type::ECMA>(status);
	}
};

} // namespace xeno

#endif /* XENO_H_ */
