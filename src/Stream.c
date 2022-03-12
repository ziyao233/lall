/*
	lall
	File:/src/Stream.c
	Date:2022.03.11
	By MIT License.
	Copyright (c) 2022 lall developers.All rights reserved.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>

#include<lua.h>
#include<lauxlib.h>

#include"Stream.h"

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
	String,Integer,Integer lall.Stream:read(length);
	length:
		Integer		Size in byte
		String
				"a"	Read until EOF
				"l"	Read a line
	Return:
		String		Data that has been read
		Integer		The size of the valid data
		Integer		errno
*/
static int interface_stream_read(lua_State *state)
{
	Lall_Stream *s = lall_stream_cdata(state,1);

	if (lua_isinteger(state,2)) {
		int size = lua_tointeger(state,2);
		luaL_Buffer buffer;
		uint8_t *buf = (uint8_t*)luaL_buffinitsize(state,&buffer,
							   size);

		int err = 0;
		int count = 0;
		while (count < size) {
			ssize_t retVal = read(s->fd,(void*)(buf + count),
					      size);

			// A signal came or an error occured.
			if (retVal < 0) {
				err = retVal;
				break;
			} else {
				count += retVal;
			}
		}

		luaL_pushresultsize(&buffer,count);
		lua_pushinteger(state,count);
		lua_pushinteger(state,err);
	} else if (lua_isstring(state,2)) {
		char arg = *lua_tostring(state,2);

		if (arg == 'a') {
			luaL_Buffer buf;
			luaL_buffinit(state,&buf);

			uint8_t tmp[1024];
			int err;
			size_t length = 0;
			while (1) {
				ssize_t retVal = read(s->fd,
						      (void*)tmp,
						      1024);
				err = errno;
				if (retVal < 0) {
					err = errno;
					break;
				}
				length += retVal;
				luaL_addlstring(&buf,(const char*)tmp,
						retVal);

				if (retVal != 1024)
					break;
			}
			luaL_pushresult(&buf);
			lua_pushinteger(state,(lua_Integer)length);
			lua_pushinteger(state,err);
		} else {
			luaL_error(state,"Unknow opreator %c",arg);
		}
	} else {
		luaL_typeerror(state,2,"string or integer");
	}

	return 3;
}

/*
	Integer,Integer lall.stream:read(String data,
					 Integer offset,
					 Integer size)
	data:	Anything can be casted into String
		(e.g. with __tostring method)
	offset:	The start position of data in data
	size:	The size of data.
		The full part of data will be written if nil is passed in.
	Return:
		Integer		The size of the data that has been
				written
		Integer		errno
*/
static int interface_stream_write(lua_State *state)
{
	Lall_Stream *stream = lall_stream_cdata(state,1);

	size_t size;
	const char *data = lua_tolstring(state,2,&size);

	size_t start = lua_tointeger(state,2);
	size = (lua_gettop(state) < 3	||
		lua_isnil(state,3))	?
			size		:
			lua_tointeger(state,3);

	ssize_t tmp = write(stream->fd,
			    (void*)(data + start - 1),
			    size - start + 1);
	int err = errno;

	lua_pushinteger(state,(lua_Integer)tmp);
	lua_pushinteger(state,err);

	return 2;
}

static int interface_stream_option(lua_State *state)
{
	Lall_Stream *stream = lall_stream_cdata(state,1);
	
	if (lua_gettop(state) == 1) {
		lua_pushinteger(state,fcntl(stream->fd,F_GETFL,0));
		lua_pushinteger(state,errno);
		return 2;
	} else {
		int opt = luaL_checkinteger(state,2);
		fcntl(stream->fd,F_SETFD,opt);
		lua_pushinteger(state,errno);
		return 1;
	}
}

static const struct luaL_Reg streamPrototype[] = {
		{
			"fd",		interface_stream_fd,
		},
		{
			"read",		interface_stream_read,
		},
		{
			"write",	interface_stream_write,
		},
		{
			"option",	interface_stream_option,
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
int luaopen_lall_Stream(lua_State *state)
{
	luaL_newmetatable(state,"lall.Stream");
	luaL_newlib(state,streamPrototype);
	lua_setfield(state,-2,"__index");

	luaL_newlib(state,modStream);
	return 1;
}

/*
	For the girl I love...
	Although life is full of pain...be brave plz...
	...
	Everything will certainly get better.
*/
