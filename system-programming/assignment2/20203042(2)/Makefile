CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = 20203042.c
OBJS = $(SRCS:.c=.o)
TARGET = 20203042

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)