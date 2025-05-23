#include "common.h"

// 전역 변수
int sock = -1;
volatile int running = 1;

// 명령어 모드 정의
typedef enum {
    MODE_MENU,         // 메인 메뉴 모드
    MODE_LED,          // LED 제어 모드
    MODE_BUZZER,       // 부저 제어 모드
    MODE_AUTO_LIGHT,   // 자동 조명 제어 모드
    MODE_BRIGHTNESS,   // LED 밝기 제어 모드
    MODE_SEGMENT,      // 7세그먼트 제어 모드
    MODE_COUNTDOWN,    // 카운트다운 제어 모드
    MODE_COUNT         // 카운트 제어 모드
} CommandMode;

// 소켓을 안전하게 닫는 함수
void close_socket_safely(int *sock) {
    if (*sock >= 0) {
        printf("소켓 %d 닫기\n", *sock);
        shutdown(*sock, SHUT_RDWR);
        close(*sock);
        *sock = -1;
    }
}

// 시그널 핸들러 함수
void sigint_handler(int sig) {
    (void)sig;
    printf("\n클라이언트를 종료합니다...\n");
    running = 0;
    
    // 소켓 닫기
    close_socket_safely(&sock);
    
    exit(EXIT_SUCCESS);
}

// 명령어 메뉴 출력 함수를 수정
void print_menu() {
    printf("\n======MENU======\n");
    printf("[ On : 1 / Off : 0 ] , 종료 : 00\n");
    printf("1. LED control (on/off)\n");
    printf("2. BUZZER control (on/off)\n");
    printf("3. AUTO_LIGHT control (on/off)\n");
    printf("-----------------------------------\n");
    printf("4. LED_BRIGHTNESS <Level> - 1 : Low | 2 : Middle | 3 : High\n");
    printf("5. READ_LIGHT (on -> 1 , off -> 0)\n");
    printf("6. SEGMENT <Number> - 0 ~ 9\n");
    printf("7. COUNTDOWN <Start> - 1 ~ 9\n");
    printf("8. COUNT <Start> <end> - 0 ~ 9\n");
    printf("9. STATUS - 서버 상태 확인\n");
    printf("10. SHUTDOWN - 서버 종료 (관리자 전용)\n");
    printf("0. EXIT - 클라이언트 종료\n");
}

// 현재 모드에 따른 프롬프트 출력
void print_mode_prompt(CommandMode mode) {
    switch (mode) {
        case MODE_LED:
            printf("\n[LED 제어 모드] 1:켜기, 0:끄기, 00:메뉴로 돌아가기\n");
            break;
        case MODE_BUZZER:
            printf("\n[부저 제어 모드] 1:켜기, 0:끄기, 00:메뉴로 돌아가기\n");
            break;
        case MODE_AUTO_LIGHT:
            printf("\n[자동 조명 제어 모드] 1:켜기, 0:끄기, 00:메뉴로 돌아가기\n");
            break;
        case MODE_BRIGHTNESS:
            printf("\n[LED 밝기 제어 모드] 1:최저, 2:중간, 3:최대, 00:메뉴로 돌아가기\n");
            break;
        case MODE_SEGMENT:
            printf("\n[7세그먼트 제어 모드] 0-9:숫자 표시, 00:메뉴로 돌아가기\n");
            break;
        case MODE_COUNTDOWN:
            printf("\n[카운트다운 제어 모드] 1-9:시작 숫자, 00:메뉴로 돌아가기\n");
            break;
        case MODE_COUNT:
            printf("\n[카운트 제어 모드] 시작,끝 형식으로 입력 (예: 3,9), 00:메뉴로 돌아가기\n");
            break;
        default:
            break;
    }
}

// 로그 출력 함수 (클라이언트용)
void log_message(LogLevel level, const char* message) {
    const char* level_str;
    
    switch (level) {
        case CUSTOM_LOG_DEBUG:
            level_str = "디버그";
            break;
        case CUSTOM_LOG_INFO:
            level_str = "정보";
            break;
        case CUSTOM_LOG_WARNING:
            level_str = "경고";
            break;
        case CUSTOM_LOG_ERROR:
            level_str = "오류";
            break;
        default:
            level_str = "알 수 없음";
            break;
    }
    
    printf("[%s] [%s] %s\n", get_time_string(), level_str, message);
    fflush(stdout);
}

// 현재 시간을 문자열로 반환하는 함수 (클라이언트용)
char* get_time_string() {
    static char time_str[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    return time_str;
}

// 명령어 모드에서 입력을 처리하는 함수
char* process_mode_input(CommandMode mode, const char* input) {
    static char cmd[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int start, end;
    
    // "00" 입력 시 메인 메뉴로 돌아가기
    if (strcmp(input, "00") == 0) {
        return NULL;  // NULL 반환하여 메인 메뉴로 돌아가기
    }
    
    // 모드별 입력 처리
    switch (mode) {
        case MODE_LED:
            if (strcmp(input, "1") == 0) {
                return "LED_ON";
            } else if (strcmp(input, "0") == 0) {
                return "LED_OFF";
            } else {
                printf("잘못된 입력입니다. 1, 0, 또는 00만 입력하세요.\n");
                return "";  // 빈 문자열 반환하여 명령 무시
            }
            
        case MODE_BUZZER:
            if (strcmp(input, "1") == 0) {
                return "BUZZER_ON";
            } else if (strcmp(input, "0") == 0) {
                return "BUZZER_OFF";
            } else {
                printf("잘못된 입력입니다. 1, 0, 또는 00만 입력하세요.\n");
                return "";
            }
            
        case MODE_AUTO_LIGHT:
            if (strcmp(input, "1") == 0) {
                return "AUTO_LIGHT ON";
            } else if (strcmp(input, "0") == 0) {
                return "AUTO_LIGHT OFF";
            } else {
                printf("잘못된 입력입니다. 1, 0, 또는 00만 입력하세요.\n");
                return "";
            }
            
        case MODE_BRIGHTNESS:
            if (strcmp(input, "1") == 0 || strcmp(input, "2") == 0 || strcmp(input, "3") == 0) {
                snprintf(cmd, BUFFER_SIZE, "LED_BRIGHTNESS%s", input);
                return cmd;
            } else {
                printf("잘못된 입력입니다. 1, 2, 3, 또는 00만 입력하세요.\n");
                return "";
            }
            
        case MODE_SEGMENT:
            if (strlen(input) == 1 && isdigit(input[0])) {
                snprintf(cmd, BUFFER_SIZE, "SEGMENT %s", input);
                return cmd;
            } else {
                printf("잘못된 입력입니다. 0-9 또는 00만 입력하세요.\n");
                return "";
            }
            
        case MODE_COUNTDOWN:
            if (strlen(input) == 1 && input[0] >= '1' && input[0] <= '9') {
                snprintf(cmd, BUFFER_SIZE, "COUNTDOWN %s", input);
                return cmd;
            } else {
                printf("잘못된 입력입니다. 1-9 또는 00만 입력하세요.\n");
                return "";
            }
            
        case MODE_COUNT:
            // 쉼표로 구분된 두 숫자 파싱 (예: "3,9")
            if (sscanf(input, "%d,%d", &start, &end) == 2) {
                // 범위 검사
                if (start < 0) start = 0;
                if (start > 9) start = 9;
                if (end < 0) end = 0;
                if (end > 9) end = 9;
                
                snprintf(cmd, BUFFER_SIZE, "COUNT %d %d", start, end);
                return cmd;
            } else {
                printf("잘못된 입력입니다. '시작,끝' 형식으로 입력하세요 (예: 3,9).\n");
                return "";
            }
            
        default:
            return "";
    }
}

// 메뉴 번호를 서버 명령어로 변환하는 함수를 수정
char* convert_menu_to_command(const char* input) {
    static char cmd[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int num, start, end, level, onoff;
    
    // 입력이 숫자인지 확인
    if (isdigit(input[0])) {
        switch (atoi(input)) {
            case 1: // LED control
                return "MODE_LED";  // 특수 모드 전환 명령
                
            case 2: // BUZZER control
                return "MODE_BUZZER";  // 특수 모드 전환 명령
                
            case 3: // AUTO_LIGHT control
                return "MODE_AUTO_LIGHT";  // 특수 모드 전환 명령
                
            case 4: // LED_BRIGHTNESS
                return "MODE_BRIGHTNESS";  // 특수 모드 전환 명령
                
            case 5: // READ_LIGHT
                return "READ_LIGHT";
                
            case 6: // SEGMENT
                return "MODE_SEGMENT";  // 특수 모드 전환 명령
                
            case 7: // COUNTDOWN
                return "MODE_COUNTDOWN";  // 특수 모드 전환 명령
                
            case 8: // COUNT
                return "MODE_COUNT";  // 특수 모드 전환 명령
                
            case 9: // STATUS
                return "STATUS";
                
            case 10: // SHUTDOWN
                printf("정말로 서버를 종료하시겠습니까? (y/N): ");
                fflush(stdout);
                if (fgets(temp, BUFFER_SIZE, stdin) == NULL) {
                    return NULL;
                }
                temp[strcspn(temp, "\n")] = 0;
                if (temp[0] == 'y' || temp[0] == 'Y') {
                    return "SHUTDOWN";
                } else {
                    printf("서버 종료가 취소되었습니다.\n");
                    return "";
                }
                
            case 0: // EXIT
                return "EXIT";
                
            default:
                return "";
        }
    } else {
        // 이미 명령어 형식이면 그대로 반환
        return (char*)input;
    }
}

// 메인 함수
int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};
    char log_msg[BUFFER_SIZE];
    int retry_count = 0;
    char *command;
    CommandMode current_mode = MODE_MENU;  // 현재 명령어 모드
    
    // 명령행 인자 확인
    if (argc < 2) {
        printf("사용법: %s <서버_IP_주소>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // 시그널 핸들러 등록
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);
    signal(SIGPIPE, SIG_IGN);  // SIGPIPE 무시 추가
    
    log_message(CUSTOM_LOG_INFO, "하드웨어 제어 클라이언트 시작");
    
    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_message(CUSTOM_LOG_ERROR, "소켓 생성 실패");
        return EXIT_FAILURE;
    }
    
    printf("클라이언트 소켓 생성됨: %d\n", sock);
    
    // 서버 주소 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // 서버 IP 주소 변환
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        log_message(CUSTOM_LOG_ERROR, "유효하지 않은 주소/주소가 지원되지 않음");
        close_socket_safely(&sock);
        return EXIT_FAILURE;
    }
    
    // 서버에 연결 (재시도 로직 포함)
    log_message(CUSTOM_LOG_INFO, "서버에 연결 시도 중...");
    
    while (retry_count < MAX_RETRY && running) {
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
            snprintf(log_msg, BUFFER_SIZE, "서버(%s:%d)에 연결되었습니다", argv[1], PORT);
            log_message(CUSTOM_LOG_INFO, log_msg);
            break;
        }
        
        // 연결 실패 처리
        snprintf(log_msg, BUFFER_SIZE, "연결 실패: %s", strerror(errno));
        log_message(CUSTOM_LOG_WARNING, log_msg);
        
        retry_count++;
        if (retry_count < MAX_RETRY) {
            snprintf(log_msg, BUFFER_SIZE, "%d초 후 재시도 (%d/%d)...", 
                     RETRY_DELAY, retry_count, MAX_RETRY);
            log_message(CUSTOM_LOG_INFO, log_msg);
            sleep(RETRY_DELAY);
        } else {
            log_message(CUSTOM_LOG_ERROR, "최대 재시도 횟수 초과. 클라이언트를 종료합니다.");
            close_socket_safely(&sock);
            return EXIT_FAILURE;
        }
    }
    
    if (!running) {
        close_socket_safely(&sock);
        return EXIT_SUCCESS;
    }
    
    // 서버로부터 환영 메시지 수신
    log_message(CUSTOM_LOG_INFO, "서버로부터 환영 메시지 대기 중...");
    
    int valread = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    
    if (valread > 0) {
        buffer[valread] = '\0';
        snprintf(log_msg, BUFFER_SIZE - 40, "서버로부터 수신된 메시지: '%s'", buffer);
        log_message(CUSTOM_LOG_INFO, log_msg);
        
        if (strcmp(buffer, "SERVER_READY") == 0) {
            log_message(CUSTOM_LOG_INFO, "서버가 준비되었습니다.");
        } else {
            log_message(CUSTOM_LOG_WARNING, "서버로부터 예상치 못한 응답을 받았습니다.");
            close_socket_safely(&sock);
            return EXIT_FAILURE;
        }
    } else if (valread == 0) {
        log_message(CUSTOM_LOG_ERROR, "서버가 연결을 종료했습니다.");
        close_socket_safely(&sock);
        return EXIT_FAILURE;
    } else {
        snprintf(log_msg, BUFFER_SIZE, "읽기 오류: %s", strerror(errno));
        log_message(CUSTOM_LOG_ERROR, log_msg);
        close_socket_safely(&sock);
        return EXIT_FAILURE;
    }
    
    // 명령어 메뉴 출력
    print_menu();
    
    // 명령 입력 및 전송 루프
    while (running) {
        // 현재 모드에 따른 프롬프트 출력
        if (current_mode == MODE_MENU) {
            printf("\n명령 입력: ");
        } else {
            print_mode_prompt(current_mode);
            printf("입력: ");
        }
        fflush(stdout);
        
        // 명령 입력 받기
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            if (errno == EINTR && !running) {
                break;
            }
            log_message(CUSTOM_LOG_ERROR, "입력 오류");
            continue;
        }
        
        // 개행 문자 제거
        input[strcspn(input, "\n")] = 0;
        
        // 입력이 비어있으면 다시 입력 받기
        if (strlen(input) == 0) {
            continue;
        }
        
        // 현재 모드에 따라 입력 처리
        if (current_mode == MODE_MENU) {
            // 메인 메뉴 모드에서의 입력 처리
            command = convert_menu_to_command(input);
            
            // 모드 전환 명령 처리
            if (command != NULL && strncmp(command, "MODE_", 5) == 0) {
                if (strcmp(command, "MODE_LED") == 0) {
                    current_mode = MODE_LED;
                    continue;
                } else if (strcmp(command, "MODE_BUZZER") == 0) {
                    current_mode = MODE_BUZZER;
                    continue;
                } else if (strcmp(command, "MODE_AUTO_LIGHT") == 0) {
                    current_mode = MODE_AUTO_LIGHT;
                    continue;
                } else if (strcmp(command, "MODE_BRIGHTNESS") == 0) {
                    current_mode = MODE_BRIGHTNESS;
                    continue;
                } else if (strcmp(command, "MODE_SEGMENT") == 0) {
                    current_mode = MODE_SEGMENT;
                    continue;
                } else if (strcmp(command, "MODE_COUNTDOWN") == 0) {
                    current_mode = MODE_COUNTDOWN;
                    continue;
                } else if (strcmp(command, "MODE_COUNT") == 0) {
                    current_mode = MODE_COUNT;
                    continue;
                }
            }
        } else {
            // 특정 명령어 모드에서의 입력 처리
            command = process_mode_input(current_mode, input);
            
            // NULL 반환 시 메인 메뉴로 돌아가기
            if (command == NULL) {
                current_mode = MODE_MENU;
                print_menu();
                continue;
            }
            
            // 빈 문자열 반환 시 명령 무시
            if (strlen(command) == 0) {
                continue;
            }
        }
        
        // 명령어가 없으면 다시 입력 받기
        if (command == NULL || strlen(command) == 0) {
            continue;
        }
        
        // EXIT 명령 처리
        if (strcmp(command, "EXIT") == 0 || strcmp(command, "0") == 0) {
            // 서버에 종료 명령 전송
            log_message(CUSTOM_LOG_INFO, "서버에 EXIT 명령 전송 중...");
            
            if (send(sock, "EXIT", 4, 0) < 0) {
                snprintf(log_msg, BUFFER_SIZE, "EXIT 명령 전송 실패: %s", strerror(errno));
                log_message(CUSTOM_LOG_WARNING, log_msg);
            } else {
                log_message(CUSTOM_LOG_INFO, "EXIT 명령 전송 성공");
                
                // 서버 응답 대기
                memset(buffer, 0, BUFFER_SIZE);
                int valread = recv(sock, buffer, BUFFER_SIZE - 1, 0);
                
                if (valread > 0) {
                    buffer[valread] = '\0';
                    snprintf(log_msg, BUFFER_SIZE - 16, "서버 응답: %s", buffer);
                    log_message(CUSTOM_LOG_INFO, log_msg);
                } else {
                    log_message(CUSTOM_LOG_WARNING, "서버 응답 수신 실패");
                }
            }
            
            log_message(CUSTOM_LOG_INFO, "프로그램을 종료합니다.");
            break;
        }
        
        // 서버에 명령 전송
        if (send(sock, command, strlen(command), 0) < 0) {
            snprintf(log_msg, BUFFER_SIZE, "명령 전송 실패: %s", strerror(errno));
            log_message(CUSTOM_LOG_ERROR, log_msg);
            
            // 연결 상태 확인
            if (errno == EPIPE || errno == ECONNRESET || errno == ENOTCONN) {
                log_message(CUSTOM_LOG_ERROR, "서버와의 연결이 끊어졌습니다.");
                break;
            }
            continue;
        }
        
        snprintf(log_msg, BUFFER_SIZE, "명령을 전송했습니다: %.900s%s", 
                 command, strlen(command) > 900 ? "..." : "");
        log_message(CUSTOM_LOG_INFO, log_msg);
        
        // 서버로부터 응답 수신
        memset(buffer, 0, BUFFER_SIZE);
        valread = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        
        if (valread > 0) {
            buffer[valread] = '\0';
            snprintf(log_msg, BUFFER_SIZE, "서버 응답: %.900s%s", 
                     buffer, strlen(buffer) > 900 ? "..." : "");
            log_message(CUSTOM_LOG_INFO, log_msg);
            
            // SHUTDOWN 응답 처리
            if (strstr(buffer, "SERVER SHUTTING DOWN") != NULL) {
                log_message(CUSTOM_LOG_INFO, "서버가 종료됩니다. 클라이언트를 종료합니다.");
                break;
            }
            
            // 메인 메뉴 모드에서만 메뉴 다시 출력
            if (current_mode == MODE_MENU) {
                print_menu();
            }
        } else if (valread == 0) {
            log_message(CUSTOM_LOG_WARNING, "서버가 연결을 종료했습니다.");
            break;
        } else {
            snprintf(log_msg, BUFFER_SIZE, "응답 수신 실패: %s", strerror(errno));
            log_message(CUSTOM_LOG_ERROR, log_msg);
            
            // 연결 상태 확인
            if (errno == ECONNRESET || errno == ENOTCONN) {
                log_message(CUSTOM_LOG_ERROR, "서버와의 연결이 끊어졌습니다.");
                break;
            }
        }
    }
    
    // 소켓 닫기
    close_socket_safely(&sock);
    
    log_message(CUSTOM_LOG_INFO, "프로그램 종료");
    return EXIT_SUCCESS;
}
