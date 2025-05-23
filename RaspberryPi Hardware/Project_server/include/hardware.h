#ifndef HARDWARE_H
#define HARDWARE_H

#include "common.h"

// 핀 번호 정의 (WiringPi 핀 번호 사용)
#define LED_PIN 0       // GPIO 17 (WiringPi 핀 0)
#define BUZZER_PIN 2    // GPIO 27 (WiringPi 핀 2)
#define LIGHT_PIN 3     // GPIO 22 (WiringPi 핀 3)
#define SWITCH_PIN 5    // GPIO 24 (WiringPi 핀 5)

// 7세그먼트 핀 정의 (7447 IC 사용)
#define SEG_A 4         // 7447 IC의 A 입력 핀 (WiringPi 핀 4)
#define SEG_B 1         // 7447 IC의 B 입력 핀 (WiringPi 핀 1)
#define SEG_C 16        // 7447 IC의 C 입력 핀 (WiringPi 핀 16)
#define SEG_D 15        // 7447 IC의 D 입력 핀 (WiringPi 핀 15)

// LED 밝기 레벨 정의
#define LED_BRIGHTNESS_LOW 1    // 최저 밝기
#define LED_BRIGHTNESS_MID 2    // 중간 밝기
#define LED_BRIGHTNESS_HIGH 3   // 최대 밝기

// 하드웨어 초기화 및 정리 함수
int hardware_init(void);
void hardware_cleanup(void);

// LED 관련 함수
void led_init(void);
void led_control(int state);
void led_brightness(int level);
void led_cleanup(void);

// 부저 관련 함수
void buzzer_init(void);
void buzzer_control(int state);
void buzzer_cleanup(void);

// 조도센서 관련 함수
void light_sensor_init(void);
int light_sensor_read(void);
void light_sensor_cleanup(void);

// 조도센서 자동 제어 관련 함수
void auto_light_control(int enable);
int is_auto_light_enabled(void);

// 7세그먼트 관련 함수
void segment_init(void);
void segment_display(int number);
void segment_count(int start, int end);
void segment_countdown(int start_value);
void countdown_stop(void);
void segment_cleanup(void);

// 스위치 관련 함수
void switch_init(void);
int switch_read(void);
void setup_switch_interrupt(void);
void switch_toggle_mode(int enable);
int is_switch_toggle_mode(void);
void switch_cleanup(void);

#endif /* HARDWARE_H */
