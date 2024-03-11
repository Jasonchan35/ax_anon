#pragma once

#if AX_OS_UNIX

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <wctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/file.h>
#include <time.h>
#include <signal.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>

#define ax_EXE_SUFFIX ""
#define	ax_SO_SUFFIX  ".so"

#endif // #if AX_OS_UNIX