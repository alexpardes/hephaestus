#pragma once
#include <string>

struct lua_State;

class LuaUtilities {
  public:
    LuaUtilities();
    ~LuaUtilities();
    void LoadFile(const std::string& filename);
    void Call(const std::string& function);

  private:
    lua_State* lua;
};