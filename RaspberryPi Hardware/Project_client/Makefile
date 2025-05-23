# 컴파일러 및 플래그 설정
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS_SERVER = -lwiringPi -lpthread
LDFLAGS_CLIENT = -lpthread

# 소스 파일 목록
SERVER_SRC = src/server.c src/daemon.c
HARDWARE_SRC = src/hardware.c
CLIENT_SRC = client.c

# 실행 파일 및 라이브러리 이름
SERVER = server
CLIENT = client
HARDWARE_LIB = libhardware.so

# 기본 타겟 - 서버와 클라이언트 모두 빌드
all: $(HARDWARE_LIB) $(SERVER) $(CLIENT)

# 공유 라이브러리 빌드
$(HARDWARE_LIB): $(HARDWARE_SRC)
	$(CC) -fPIC -c $(HARDWARE_SRC) -Iinclude $(CFLAGS)
	$(CC) -shared -o $@ hardware.o $(LDFLAGS_SERVER)
	@rm -f hardware.o
	@echo "하드웨어 라이브러리 빌드 완료: $@"

# 서버 빌드 (rpath 옵션 추가)
$(SERVER): $(SERVER_SRC) $(HARDWARE_LIB)
	$(CC) $(CFLAGS) -Iinclude $(SERVER_SRC) -o $@ -L. -lhardware -Wl,-rpath,$(shell pwd) $(LDFLAGS_SERVER)
	@echo "서버 빌드 완료: $@"

# 클라이언트 빌드
$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $@ $(LDFLAGS_CLIENT)
	@echo "클라이언트 빌드 완료: $@"

# 클린 타겟
clean:
	rm -f $(SERVER) $(CLIENT) $(HARDWARE_LIB) *.o
	@echo "정리 완료"

# 서버만 빌드
server: $(HARDWARE_LIB) $(SERVER)

# 클라이언트만 빌드
client: $(CLIENT)

# 서버 실행 타겟
run-server: $(SERVER)
	@echo "서버 실행 중..."
	@./$(SERVER)

# 서버 데몬 모드 실행 타겟
run-daemon: $(SERVER)
	@echo "서버를 데몬 모드로 실행 중..."
	@./$(SERVER) -d

# 서버 단일 클라이언트 모드 실행 타겟
run-single: $(SERVER)
	@echo "서버를 단일 클라이언트 모드로 실행 중..."
	@./$(SERVER) -s

# 클라이언트 실행 타겟
run-client: $(CLIENT)
	@if [ -z "$(IP)" ]; then \
		echo "사용법: make run-client IP=<서버IP주소>"; \
	else \
		echo "클라이언트 실행 중..."; \
		./$(CLIENT) $(IP); \
	fi

# 라이브러리 시스템 설치 타겟
install-lib: $(HARDWARE_LIB)
	@echo "하드웨어 라이브러리를 시스템에 설치 중..."
	@sudo cp $(HARDWARE_LIB) /usr/local/lib/
	@sudo ldconfig
	@echo "라이브러리 설치 완료"

# 타겟이 파일이 아님을 명시
.PHONY: all clean server client run-server run-daemon run-single run-client install-lib
