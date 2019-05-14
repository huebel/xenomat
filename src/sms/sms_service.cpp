/*
 * sms_service.cpp
 *
 *  Created on: 12.05.2019
 *      Author: Peter Hübel
 */
//#define NDEBUG // always suppress traces
#include <xeno/trace.h>
#include <xeno/document.h>
#include <xeno/xeno_io.h>

#include "sms_service.hpp"

#include <cmath>

struct sms_acceleration {
	float x;		// Right-left acceleration (positive is rightwards)
	float y;		// Front-rear acceleration (positive is rearwards)
	float z;		// Up-down acceleration (positive is upwards)
	sms_acceleration() {}
	IO_CLASS(sms_acceleration);
};

// Structure for specifying a calibration.
struct sms_calibration {
	float zeros[3];	// Zero points for three axes (X, Y, Z)
	float onegs[3];	// One gravity values for three axes
	sms_calibration() {}
	IO_CLASS(sms_calibration);
};

IO_CLASS_IMPL(sms_acceleration)
:	IO_ATTR(x)
,	IO_ATTR(y)
,	IO_ATTR(z)
{}

IO_CLASS_IMPL(sms_calibration)
{
	IO_ARRAY(zeros);
	IO_ARRAY(onegs);
}

extern "C" {

// Returns time in microseconds, clipped to a long
long getUTime() {
	struct timeval t;
	gettimeofday(&t, 0);
	return (t.tv_sec * 1000000 + t.tv_usec);
}

// Returns time in milliseconds, clipped to a long
long getMTime() {
	struct timeval t;
	gettimeofday(&t, 0);
	return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

// Starts the accelerometer.
int smsStartup(void*, void*);

// Shuts down the accelerometer.
void smsShutdown(void);

// Fills in the accel record with calibrated acceleration data. Takes
// 1-2ms to return a value. Returns 0 if success, error number if failure.
int smsGetData(sms_acceleration *accel);

} // extern "C"


namespace macbook {

XENO_REGISTER(macbook, sms, sms_service);

sms_service::sms_service(Context& origin)
:	Origin(origin)
,	type("@type", origin, type::XML)
{
	if (smsStartup(nullptr, nullptr)) {
		throw "Can not initialise accelerometer";
	}
	else {
		TRACE("sms_service(%p)::CTOR\n", this);
	}
}

sms_service::~sms_service()
{
	smsShutdown();
}

void sms_service::invoke(contact& visitor, sequens& route)
{
	TRACELN("sms_service::invoke");
	if (route.empty()) {
		float period = 0.01; // Time between samples in seconds (all 10ms)
		int count = 100;	 // Number of samples; 0 means "go forever"
		const long uPeriod = round(period * 1000000);
		long startMTime, lastUTime, thisUTime, waitUTime;
		lastUTime = getUTime();
		startMTime = getMTime();
		sms_acceleration accel;
		element& samples = visitor.content().element().child("acceleration");
		samples.attr("count", std::to_string(count));
		samples.attr("period", std::to_string(uPeriod)+"µs");
		// Sample loop
		while (true) {
			if (0 == smsGetData(&accel)) {
				// TRACE("Sample [%04d]\t", count);
				context_writer(samples.child("sms")).apply(accel);
			}
			else {
				throw "Could not get SMS sample";
			}

			if (count > 0 && --count == 0) {
				// Time to stop sampling
				break;
			}

			// Yes, there's overflow all over the place here, but it doesn't
			// matter.
			thisUTime = getUTime();
			waitUTime = lastUTime + uPeriod - thisUTime;
			if (waitUTime > 0) {
				usleep(lastUTime + uPeriod - thisUTime);
			}
			lastUTime += uPeriod;
			// Make sure we don't get too far behind
			if (lastUTime < thisUTime) {
				lastUTime = thisUTime;
			}
		}	// End sample loop
		visitor.content_type((type.str()+";charset=utf-8").c_str()).status("200 OK");
	}
}

} // namespace macbook
