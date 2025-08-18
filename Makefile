CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 \
         -Wall -Wextra -Werror -Wno-unused-parameter -fno-asm

SRCS = src/main.c src/prompt.c src/parser.c src/validate.c src/cmdrunner.c \
		src/cmdarchive.c \
		src/hop_util.c \
		src/reveal_util.c \
		src/log_util.c \
		src/activities_util.c \
		src/ping_util.c \
		src/fgbg_util.c

OBJS = $(SRCS:.c=.o)
INC  = -Iinclude
TARGET = shell.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
