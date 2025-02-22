C = gcc
CFLAGS = -Wall -g

#定义库路径和库文件
SQLITE_LIB_DIR = /home/iot25/machenfeng/kank/sq_work/sqlite-src-3470200/build/.libs
SQLITE_LIB = -lsqlite3
PTHREAD_LIB = -lpthread

# 定义源文件和目标文件
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = monitor

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# 默认目标
all: $(BIN_DIR)/$(TARGET)

# 链接目标文件生成可执行文件
$(BIN_DIR)/$(TARGET): $(OBJS)
		@mkdir -p $(BIN_DIR)
		$(CC) $(CFLAGS) -o $@ $^ -L$(SQLITE_LIB_DIR) $(SQLITE_LIB) $(PTHREAD_LIB)

# 编译源文件生成目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
			@mkdir -p $(OBJ_DIR)
			$(CC) $(CFLAGS) -c -o $@ $<

# 清理生成的文件
clean:
		rm -rf $(OBJ_DIR) $(BIN_DIR)
.PHONY: all clean
