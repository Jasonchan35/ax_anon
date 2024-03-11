#include "ax_poll.h"

#if ax_poll_win32_impl_by_select

// fd_set for ::select()

struct axPollFdSet {
	const static axInt kMaxCount = 4096;

	// copy from fd_set
    u_int   fd_count;              /* how many are SET? */
    SOCKET  fd_array[kMaxCount];   /* an array of SOCKETs */
	// ------

	axPollFdSet() {
		fd_count = 0;
	}

	void set(SOCKET fd)	{ fd_array[fd_count] = fd; fd_count++; }
	bool find(SOCKET fd) {
		auto* p = fd_array;
		auto* e = p + fd_count;
		for (; p < e; p++) {
			if (*p == fd)
				return true;
		}
		return false;
	}
};

int ax_poll(axPollFd* fdArray, unsigned int fdCount, int timeoutMilliseconds) noexcept {
	axPollFdSet rd; //read
	axPollFdSet wr; //write
	axPollFdSet ex; //except

	if (fdCount >= axPollFdSet::kMaxCount) {
		AX_ASSERT(false);
		return -1;
	}

	int highesetFd = 0;
	{
		auto* p = fdArray;
		auto* e = p + fdCount;
		for (; p < e; p++) {
			auto fd = p->fd;
			if (fd == INVALID_SOCKET)
				continue;

			short& ev = p->events;
			p->revents = 0;

			if (ev & POLLIN ) rd.set(fd);
			if (ev & POLLOUT) wr.set(fd);
			ex.set( fd );

			if ((int)fd > highesetFd) {
				highesetFd = (int)fd;
			}
		}
	}

	timeval *tv = nullptr; //infinite
	timeval t;

	if (timeoutMilliseconds >= 0) {
		t.tv_usec = (timeoutMilliseconds % 1000) * 1000;
		t.tv_sec  = timeoutMilliseconds / 1000;
		tv = &t;
	}

	int ret;
	if (rd.fd_count == 0 && wr.fd_count == 0 && ex.fd_count == 0) {
		Sleep(timeoutMilliseconds);
		ret = 0;
	}else{
		ret = select( highesetFd + 1, // Win32: Ignored. The nfds parameter is included only for compatibility with Berkeley sockets.
					  rd.fd_count ? reinterpret_cast<fd_set*>(&rd) : nullptr,
					  wr.fd_count ? reinterpret_cast<fd_set*>(&wr) : nullptr,
					  ex.fd_count ? reinterpret_cast<fd_set*>(&ex) : nullptr,
					  tv);
	}

	if (ret < 0) {
		return ret;
	}

	{
		auto* p = fdArray;
		auto* e = p + fdCount;
		for (; p < e; p++) {
			if (rd.find(p->fd)) {
				p->revents |= POLLIN;
			}
			if (wr.find(p->fd)) {
				p->revents |= POLLOUT;
			}
			if (ex.find(p->fd)) {
				p->revents |= POLLERR;
			}
		}
	}

	return ret;
}

#endif // AX_OS_WINDOWS
