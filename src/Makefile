#	lall
#	File:/src/Makefile
#	Date:2021.01.15
#	By MIT License.
#	Copyright(C) 2022 Suote127.All rights reserved.

# Tools
CC		:= gcc -std=gnu99
COPY		:= cp
REMOVE		:= rm
ECHO		:= echo

# Files
FILE_OBJS	:= Stream.o lall.o
FILE_OUTPUT	:= lall.so

# Flags
FLAG_CC		:= -O0 -Wextra -Wall -g -fPIC

default:build

%.o:%.c
	@$(ECHO) [C99] $< $@
	@$(CC) $< -c $(FLAG_CC)

build:$(FILE_OBJS)
	@$(ECHO) [CC] $(FILE_OBJS) -shared -o $(FILE_OUTPUT)
	@$(CC) $(FILE_OBJS) -shared -o $(FILE_OUTPUT)

clean:
	@$(ECHO) [REMOVE] $(FILE_OBJS)
	@$(REMOVE) $(FILE_OBJS)

disbuild:clean
	@$(ECHO) [REMOVE] $(FILE_OUTPUT)
	@$(REMOVE) $(FILE_OUTPUT)
