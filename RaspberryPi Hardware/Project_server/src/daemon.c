#include "../include/daemon.h"
#include <sys/stat.h>
#include <fcntl.h>

// 데몬 프로세스 생성 함수 수정
void daemonize(const char* name) {
    pid_t pid;
    
    // 이미 데몬 프로세스인지 확인
    if (getppid() == 1) {
        // 이미 데몬 프로세스임
        return;
    }
    
    // 첫 번째 fork()로 부모 프로세스 종료
    pid = fork();
    
    if (pid < 0) {
        syslog(LOG_ERR, "fork() 실패");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) {
        // 부모 프로세스 종료
        exit(EXIT_SUCCESS);
    }
    
    // 자식 프로세스가 세션 리더가 됨
    if (setsid() < 0) {
        syslog(LOG_ERR, "setsid() 실패");
        exit(EXIT_FAILURE);
    }
    
    // 시그널 핸들러 설정
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    
    // 두 번째 fork()로 세션 리더 종료
    pid = fork();
    
    if (pid < 0) {
        syslog(LOG_ERR, "두 번째 fork() 실패");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) {
        // 세션 리더 종료
        exit(EXIT_SUCCESS);
    }
    
    // 파일 생성 마스크 변경
    umask(0);
    
    // 작업 디렉토리 변경 - 하드웨어 제어를 위해 주석 처리
    // chdir("/");
    
    // 표준 입출력 유지 (3번 이상의 파일 디스크립터만 닫음)
    int maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1) {
        maxfd = 1024;
    }
    
    for (int i = 3; i < maxfd; i++) {
        close(i);
    }
    
    // 표준 입출력 리다이렉션 제거 - 하드웨어 제어를 위해
    // open("/dev/null", O_RDWR);
    // dup(0);
    // dup(0);
    
    // syslog 열기
    openlog(name, LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "데몬 프로세스가 시작되었습니다.");
    
    // 현재 작업 디렉토리 로깅
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        syslog(LOG_NOTICE, "현재 작업 디렉토리: %s", cwd);
    } else {
        syslog(LOG_WARNING, "현재 작업 디렉토리를 가져올 수 없습니다");
    }
    
    // 권한 확인 로깅
    syslog(LOG_NOTICE, "현재 UID: %d, GID: %d, EUID: %d, EGID: %d", 
           getuid(), getgid(), geteuid(), getegid());
    
    // 환경 변수 로깅 및 설정 부분을 다음과 같이 수정:
    char* ld_library_path = getenv("LD_LIBRARY_PATH");
    if (ld_library_path != NULL) {
        syslog(LOG_NOTICE, "LD_LIBRARY_PATH: %s", ld_library_path);
    } else {
        syslog(LOG_WARNING, "LD_LIBRARY_PATH가 설정되지 않았습니다");
        // 현재 작업 디렉토리의 절대 경로로 환경 변수 설정
        char abs_path[1024];
        if (getcwd(abs_path, sizeof(abs_path)) != NULL) {
            setenv("LD_LIBRARY_PATH", abs_path, 1);
            syslog(LOG_NOTICE, "LD_LIBRARY_PATH를 절대 경로로 설정했습니다: %s", abs_path);
        } else {
            setenv("LD_LIBRARY_PATH", "/home/sso/work/Project", 1);
            syslog(LOG_NOTICE, "LD_LIBRARY_PATH를 기본 경로로 설정했습니다: /home/sso/work/Project");
        }
    }
}
