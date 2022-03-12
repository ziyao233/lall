/*
	lall
	File:/src/Stream.h
	Date:2022.01.15
	By MIT License.
	Copyright (c) 2022 lall developers.All rights reserved.
*/

#ifndef __LALL_STREAM_H_INC__
#define __LALL_STREAM_H_INC__

#include<lua.h>
#include<lauxlib.h>

typedef struct {
	int fd;
}Lall_Stream;

#define lall_stream_cdata(state,pos) ((Lall_Stream*)luaL_checkudata((state),(pos),"lall.Stream"))

Lall_Stream *lall_stream_new(lua_State *state,int fd);

#endif	// __LALL_STREAM_H_INC__
