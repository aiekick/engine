/**
 * @file
 */

#pragma once

#include <memory>

namespace core {

/**
 * The time provider will get an updated tick time with every tick. It does not perform any timer system call on getting the
 * tick time - only if you really need the current time.
 */
class TimeProvider {
private:
	unsigned long _tickTime;
public:
	TimeProvider();

	/**
	 * @brief The tick time gives you the time in milliseconds when the tick was started.
	 * @note Updated once per tick
	 */
	inline unsigned long tickMillis() const {
		return _tickTime;
	}

	inline unsigned long tickSeconds() const {
		return _tickTime / 1000UL;
	}

	static std::string toString(unsigned long millis, const char *format = "%d-%m-%Y %H-%M-%S");

	unsigned long currentTime() const;

	static double currentNanos();

	void update(unsigned long now) {
		_tickTime = now;
	}
};

typedef std::shared_ptr<TimeProvider> TimeProviderPtr;

}
