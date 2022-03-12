/*	
	lall
	File:/src/lall.c
	Date:2022.03.13
	By MIT License.
	Copyright(c) 2022 lall developers.All rights reserved.
*/

#include<stdio.h>
#include<stdlib.h>

#include<lua.h>
#include<lauxlib.h>

int luaopen_lall_Stream(lua_State *state);

const struct {
	const char *modName;
	const char *subModName;
	int (*loader)(lua_State *state);
} gModList[] = {
			{
				"lall.Stream",
				"Stream",
				luaopen_lall_Stream
			},
			{
				NULL,
				NULL,
				NULL
			}
	       };

int luaopen_lall(lua_State *state)
{
	lua_newtable(state);

	for (int i = 0;gModList[i].modName;i++) {
		luaL_requiref(state,gModList[i].modName,
			      gModList[i].loader,1);
		lua_setfield(state,-2,gModList[i].subModName);
	}

	return 1;
}
