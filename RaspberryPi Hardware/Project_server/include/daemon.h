#ifndef DAEMON_H
#define DAEMON_H

#include "common.h"

// 데몬 프로세스 생성 함수
void daemonize(const char* name);

#endif /* DAEMON_H */