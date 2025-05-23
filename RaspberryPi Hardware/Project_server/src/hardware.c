#include "../include/common.h"
#include "../include/hardware.h"
#include <wiringPi.h>
#include <softPwm.h>  // 소프트웨어 PWM을 위한 헤더 추가
#include <softTone.h>  // 소프트웨어 톤 생성을 위한 헤더 추가
#include <syslog.h>
#include <pthread.h>

// PWM 관련 상수 정의
#define PWM_RANGE 100  // PWM 범위 (0-100)
#define PWM_LOW 25     // 낮은 밝기 (25%)
#define PWM_MEDIUM 50  // 중간 밝기 (50%)
#define PWM_HIGH 100   // 높은 밝기 (100%)

// 7세그먼트 BCD 패턴 (0-9)
static const int SEGMENT_PATTERNS[10][4] = {
    {0, 0, 0, 0},  // 0 - BCD 코드 0000
    {0, 0, 0, 1},  // 1 - BCD 코드 0001
    {0, 0, 1, 0},  // 2 - BCD 코드 0010
    {0, 0, 1, 1},  // 3 - BCD 코드 0011
    {0, 1, 0, 0},  // 4 - BCD 코드 0100
    {0, 1, 0, 1},  // 5 - BCD 코드 0101
    {0, 1, 1, 0},  // 6 - BCD 코드 0110
    {0, 1, 1, 1},  // 7 - BCD 코드 0111
    {1, 0, 0, 0},  // 8 - BCD 코드 1000
    {1, 0, 0, 1}   // 9 - BCD 코드 1001
};

// 7세그먼트 핀 배열 (A, B, C, D)
static const int SEGMENT_PINS[4] = {SEG_A, SEG_B, SEG_C, SEG_D};

// 음계 주파수 정의
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_REST 0   // 쉼표 (무음)

// 멜로디 정의 (음계와 지속 시간(ms))
typedef struct {
    int frequency;  // 주파수 (Hz)
    int duration;   // 지속 시간 (ms)
} Note;

// 파헬벨의 캐논변주곡 주요 부분 (약 30초)
static const Note CANON_MELODY[] = {
    // 첫 번째 부분
    {NOTE_D4, 250},  // D4
    {NOTE_FS4, 250}, // F#4
    {NOTE_A4, 250},  // A4
    {NOTE_D5, 250},  // D5
    {NOTE_CS5, 250}, // C#5
    {NOTE_A4, 250},  // A4
    {NOTE_B4, 250},  // B4
    {NOTE_FS4, 250}, // F#4
    
    {NOTE_G4, 250},  // G4
    {NOTE_A4, 250},  // A4
    {NOTE_B4, 250},  // B4
    {NOTE_G4, 250},  // G4
    {NOTE_A4, 250},  // A4
    {NOTE_FS4, 250}, // F#4
    {NOTE_G4, 250},  // G4
    {NOTE_E4, 250},  // E4
    
    // 두 번째 부분
    {NOTE_D4, 250},  // D4
    {NOTE_FS4, 250}, // F#4
    {NOTE_A4, 250},  // A4
    {NOTE_D5, 250},  // D5
    {NOTE_FS5, 250}, // F#5
    {NOTE_A5, 250},  // A5
    {NOTE_G5, 250},  // G5
    {NOTE_FS5, 250}, // F#5
    
    {NOTE_E5, 250},  // E5
    {NOTE_CS5, 250}, // C#5
    {NOTE_D5, 250},  // D5
    {NOTE_FS4, 250}, // F#4
    {NOTE_B4, 250},  // B4
    {NOTE_A4, 250},  // A4
    {NOTE_G4, 250},  // G4
    {NOTE_FS4, 250}, // F#4
    
    // 세 번째 부분
    {NOTE_E4, 250},  // E4
    {NOTE_E5, 250},  // E5
    {NOTE_D5, 250},  // D5
    {NOTE_CS5, 250}, // C#5
    {NOTE_B4, 250},  // B4
    {NOTE_A4, 250},  // A4
    {NOTE_G4, 250},  // G4
    {NOTE_FS4, 250}, // F#4
    
    {NOTE_G4, 250},  // G4
    {NOTE_FS4, 250}, // F#4
    {NOTE_E4, 250},  // E4
    {NOTE_D4, 250},  // D4
    {NOTE_G4, 250},  // G4
    {NOTE_FS4, 250}, // F#4
    {NOTE_G4, 250},  // G4
    {NOTE_A4, 250},  // A4
    
    // 네 번째 부분
    {NOTE_D5, 250},  // D5
    {NOTE_FS5, 250}, // F#5
    {NOTE_A5, 250},  // A5
    {NOTE_G5, 250},  // G5
    {NOTE_FS5, 250}, // F#5
    {NOTE_D5, 250},  // D5
    {NOTE_E5, 250},  // E5
    {NOTE_CS5, 250}, // C#5
    
    {NOTE_B4, 250},  // B4
    {NOTE_CS5, 250}, // C#5
    {NOTE_D5, 250},  // D5
    {NOTE_A4, 250},  // A4
    {NOTE_B4, 250},  // B4
    {NOTE_FS4, 250}, // F#4
    {NOTE_D4, 250},  // D4
    {NOTE_FS4, 250}, // F#4
    
    // 다섯 번째 부분
    {NOTE_A4, 250},  // A4
    {NOTE_CS5, 250}, // C#5
    {NOTE_D5, 250},  // D5
    {NOTE_FS5, 250}, // F#5
    {NOTE_A5, 250},  // A5
    {NOTE_G5, 250},  // G5
    {NOTE_FS5, 250}, // F#5
    {NOTE_E5, 250},  // E5
    
    {NOTE_D5, 250},  // D5
    {NOTE_CS5, 250}, // C#5
    {NOTE_B4, 250},  // B4
    {NOTE_A4, 250},  // A4
    {NOTE_G4, 250},  // G4
    {NOTE_FS4, 250}, // F#4
    {NOTE_E4, 250},  // E4
    {NOTE_D4, 500},  // D4 (마지막 음은 길게)
};

#define MELODY_LENGTH (sizeof(CANON_MELODY) / sizeof(Note))

// 부저 재생 관련 변수
static volatile int buzzer_playing = 0;
static pthread_t buzzer_thread;
static volatile int buzzer_thread_running = 0;

// 조도센서 자동 제어 관련 변수
static volatile int auto_light_enabled = 0;
static pthread_t auto_light_thread;
static volatile int auto_light_thread_running = 0;

// 7세그먼트 카운트다운 관련 변수
static volatile int countdown_running = 0;
static pthread_t countdown_thread;
static volatile int countdown_thread_running = 0;
static volatile int countdown_value = 0;

// 스위치 인터럽트 관련 변수
static volatile int last_switch_time = 0;
static volatile int switch_debounce_time = 200; // 디바운싱 시간 (ms)

// 로그 출력 함수
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
    
    syslog(LOG_INFO, "[%s] %s", level_str, message);
}

// 현재 시간을 문자열로 반환하는 함수
char* get_time_string() {
    static char time_str[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    return time_str;
}

// 부저 재생 스레드 함수
void* buzzer_thread_func(void* arg) {
    (void)arg;  // 사용하지 않는 매개변수
    
    printf("부저 재생 스레드 시작\n");
    log_message(CUSTOM_LOG_INFO, "부저 재생 스레드 시작");
    
    for (int i = 0; i < MELODY_LENGTH && buzzer_thread_running; i++) {
        // 현재 음표의 주파수로 소리 내기
        printf("음표 %d/%d: 주파수 %d Hz, 지속 시간 %d ms\n", 
               i+1, (int)MELODY_LENGTH, CANON_MELODY[i].frequency, CANON_MELODY[i].duration);
        
        softToneWrite(BUZZER_PIN, CANON_MELODY[i].frequency);
        
        // 지정된 시간 동안 대기 (중간에 중지 가능하도록 짧은 간격으로 체크)
        int wait_time = 0;
        while (wait_time < CANON_MELODY[i].duration && buzzer_thread_running) {
            delay(10);  // 10ms 단위로 체크
            wait_time += 10;
        }
        
        // 스레드 중지 요청이 있으면 루프 종료
        if (!buzzer_thread_running) {
            break;
        }
    }
    
    // 부저 끄기
    softToneWrite(BUZZER_PIN, 0);
    
    printf("부저 재생 스레드 종료\n");
    log_message(CUSTOM_LOG_INFO, "부저 재생 스레드 종료");
    
    buzzer_playing = 0;
    
    return NULL;
}

// 조도센서 자동 제어 스레드 함수
void* auto_light_thread_func(void* arg) {
    (void)arg;  // 사용하지 않는 매개변수
    int prev_light_value = -1;  // 이전 조도센서 값 (초기값은 유효하지 않은 값)
    
    printf("조도센서 자동 제어 스레드 시작\n");
    log_message(CUSTOM_LOG_INFO, "조도센서 자동 제어 스레드 시작");
    
    while (auto_light_thread_running) {
        // 조도센서 값 읽기
        int light_value = light_sensor_read();
        
        // 값이 변경되었을 때만 LED 제어
        if (light_value != prev_light_value) {
            if (light_value == 0) {  // 빛이 감지되지 않음 (어두움)
                printf("자동 조명: 빛 미감지 (어두움) - LED 켜기\n");
                led_control(ON);
            } else {  // 빛이 감지됨 (밝음)
                printf("자동 조명: 빛 감지 (밝음) - LED 끄기\n");
                led_control(OFF);
            }
            prev_light_value = light_value;
        }
        
        // 0.5초 대기
        delay(500);
    }
    
    printf("조도센서 자동 제어 스레드 종료\n");
    log_message(CUSTOM_LOG_INFO, "조도센서 자동 제어 스레드 종료");
    
    return NULL;
}

// 7세그먼트 카운트다운 스레드 함수
void* countdown_thread_func(void* arg) {
    (void)arg;  // 사용하지 않는 매개변수
    
    printf("7세그먼트 카운트다운 스레드 시작, 초기값: %d\n", countdown_value);
    log_message(CUSTOM_LOG_INFO, "7세그먼트 카운트다운 스레드 시작");
    
    // 초기값 표시
    segment_display(countdown_value);
    
    // 카운트다운 시작
    while (countdown_thread_running && countdown_value > 0) {
        // 1초 대기
        delay(1000);
        
        // 값 감소
        countdown_value--;
        printf("카운트다운: %d\n", countdown_value);
        
        // 세그먼트에 표시
        segment_display(countdown_value);
        
        // 0이 되면 부저 울림
        if (countdown_value == 0) {
            printf("카운트다운 완료! 부저 울림\n");
            buzzer_control(ON);
            break;
        }
    }
    
    printf("7세그먼트 카운트다운 스레드 종료\n");
    log_message(CUSTOM_LOG_INFO, "7세그먼트 카운트다운 스레드 종료");
    
    countdown_running = 0;
    
    return NULL;
}

// 스위치 인터럽트 처리 함수
void switch_interrupt_handler(void) {
    // 현재 시간 가져오기
    int current_time = millis();
    
    // 디바운싱 처리
    if (current_time - last_switch_time < switch_debounce_time) {
        return;
    }
    
    // 마지막 인터럽트 시간 업데이트
    last_switch_time = current_time;
    
    printf("스위치 인터럽트 발생! 부저 상태 토글\n");
    
    // 부저 상태 토글
    if (buzzer_playing) {
        buzzer_control(OFF);
    } else {
        buzzer_control(ON);
    }
}

// 하드웨어 초기화 함수
int hardware_init(void) {
    log_message(CUSTOM_LOG_INFO, "하드웨어 초기화 시작...");
    
    // WiringPi 초기화
    if (wiringPiSetup() == -1) {
        log_message(CUSTOM_LOG_ERROR, "WiringPi 초기화 실패");
        return -1;
    }
    log_message(CUSTOM_LOG_INFO, "WiringPi 초기화 성공");
    
    // 각 장치 초기화
    led_init();
    buzzer_init();
    light_sensor_init();
    segment_init();
    switch_init();
    
    log_message(CUSTOM_LOG_INFO, "모든 장치 초기화 완료");
    return 0;
}

// 하드웨어 정리 함수
void hardware_cleanup(void) {
    log_message(CUSTOM_LOG_INFO, "하드웨어 정리 시작...");
    
    // 부저 재생 중지
    if (buzzer_playing) {
        buzzer_control(OFF);
    }
    
    // 조도센서 자동 제어 중지
    if (auto_light_enabled) {
        auto_light_control(0);
    }
    
    // 카운트다운 중지
    if (countdown_running) {
        countdown_stop();
    }
    
    led_cleanup();
    buzzer_cleanup();
    light_sensor_cleanup();
    segment_cleanup();
    switch_cleanup();
    
    log_message(CUSTOM_LOG_INFO, "하드웨어 정리 완료");
}

// LED 초기화 함수 - PWM 사용
void led_init(void) {
    // 소프트웨어 PWM 초기화
    if (softPwmCreate(LED_PIN, 0, PWM_RANGE) != 0) {
        printf("소프트웨어 PWM 초기화 실패\n");
        log_message(CUSTOM_LOG_ERROR, "소프트웨어 PWM 초기화 실패");
        
        // 실패 시 일반 디지털 출력으로 대체
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, LOW);
    } else {
        printf("소프트웨어 PWM 초기화 성공\n");
    }
    
    log_message(CUSTOM_LOG_INFO, "LED 초기화 완료");
}

// LED 제어 함수 - PWM 사용
void led_control(int state) {
    char log_msg[BUFFER_SIZE];
    snprintf(log_msg, sizeof(log_msg), "LED %s 시도 중...", state ? "켜기" : "끄기");
    log_message(CUSTOM_LOG_INFO, log_msg);
    
    // PWM 값 설정 (ON: 100%, OFF: 0%)
    softPwmWrite(LED_PIN, state ? PWM_HIGH : 0);
    
    snprintf(log_msg, sizeof(log_msg), "LED %s 완료", state ? "켜기" : "끄기");
    log_message(CUSTOM_LOG_INFO, log_msg);
}

// LED 밝기 제어 함수 - PWM 사용
void led_brightness(int level) {
    char log_msg[BUFFER_SIZE];
    int pwm_value = 0;
    
    // 밝기 레벨에 따라 PWM 값 설정 (1: 최저, 2: 중간, 3: 최대)
    switch (level) {
        case 1: // 최저 밝기
            pwm_value = PWM_LOW;
            snprintf(log_msg, sizeof(log_msg), "LED 밝기 설정: 최저 (PWM: %d%%)", pwm_value);
            break;
            
        case 2: // 중간 밝기
            pwm_value = PWM_MEDIUM;
            snprintf(log_msg, sizeof(log_msg), "LED 밝기 설정: 중간 (PWM: %d%%)", pwm_value);
            break;
            
        case 3: // 최대 밝기
            pwm_value = PWM_HIGH;
            snprintf(log_msg, sizeof(log_msg), "LED 밝기 설정: 최대 (PWM: %d%%)", pwm_value);
            break;
            
        default:
            pwm_value = PWM_MEDIUM;  // 기본값은 중간 밝기
            snprintf(log_msg, sizeof(log_msg), "잘못된 LED 밝기 레벨: %d, 중간 밝기로 설정", level);
            log_message(CUSTOM_LOG_WARNING, log_msg);
            break;
    }
    
    // PWM 값 설정
    softPwmWrite(LED_PIN, pwm_value);
    
    log_message(CUSTOM_LOG_INFO, log_msg);
    printf("LED 밝기 설정 완료: 레벨 %d (PWM: %d%%)\n", level, pwm_value);
}

// LED 정리 함수
void led_cleanup(void) {
    // LED 끄기
    softPwmWrite(LED_PIN, 0);
    
    // 소프트웨어 PWM 정리 (핀을 입력 모드로 변경)
    pinMode(LED_PIN, INPUT);
}

// 부저 초기화 함수
void buzzer_init(void) {
    // softTone 초기화
    if (softToneCreate(BUZZER_PIN) != 0) {
        printf("softTone 초기화 실패\n");
        log_message(CUSTOM_LOG_ERROR, "softTone 초기화 실패");
    } else {
        printf("softTone 초기화 성공\n");
        log_message(CUSTOM_LOG_INFO, "부저 초기화 완료");
    }
    
    // 초기에는 소리 없음
    softToneWrite(BUZZER_PIN, 0);
    buzzer_playing = 0;
}

// 부저 제어 함수
void buzzer_control(int state) {
    printf("부저 %s 명령 실행\n", state ? "켜기" : "끄기");
    
    if (state == ON) {
        // 이미 재생 중이면 중복 실행 방지
        if (buzzer_playing) {
            printf("부저가 이미 재생 중입니다.\n");
            return;
        }
        
        // 캐논변주곡 재생 스레드 시작
        printf("캐논변주곡 재생 시작...\n");
        
        buzzer_thread_running = 1;
        buzzer_playing = 1;
        
        // 스레드 생성
        if (pthread_create(&buzzer_thread, NULL, buzzer_thread_func, NULL) != 0) {
            printf("부저 재생 스레드 생성 실패\n");
            log_message(CUSTOM_LOG_ERROR, "부저 재생 스레드 생성 실패");
            buzzer_thread_running = 0;
            buzzer_playing = 0;
            return;
        }
        
        // 스레드를 분리 모드로 설정 (자동으로 리소스 정리)
        pthread_detach(buzzer_thread);
        
        printf("캐논변주곡 재생 스레드가 시작되었습니다.\n");
    } else {
        // 부저 끄기
        if (buzzer_playing) {
            printf("캐논변주곡 재생 중지 중...\n");
            buzzer_thread_running = 0;
            
            // 스레드가 종료될 때까지 짧게 대기
            delay(100);
            
            // 부저 소리 즉시 중지
            softToneWrite(BUZZER_PIN, 0);
            buzzer_playing = 0;
            
            printf("캐논변주곡 재생이 중지되었습니다.\n");
        } else {
            printf("부저가 이미 꺼져 있습니다.\n");
        }
    }
}

// 부저 정리 함수
void buzzer_cleanup(void) {
    // 부저 재생 중지
    if (buzzer_playing) {
        buzzer_thread_running = 0;
        delay(100);
    }
    
    softToneWrite(BUZZER_PIN, 0);
}

// 스위치 초기화 함수
void switch_init(void) {
    // 스위치 핀을 입력으로 설정 (내부 풀업 저항 사용)
    pinMode(SWITCH_PIN, INPUT);
    pullUpDnControl(SWITCH_PIN, PUD_UP);
    
    // 인터럽트 설정 (하강 에지에서 트리거)
    wiringPiISR(SWITCH_PIN, INT_EDGE_FALLING, &switch_interrupt_handler);
    
    printf("스위치 초기화 완료 (핀: %d)\n", SWITCH_PIN);
    log_message(CUSTOM_LOG_INFO, "스위치 초기화 완료");
}

// 스위치 상태 읽기 함수
int switch_read(void) {
    return digitalRead(SWITCH_PIN);
}

// 스위치 정리 함수
void switch_cleanup(void) {
    // 특별한 정리 작업 없음
    pinMode(SWITCH_PIN, INPUT);
}

// 조도센서 초기화 함수
void light_sensor_init(void) {
    pinMode(LIGHT_PIN, INPUT);
    log_message(CUSTOM_LOG_INFO, "조도센서 초기화 완료");
}

// 조도센서 읽기 함수
int light_sensor_read(void) {
    return digitalRead(LIGHT_PIN);
}

// 조도센서 자동 제어 함수
void auto_light_control(int enable) {
    if (enable && !auto_light_enabled) {
        // 자동 조명 제어 활성화
        printf("조도센서 자동 제어 활성화 중...\n");
        log_message(CUSTOM_LOG_INFO, "조도센서 자동 제어 활성화 중...");
        
        auto_light_thread_running = 1;
        
        // 스레드 생성
        if (pthread_create(&auto_light_thread, NULL, auto_light_thread_func, NULL) != 0) {
            printf("조도센서 자동 제어 스레드 생성 실패\n");
            log_message(CUSTOM_LOG_ERROR, "조도센서 자동 제어 스레드 생성 실패");
            auto_light_thread_running = 0;
            return;
        }
        
        // 스레드를 분리 모드로 설정
        pthread_detach(auto_light_thread);
        
        auto_light_enabled = 1;
        printf("조도센서 자동 제어가 활성화되었습니다.\n");
        log_message(CUSTOM_LOG_INFO, "조도센서 자동 제어가 활성화되었습니다.");
    } 
    else if (!enable && auto_light_enabled) {
        // 자동 조명 제어 비활성화
        printf("조도센서 자동 제어 비활성화 중...\n");
        log_message(CUSTOM_LOG_INFO, "조도센서 자동 제어 비활성화 중...");
        
        auto_light_thread_running = 0;
        
        // 스레드 종료 대기 (짧게)
        delay(600);
        
        auto_light_enabled = 0;
        printf("조도센서 자동 제어가 비활성화되었습니다.\n");
        log_message(CUSTOM_LOG_INFO, "조도센서 자동 제어가 비활성화되었습니다.");
    }
}

// 조도센서 자동 제어 상태 확인 함수
int is_auto_light_enabled(void) {
    return auto_light_enabled;
}

// 조도센서 정리 함수
void light_sensor_cleanup(void) {
    // 특별한 정리 작업 없음
}

// 7세그먼트 초기화 함수
void segment_init(void) {
    int i;
    
    // 모든 세그먼트 핀을 출력으로 설정
    for (i = 0; i < 4; i++) {
        pinMode(SEGMENT_PINS[i], OUTPUT);
        digitalWrite(SEGMENT_PINS[i], LOW);
    }
    
    log_message(CUSTOM_LOG_INFO, "7세그먼트 초기화 완료");
}

// 7세그먼트에 숫자 표시 함수
void segment_display(int number) {
    int i;
    char log_msg[BUFFER_SIZE];
    
    // 범위 검사 (0-9)
    if (number < 0 || number > 9) {
        log_message(CUSTOM_LOG_WARNING, "7세그먼트 표시 범위 초과 (0-9만 가능)");
        return;
    }
    
    snprintf(log_msg, sizeof(log_msg), "7세그먼트 표시 시작: %d", number);
    log_message(CUSTOM_LOG_INFO, log_msg);
    
    // 각 세그먼트 핀에 BCD 패턴 적용
    for (i = 0; i < 4; i++) {
        snprintf(log_msg, sizeof(log_msg), "세그먼트 핀 %d 설정: %d", 
                 SEGMENT_PINS[i], SEGMENT_PATTERNS[number][i]);
        log_message(CUSTOM_LOG_DEBUG, log_msg);
        
        digitalWrite(SEGMENT_PINS[i], SEGMENT_PATTERNS[number][i] ? HIGH : LOW);
    }
    
    snprintf(log_msg, sizeof(log_msg), "7세그먼트에 %d 표시 완료", number);
    log_message(CUSTOM_LOG_INFO, log_msg);
}

// 7세그먼트 카운트다운 시작 함수
void segment_countdown(int start_value) {
    char log_msg[BUFFER_SIZE];
    
    // 범위 검사 (1-9)
    if (start_value < 1 || start_value > 9) {
        snprintf(log_msg, sizeof(log_msg), "잘못된 카운트다운 시작값: %d (1-9 범위만 가능)", start_value);
        log_message(CUSTOM_LOG_WARNING, log_msg);
        return;
    }
    
    // 이미 실행 중인 카운트다운이 있으면 중지
    if (countdown_running) {
        countdown_stop();
    }
    
    // 카운트다운 값 설정
    countdown_value = start_value;
    
    snprintf(log_msg, sizeof(log_msg), "7세그먼트 카운트다운 시작: %d부터", start_value);
    log_message(CUSTOM_LOG_INFO, log_msg);
    
    // 카운트다운 스레드 시작
    countdown_thread_running = 1;
    countdown_running = 1;
    
    // 스레드 생성
    if (pthread_create(&countdown_thread, NULL, countdown_thread_func, NULL) != 0) {
        printf("카운트다운 스레드 생성 실패\n");
        log_message(CUSTOM_LOG_ERROR, "카운트다운 스레드 생성 실패");
        countdown_thread_running = 0;
        countdown_running = 0;
        return;
    }
    
    // 스레드를 분리 모드로 설정
    pthread_detach(countdown_thread);
    
    printf("7세그먼트 카운트다운이 시작되었습니다.\n");
}

// 7세그먼트 카운트다운 중지 함수
void countdown_stop(void) {
    if (countdown_running) {
        printf("7세그먼트 카운트다운 중지 중...\n");
        log_message(CUSTOM_LOG_INFO, "7세그먼트 카운트다운 중지 중...");
        
        // 스레드 종료 플래그 설정
        countdown_thread_running = 0;
        
        // 스레드 종료 대기 (충분한 시간 제공)
        int wait_count = 0;
        while (countdown_running && wait_count < 10) {
            delay(100);  // 100ms씩 대기
            wait_count++;
        }
        
        // 부저 끄기 (카운트다운 종료 시 부저가 울리고 있을 수 있음)
        if (buzzer_playing) {
            buzzer_control(OFF);
        }
        
        // 세그먼트 초기화 (0 표시)
        segment_display(0);
        
        countdown_running = 0;
        printf("7세그먼트 카운트다운이 중지되었습니다.\n");
        log_message(CUSTOM_LOG_INFO, "7세그먼트 카운트다운이 중지되었습니다.");
    } else {
        printf("카운트다운이 실행 중이지 않습니다.\n");
        log_message(CUSTOM_LOG_INFO, "카운트다운이 실행 중이지 않습니다.");
    }
}

// 7세그먼트 카운트 함수 (기존 함수)
void segment_count(int start, int end) {
    int i;
    char log_msg[BUFFER_SIZE];
    
    // 범위 검사
    if (start < 0 || start > 9 || end < 0 || end > 9) {
        log_message(CUSTOM_LOG_WARNING, "7세그먼트 카운트 범위 초과 (0-9만 가능)");
        return;
    }
    
    snprintf(log_msg, sizeof(log_msg), "7세그먼트 카운트 시작: %d부터 %d까지", start, end);
    log_message(CUSTOM_LOG_INFO, log_msg);
    
    // 오름차순 또는 내림차순 카운트
    if (start <= end) {
        for (i = start; i <= end; i++) {
            segment_display(i);
            sleep(1);
        }
    } else {
        for (i = start; i >= end; i--) {
            segment_display(i);
            sleep(1);
        }
    }
    
    log_message(CUSTOM_LOG_INFO, "7세그먼트 카운트 완료");
}

// 7세그먼트 정리 함수
void segment_cleanup(void) {
    int i;
    
    // 카운트다운 중지
    if (countdown_running) {
        countdown_stop();
    }
    
    // 모든 세그먼트 핀을 LOW로 설정
    for (i = 0; i < 4; i++) {
        digitalWrite(SEGMENT_PINS[i], LOW);
        pinMode(SEGMENT_PINS[i], INPUT);
    }
}
