/*
	lall
	File:/src/stream.c
	Date:2022.01.15
	By MIT License.
	Copyright (c) 2022 lall developers.All rights reserved.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<lua.h>
#include<lauxlib.h>

#include"stream.h"

Lall_Stream *lall_stream_new(lua_State *state,int fd)
{
	Lall_Stream *stream = (Lall_Stream*)
		lua_newuserdata(state,sizeof(Lall_Stream));

	luaL_getmetatable(state,"lall.Stream");
	lua_setmetatable(state,-2);

	stream->fd = fd;

	return stream;
}

static int interface_stream(lua_State *state)
{
	int fd = (int)luaL_checkinteger(state,1);
	lall_stream_new(state,fd);
	return 1;
}

/*
	The stream needs being on the top of the stack.
*/
static int interface_stream_fd(lua_State *state)
{
	lua_pushinteger(state,lall_stream_cdata(state,1)->fd);
	return 1;
}


static const struct luaL_Reg streamPrototype[] = {
		{
			"fd",		interface_stream_fd,
		},
		{
			NULL,		NULL
		}
	};

static const struct luaL_Reg modStream[] = {
		{
			"Stream",	interface_stream
		},
		{
			NULL,		NULL
		}
	};


/*
	The main chunk of module lall.stream
*/
int luaopen_lall_stream(lua_State *state)
{
	luaL_newmetatable(state,"lall.Stream");
	luaL_newlib(state,streamPrototype);
	lua_setfield(state,-2,"__index");

	luaL_newlib(state,modStream);
	return 1;
}
