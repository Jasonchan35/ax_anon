#pragma once

#define axPollFlags_EnumList(E) \
	E(None,		= 0) \
	E(In,		= POLLIN) \
	E(Out,		= POLLOUT) \
	E(Error,	= POLLERR) \
	E(PRI,		= POLLPRI) \
	E(Hup,		= POLLHUP) \
	E(Invalid,	= POLLNVAL) \
//----------
ax_ENUM_FLAGS_CLASS(axPollFlags, axInt16);

// https://curl.haxx.se/mail/lib-2012-10/0038.html
// RE: cURL on Windows with _WIN32_WINNT=0x0600 suffers from WSAPoll bug
// WSAPoll does not report POLLERR when socket connection timeout

#if 0 // AX_OS_WINDOWS && _WIN32_WINNT >= 0x0600 // before Windows Vista or Windows Server 2008
	using axPollFd = ::WSAPOLLFD;

	AX_INLINE
	int ax_poll(axPollFd* fdArray, unsigned int fdCount, int timeoutMilliseconds) noexcept {
		int ret = ::WSAPoll(fdArray, fdCount, timeoutMilliseconds);
		//if (ret < 0) {
		//	auto err = WSAGetLastError();
		//	AX_DUMP_VAR(err);
		//}
		return ret;
	}

#elif AX_OS_WINDOWS
	#define ax_poll_win32_impl_by_select 1

	struct axPollFd {
		SOCKET	fd;
		short	events;
		short	revents;
	};
	int ax_poll(axPollFd* fdArray, unsigned int fdCount, int timeoutMilliseconds) noexcept;

#else
	using axPollFd = ::pollfd;

	AX_INLINE
	int ax_poll(axPollFd* fdArray, unsigned int fdCount, int timeoutMilliseconds) noexcept {
		return ::poll(fdArray, fdCount, timeoutMilliseconds);
	}
#endif
