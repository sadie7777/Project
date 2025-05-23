#include "../include/common.h"
#include "../include/daemon.h"
#include "../include/hardware.h"
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <stdarg.h>  // va_list 등을 위해 추가
#include <sys/select.h>  // select() 함수 사용을 위해 추가

// 전역 변수
int server_fd = -1;
volatile int running = 1;
int hardware_initialized = 0;
FILE *log_file = NULL;  // 로그 파일 포인터
int single_client_mode = 0;  // 단일 클라이언트 모드 플래그

// 최대 클라이언트 수 정의
#define MAX_CLIENTS 10
int client_sockets[MAX_CLIENTS];  // 클라이언트 소켓 배열

// 로그 파일에 메시지 기록 함수 - 터미널과 파일 모두에 기록
void log_to_file(const char *format, ...) {
    // 현재 시간 가져오기
    char time_str[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    
    // 터미널에 출력
    va_list args1;
    va_start(args1, format);
    printf("[%s] ", time_str);
    vprintf(format, args1);
    fflush(stdout);  // 즉시 출력
    va_end(args1);
    
    // 로그 파일이 열려있으면 파일에도 기록
    if (log_file != NULL) {
        // 로그 파일에 시간 기록
        fprintf(log_file, "[%s] ", time_str);
        
        // 가변 인자 처리
        va_list args2;
        va_start(args2, format);
        vfprintf(log_file, format, args2);
        va_end(args2);
        
        // 파일 버퍼 즉시 비우기
        fflush(log_file);
    }
}

// 소켓을 안전하게 닫는 함수
void close_socket_safely(int *sock) {
    if (*sock >= 0) {
        log_to_file("소켓 %d 닫기\n", *sock);
        shutdown(*sock, SHUT_RDWR);
        close(*sock);
        *sock = -1;
    }
}

// 시그널 핸들러 함수 (종료 처리)
void sigint_handler(int sig) {
    (void)sig;
    log_to_file("\n서버를 종료합니다...\n");
    running = 0;
    
    // 장치 정리
    if (hardware_initialized) {
        hardware_cleanup();
    }
    
    // 모든 클라이언트 소켓 닫기
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0) {
            close_socket_safely(&client_sockets[i]);
        }
    }
    
    // 서버 소켓 닫기
    close_socket_safely(&server_fd);
    
    // 로그 파일 닫기
    if (log_file != NULL) {
        log_to_file("로그 파일을 닫습니다.\n");
        fclose(log_file);
        log_file = NULL;
    }
    
    // 강제 종료 대신 정상 종료
    exit(EXIT_SUCCESS);
}

// 모든 클라이언트에게 메시지 브로드캐스트
void broadcast_to_all_clients(const char *message) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
}

// 연결된 클라이언트 수 확인
int get_connected_clients_count() {
    int count = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0) {
            count++;
        }
    }
    return count;
}

// 명령어 처리 함수
void process_command(int client_socket, char *command) {
    char response[BUFFER_SIZE] = {0};
    
    log_to_file("명령 처리 시작: %s\n", command);
    
    // 서버 종료 명령 (관리자 전용)
    if (strcmp(command, "SHUTDOWN") == 0) {
        log_to_file("서버 종료 명령 수신\n");
        strcpy(response, "OK SERVER SHUTTING DOWN");
        send(client_socket, response, strlen(response), 0);
        
        // 모든 클라이언트에게 서버 종료 알림
        broadcast_to_all_clients("SERVER SHUTDOWN");
        
        // 서버 종료
        running = 0;
        return;
    }
    // 연결된 클라이언트 수 확인 명령
    else if (strcmp(command, "STATUS") == 0) {
        int connected_clients = get_connected_clients_count();
        snprintf(response, BUFFER_SIZE, "CONNECTED_CLIENTS %d", connected_clients);
        log_to_file("상태 확인: 연결된 클라이언트 수 %d\n", connected_clients);
    }
    // LED 제어 명령
    else if (strncmp(command, "LED", 3) == 0) {
        if (strcmp(command, CMD_LED_ON) == 0) {
            log_to_file("LED 켜기 명령 실행\n");
            led_control(ON);
            strcpy(response, "OK LED ON");
            log_to_file("LED 켜기 완료\n");
        } else if (strcmp(command, CMD_LED_OFF) == 0) {
            log_to_file("LED 끄기 명령 실행\n");
            led_control(OFF);
            strcpy(response, "OK LED OFF");
            log_to_file("LED 끄기 완료\n");
        } else if (strncmp(command, "LED_BRIGHTNESS", 14) == 0) {
            int level = 0;
            if (sscanf(command + 14, "%d", &level) == 1 && level >= 1 && level <= 3) {
                log_to_file("LED 밝기 조절 명령 실행: 레벨 %d\n", level);
                led_brightness(level);
                snprintf(response, BUFFER_SIZE, "OK LED BRIGHTNESS %d", level);
                log_to_file("LED 밝기 조절 완료: 레벨 %d\n", level);
            } else {
                log_to_file("잘못된 LED 밝기 레벨: %s\n", command);
                strcpy(response, "ERROR Invalid LED brightness level. Use: LED_BRIGHTNESS <1-3>");
            }
        }
    }
    // 부저 제어 명령
    else if (strncmp(command, "BUZZER", 6) == 0) {
        if (strcmp(command, CMD_BUZZER_ON) == 0) {
            log_to_file("부저 켜기 명령 실행\n");
            buzzer_control(ON);
            strcpy(response, "OK BUZZER ON");
            log_to_file("부저 켜기 완료\n");
        } else if (strcmp(command, CMD_BUZZER_OFF) == 0) {
            log_to_file("부저 끄기 명령 실행\n");
            buzzer_control(OFF);
            strcpy(response, "OK BUZZER OFF");
            log_to_file("부저 끄기 완료\n");
        }
    }
    // 스위치 상태 읽기 명령
    else if (strcmp(command, "READ_SWITCH") == 0) {
        log_to_file("스위치 상태 읽기 명령 실행\n");
        int switch_value = switch_read();
        
        snprintf(response, BUFFER_SIZE, "SWITCH %d", switch_value);
        
        log_to_file("스위치 상태: %d, 응답 전송: %s\n", switch_value, response);
    }
    // 조도센서 읽기 명령
    else if (strcmp(command, CMD_READ_LIGHT) == 0) {
        log_to_file("조도센서 읽기 명령 실행\n");
        int light_value = light_sensor_read();
        
        snprintf(response, BUFFER_SIZE, "LIGHT %d", light_value);
        
        log_to_file("조도센서 값: %d, 응답 전송: %s\n", light_value, response);
    }
    // 조도센서 자동 제어 명령
    else if (strncmp(command, "AUTO_LIGHT", 10) == 0) {
        int enable = 0;
        if (strcmp(command, "AUTO_LIGHT ON") == 0) {
            enable = 1;
        } else if (strcmp(command, "AUTO_LIGHT OFF") == 0) {
            enable = 0;
        } else {
            log_to_file("잘못된 AUTO_LIGHT 명령 형식: %s\n", command);
            strcpy(response, "ERROR Invalid AUTO_LIGHT format. Use: AUTO_LIGHT ON|OFF");
            goto send_response;
        }
        
        log_to_file("조도센서 자동 제어 %s 명령 실행\n", enable ? "활성화" : "비활성화");
        auto_light_control(enable);
        snprintf(response, BUFFER_SIZE, "OK AUTO_LIGHT %s", enable ? "ON" : "OFF");
        log_to_file("조도센서 자동 제어 %s 완료\n", enable ? "활성화" : "비활성화");
    }
    // 7세그먼트 제어 명령
    else if (strncmp(command, CMD_SEGMENT, 7) == 0) {
        int number = atoi(command + 8);
        log_to_file("7세그먼트 표시 시작: %d\n", number);
        segment_display(number);
        snprintf(response, BUFFER_SIZE, "OK SEGMENT %d", number);
        log_to_file("7세그먼트 표시 완료: %d\n", number);
    }
    // 7세그먼트 카운트다운 명령
    else if (strncmp(command, "COUNTDOWN", 9) == 0) {
        int start_value = atoi(command + 10);
        log_to_file("7세그먼트 카운트다운 시작: %d부터\n", start_value);
        
        // 카운트다운 시작
        segment_countdown(start_value);
        
        snprintf(response, BUFFER_SIZE, "OK COUNTDOWN %d", start_value);
        log_to_file("7세그먼트 카운트다운 명령 처리 완료\n");
    }
    // 7세그먼트 카운트다운 중지 명령 (제거된 기능이지만 서버에서는 처리)
    else if (strcmp(command, "COUNTDOWN_STOP") == 0) {
        log_to_file("7세그먼트 카운트다운 중지 명령 실행\n");
        countdown_stop();
        strcpy(response, "OK COUNTDOWN_STOP");
        log_to_file("7세그먼트 카운트다운 중지 완료\n");
    }
    // 자동 카운트 명령
    else if (strncmp(command, CMD_COUNT, 5) == 0) {
        int start, end;
        if (sscanf(command + 5, "%d %d", &start, &end) == 2) {
            log_to_file("7세그먼트 카운트 시작: %d부터 %d까지\n", start, end);
            
            // 응답 먼저 전송
            snprintf(response, BUFFER_SIZE, "OK COUNT %d %d", start, end);
            if (send(client_socket, response, strlen(response), 0) < 0) {
                log_to_file("COUNT 응답 전송 실패: %s\n", strerror(errno));
            }
            
            // 카운트 실행 (응답 후 실행)
            segment_count(start, end);
            
            log_to_file("7세그먼트 카운트 완료: %d부터 %d까지\n", start, end);
            
            // 응답을 이미 보냈으므로 빈 문자열로 설정
            response[0] = '\0';
        } else {
            log_to_file("잘못된 COUNT 명령 형식: %s\n", command);
            strcpy(response, "ERROR Invalid COUNT format. Use: COUNT <start> <end>");
        }
    }
    else {
        log_to_file("알 수 없는 명령: %s\n", command);
        strcpy(response, "ERROR Unknown command");
    }
    
send_response:
    // 응답 전송 (이미 전송한 경우 제외)
    if (strlen(response) > 0) {
        log_to_file("응답 전송: %s\n", response);
        if (send(client_socket, response, strlen(response), 0) < 0) {
            log_to_file("응답 전송 실패: %s\n", strerror(errno));
        }
    }
}

// 사용법 출력 함수
void print_usage(const char *program_name) {
    printf("사용법: %s [옵션]\n", program_name);
    printf("옵션:\n");
    printf("  -d          데몬 모드로 실행\n");
    printf("  -s          단일 클라이언트 모드 (클라이언트 종료 시 서버도 종료)\n");
    printf("  -h, --help  이 도움말 출력\n");
    printf("\n");
    printf("예시:\n");
    printf("  %s           일반 모드 (다중 클라이언트)\n", program_name);
    printf("  %s -d        데몬 모드 (다중 클라이언트)\n", program_name);
    printf("  %s -s        단일 클라이언트 모드\n", program_name);
    printf("  %s -d -s     데몬 모드 + 단일 클라이언트 모드\n", program_name);
}

// 메인 함수
int main(int argc, char *argv[]) {
    int daemon_mode = 0;  // 데몬 모드 플래그
    
    // 명령행 인자 처리
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            daemon_mode = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            single_client_mode = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            printf("알 수 없는 옵션: %s\n", argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    
    // 클라이언트 소켓 배열 초기화
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }
    
    // 현재 작업 디렉토리 확인
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("현재 작업 디렉토리: %s\n", cwd);
    }
    
    // 로그 파일 열기 (절대 경로 사용)
    log_file = fopen("/home/sso/work/Project/server_log.txt", "a");  // append 모드로 변경
    if (log_file == NULL) {
        printf("로그 파일 열기 실패: %s (경로: /home/sso/work/Project/server_log.txt)\n", strerror(errno));
        return EXIT_FAILURE;  // 로그 파일을 열 수 없으면 종료
    } else {
        printf("로그 파일 열기 성공: /home/sso/work/Project/server_log.txt\n");
    }
    
    // 로그 파일 버퍼링 비활성화
    setvbuf(log_file, NULL, _IONBF, 0);
    
    // 로그 파일 시작 메시지
    fprintf(log_file, "\n\n--- 서버 시작: %s ---\n", get_time_string());
    fflush(log_file);
    
    // 데몬 프로세스로 변환 (옵션)
    if (daemon_mode) {
        printf("데몬 모드로 실행합니다...\n");
        daemonize("hardware_server");
    } else {
        printf("일반 모드로 실행합니다. (데몬 모드: ./server -d)\n");
    }
    
    // 단일 클라이언트 모드 안내
    if (single_client_mode) {
        printf("단일 클라이언트 모드: 클라이언트 종료 시 서버도 종료됩니다.\n");
    } else {
        printf("다중 클라이언트 모드: 여러 클라이언트가 동시 접속 가능합니다.\n");
    }
    
    log_to_file("서버 시작 중... (데몬 모드: %s, 단일 클라이언트 모드: %s)\n", 
                daemon_mode ? "활성화" : "비활성화",
                single_client_mode ? "활성화" : "비활성화");
    
    // 시그널 핸들러 등록
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);
    signal(SIGPIPE, SIG_IGN);  // SIGPIPE 무시
    
    // 장치 초기화
    if (hardware_init() != 0) {
        log_to_file("하드웨어 초기화 실패. 서버를 종료합니다.\n");
        if (log_file != NULL) {
            fclose(log_file);
        }
        return EXIT_FAILURE;
    }
    hardware_initialized = 1;
    
    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_to_file("소켓 생성 실패\n");
        hardware_cleanup();
        if (log_file != NULL) {
            fclose(log_file);
        }
        return EXIT_FAILURE;
    }
    
    log_to_file("서버 소켓 생성됨: %d\n", server_fd);
    
    // 소켓 옵션 설정
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        log_to_file("SO_REUSEADDR 설정 실패\n");
        close_socket_safely(&server_fd);
        hardware_cleanup();
        if (log_file != NULL) {
            fclose(log_file);
        }
        return EXIT_FAILURE;
    }
    
    log_to_file("SO_REUSEADDR 설정 성공\n");
    
    // 주소 설정
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        log_to_file("바인드 실패: %s\n", strerror(errno));
        close_socket_safely(&server_fd);
        hardware_cleanup();
        if (log_file != NULL) {
            fclose(log_file);
        }
        return EXIT_FAILURE;
    }
    
    log_to_file("바인드 성공\n");
    
    // 연결 대기 상태로 전환
    if (listen(server_fd, 5) < 0) {
        log_to_file("리슨 실패: %s\n", strerror(errno));
        close_socket_safely(&server_fd);
        hardware_cleanup();
        if (log_file != NULL) {
            fclose(log_file);
        }
        return EXIT_FAILURE;
    }
    
    log_to_file("서버가 포트 %d에서 실행 중입니다...\n", PORT);
    
    // select() 사용을 위한 변수 선언
    fd_set readfds;
    int max_sd;
    struct timeval tv;
    
    // 클라이언트 연결 수락 및 처리 루프 (select() 사용)
    while (running) {
        // fd_set 초기화
        FD_ZERO(&readfds);
        
        // 서버 소켓 추가
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;
        
        // 클라이언트 소켓 추가
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            
            // 유효한 소켓 디스크립터라면 fd_set에 추가
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            
            // 최대 소켓 디스크립터 갱신
            if (sd > max_sd) {
                max_sd = sd;
            }
        }
        
        // select 타임아웃 설정 (1초)
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        // select() 호출
        int activity = select(max_sd + 1, &readfds, NULL, NULL, &tv);
        
        if (activity < 0 && errno != EINTR) {
            log_to_file("select 오류: %s\n", strerror(errno));
            continue;
        }
        
        // 타임아웃이거나 시그널에 의한 인터럽트
        if (activity <= 0) {
            continue;
        }
        
        // 서버 소켓에 이벤트 발생 (새 클라이언트 연결)
        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            
            // 클라이언트 연결 수락
            int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
            if (new_socket < 0) {
                log_to_file("연결 수락 실패: %s\n", strerror(errno));
                continue;
            }
            
            // 클라이언트 정보 로깅
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            log_to_file("새 클라이언트 연결됨 (IP: %s, 포트: %d, 소켓: %d)\n", 
                   client_ip, ntohs(client_addr.sin_port), new_socket);
            
            // 환영 메시지 전송
            const char *welcome_msg = "SERVER_READY";
            if (send(new_socket, welcome_msg, strlen(welcome_msg), 0) < 0) {
                log_to_file("환영 메시지 전송 실패: %s\n", strerror(errno));
                close_socket_safely(&new_socket);
                continue;
            }
            
            // 클라이언트 소켓 배열에 추가
            int added = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] < 0) {
                    client_sockets[i] = new_socket;
                    log_to_file("클라이언트를 슬롯 %d에 추가\n", i);
                    added = 1;
                    break;
                }
            }
            
            // 최대 클라이언트 수 초과
            if (!added) {
                log_to_file("최대 클라이언트 수 초과. 연결 거부.\n");
                const char *error_msg = "ERROR Server is full";
                send(new_socket, error_msg, strlen(error_msg), 0);
                close_socket_safely(&new_socket);
            }
        }
        
        // 클라이언트 소켓 이벤트 처리
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                // 클라이언트로부터 데이터 수신
                char buffer[BUFFER_SIZE] = {0};
                int valread = recv(sd, buffer, BUFFER_SIZE - 1, 0);
                
                // 연결 종료 또는 오류
                if (valread <= 0) {
                    // 클라이언트 정보 가져오기
                    struct sockaddr_in peer_addr;
                    socklen_t peer_len = sizeof(peer_addr);
                    getpeername(sd, (struct sockaddr*)&peer_addr, &peer_len);
                    
                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(peer_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
                    
                    if (valread == 0) {
                        log_to_file("클라이언트 연결 종료 (IP: %s, 포트: %d, 소켓: %d)\n", 
                               client_ip, ntohs(peer_addr.sin_port), sd);
                    } else {
                        log_to_file("클라이언트 읽기 오류: %s (IP: %s, 포트: %d, 소켓: %d)\n", 
                               strerror(errno), client_ip, ntohs(peer_addr.sin_port), sd);
                    }
                    
                    // 소켓 닫기
                    close_socket_safely(&sd);
                    client_sockets[i] = -1;
                    
                    // 단일 클라이언트 모드에서는 서버 종료
                    if (single_client_mode) {
                        log_to_file("단일 클라이언트 모드: 클라이언트 종료로 인해 서버를 종료합니다.\n");
                        running = 0;
                        break;
                    }
                }
                // 데이터 수신 성공
                else {
                    buffer[valread] = '\0';
                    log_to_file("클라이언트 %d로부터 명령 수신: %s\n", i, buffer);
                    
                    // EXIT 명령 처리 (개별 클라이언트 종료)
                    if (strcmp(buffer, "EXIT") == 0) {
                        log_to_file("클라이언트 %d가 EXIT 명령을 보냈습니다. 연결을 종료합니다.\n", i);
                        const char *exit_response = "OK EXIT";
                        send(sd, exit_response, strlen(exit_response), 0);
                        
                        // 소켓 닫기
                        close_socket_safely(&sd);
                        client_sockets[i] = -1;
                        
                        // 단일 클라이언트 모드에서는 서버 종료
                        if (single_client_mode) {
                            log_to_file("단일 클라이언트 모드: EXIT 명령으로 인해 서버를 종료합니다.\n");
                            running = 0;
                            break;
                        }
                    }
                    // 일반 명령 처리
                    else {
                        process_command(sd, buffer);
                    }
                }
            }
        }
    }
    
    // 모든 클라이언트 소켓 닫기
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] > 0) {
            close_socket_safely(&client_sockets[i]);
        }
    }
    
    // 서버 소켓 닫기
    close_socket_safely(&server_fd);
    
    // 하드웨어 정리
    if (hardware_initialized) {
        hardware_cleanup();
    }
    
    log_to_file("서버 종료\n");
    
    // 로그 파일 닫기
    if (log_file != NULL) {
        fprintf(log_file, "--- 서버 종료: %s ---\n", get_time_string());
        fclose(log_file);
        log_file = NULL;
    }
    
    return EXIT_SUCCESS;
}
