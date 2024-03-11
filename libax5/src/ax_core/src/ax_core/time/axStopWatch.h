//
//  axStopWatch.h
//  ax_core
//
//  Created by Jason on 2014-11-27.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "axTimestamp.h"

template<class TIME>
class axStopWatchT {
public:
	using Time = TIME;

	axStopWatchT() {
		_start.setToNow();
		_last = _start;
	}

	void	reset() {
		_start.setToNow();
	}
	
	void	resetToLastGet() {
		_start = _last;
	}
	
	double	lastGet() {
		return (_last.toNanoseconds() - _start.toNanoseconds()) / 1000000000.0;
	}
	
	double	get() {
		_last.setToNow();
		return lastGet();
	}


private:
	Time _start;
	Time _last;
};

using axStopWatch = axStopWatchT<axHiResTime>;