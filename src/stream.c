/*
	lall
	File:/src/stream.c
	Date:2022.01.16
	By MIT License.
	Copyright (c) 2022 lall developers.All rights reserved.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include<unistd.h>
#include<errno.h>

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
	Integer lall.Stream:fd();
*/
static int interface_stream_fd(lua_State *state)
{
	lua_pushinteger(state,lall_stream_cdata(state,1)->fd);
	return 1;
}

/*
	String lall.Stream:read(length);
	length:
		Integer		Size in byte
		String
				"a"	Read until EOF
				"l"	Read a line
*/
static int interface_stream_read(lua_State *state)
{
	Lall_Stream *s = lall_stream_cdata(state,1);

	if (lua_isinteger(state,2)) {
		int size = lua_tointeger(state,2);
		luaL_Buffer buffer;
		uint8_t *buf = (uint8_t*)luaL_buffinitsize(state,&buffer,
							   size);

		for (int count = 0;count < size;) {
			ssize_t retVal = read(s->fd,(void*)(buf + count),
					      size);

			// A signal came or an error occured.
			if (retVal < 0) {
				if (errno != EINTR) {
					luaL_error(state,
	"An error occured while reading,errno %d",
	errno);
				}
			} else {
				count += retVal;
			}
		}

		luaL_pushresultsize(&buffer,size);
	} else if (lua_isstring(state,2)) {
		char arg = *lua_tostring(state,2);

		if (arg == 'a') {
			luaL_Buffer buf;
			luaL_buffinit(state,&buf);

			uint8_t tmp[1024];
			while (1) {
				ssize_t retVal = read(s->fd,
						      (void*)tmp,
						      1024);
				if (retVal != 1024) {
					if (!errno) {
						luaL_addlstring(&buf,
							(const char*)tmp,
							retVal);
						break;
					} else if (errno != EINTR) {
						luaL_error(state,
	"An error occured while reading,errno: %d",errno);
					}
				}
				luaL_addlstring(&buf,(const char*)tmp,1024);
			}
			luaL_pushresult(&buf);
		} else {
			luaL_error(state,"Unknow opreator %c",arg);
		}
	} else {
		luaL_typeerror(state,2,"string or integer");
	}

	return 1;
}

static const struct luaL_Reg streamPrototype[] = {
		{
			"fd",		interface_stream_fd,
		},
		{
			"read",		interface_stream_read,
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
