C = gcc
CFLAGS = -Wall -g -O0
LDFLAGS = 

# Target executable
TARGET = monitor

# Source files
SRCS = server_main.c server_socket.c

# Object files
OBJS = $(SRCS:.c=.o)

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
			$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(OBJS) $(TARGET)

.PHONY: all clean

