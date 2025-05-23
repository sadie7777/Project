//클라이언트와 서버 간 공유되는 상수 및 구조체 정의

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <syslog.h>
#include <netinet/tcp.h>
#include <ctype.h>  // isdigit() 함수 사용을 위해 추가

// 포트 번호 정의 (7500으로 변경)
#define PORT 7800
#define BUFFER_SIZE 1024
#define MAX_RETRY 5
#define RETRY_DELAY 2  // 재시도 간격(초)
#define CONNECTION_TIMEOUT 10  // 연결 타임아웃(초)

// 명령어 정의
#define CMD_LED_ON "LED_ON"
#define CMD_LED_OFF "LED_OFF"
#define CMD_LED_BRIGHTNESS "LED_BRIGHTNESS"
#define CMD_BUZZER_ON "BUZZER_ON"
#define CMD_BUZZER_OFF "BUZZER_OFF"
#define CMD_READ_LIGHT "READ_LIGHT"
#define CMD_AUTO_LIGHT "AUTO_LIGHT"  // 자동 조명 제어 명령어 추가
#define CMD_SEGMENT "SEGMENT"
#define CMD_COUNT "COUNT"

// 상태 정의
#define ON 1
#define OFF 0

// 응답 코드 정의
#define RESP_OK "OK"
#define RESP_ERROR "ERROR"

// 로그 레벨 열거형 (syslog.h와 충돌 방지를 위해 이름 변경)
typedef enum {
    CUSTOM_LOG_DEBUG,
    CUSTOM_LOG_INFO,
    CUSTOM_LOG_WARNING,
    CUSTOM_LOG_ERROR
} LogLevel;

// 함수 선언
void log_message(LogLevel level, const char* message);
char* get_time_string(void);

#endif /* COMMON_H */
