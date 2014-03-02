#include "LuaUtilities.h"
#include <lua/lua.hpp>

LuaUtilities::LuaUtilities() {
  lua = luaL_newstate();
  luaL_openlibs(lua);
}

LuaUtilities::~LuaUtilities() {
  lua_close(lua);
}

void LuaUtilities::LoadFile(const std::string& filename) {
  int error = luaL_loadfile(lua, filename.c_str());
  lua_pcall(lua, 0, LUA_MULTRET, 0);
}

void LuaUtilities::Call(const std::string& function) {
  lua_getglobal(lua, function.c_str());
  int error = lua_pcall(lua, 0, LUA_MULTRET, 0);
  int x = lua_tonumber(lua, -1);
}