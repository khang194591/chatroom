CC 			:= gcc

INC_DIR 	:= ./include

SRC_DIRS 	:= ./src

BUILD_DIR 	:= ./build

LIB			:= ./lib

CFLAGS 		:= -g -Wall -I$(INC_DIR) -I$(LIB) -pthread

CLIENT 		:= $(BUILD_DIR)/client

SERVER 		:= $(BUILD_DIR)/server

COMMON_SRCS := $(shell find $(SRC_DIRS)/common -name '*.c')

COMMON_OBJS := $(COMMON_SRCS:%=$(BUILD_DIR)/%.o)

SERVER_SRCS := $(shell find $(SRC_DIRS)/server -name '*.c')

SERVER_OBJS := $(SERVER_SRCS:%=$(BUILD_DIR)/%.o)

CLIENT_SRCS := $(shell find $(SRC_DIRS)/client -name '*.c')

CLIENT_OBJS := $(CLIENT_SRCS:%=$(BUILD_DIR)/%.o)

all: $(CLIENT) $(SERVER)

$(CLIENT): $(COMMON_OBJS) $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(CLIENT_OBJS) -o $@ lib/libfdr.a
	
$(SERVER): $(COMMON_OBJS) $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(SERVER_OBJS) -o $@ lib/libfdr.a

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)