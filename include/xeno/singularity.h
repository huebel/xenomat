/*
 * singularity.h
 *
 *  Created on: 06.04.2014
 *      Author: Peter Hübel
 */

#ifndef XENO_SINGULARITY_H_
#define XENO_SINGULARITY_H_

#include "xeno.h"

namespace xeno XENO_NAMESPACE_EXPORT {

template <typename T, typename T2 = int, T2 UNDEFINED = 0>
struct XENO_EXPORT singularity {
	static const T undefined = UNDEFINED;
	inline static bool match(T val) { return false; }
};

template <>
struct XENO_EXPORT singularity<std::string>
{
	static const std::string undefined;
	static bool match(const std::string& val) { return val.empty(); };
};

template <>
struct XENO_EXPORT singularity<float>
{
	static const float undefined;
	static inline bool match(float val) { return val != val; }
};

template <>
struct XENO_EXPORT singularity<double>
{
	static const double undefined;
	static inline bool match(float val) { return val != val; }
};

template <typename T>
inline bool undefined(T value)
{
	return singularity<T>::match(value);
}

template <typename T>
inline T undefined()
{
	return singularity<T>::undefined;
}

} // namespace xeno


#endif /* XENO_SINGULARITY_H_ */
