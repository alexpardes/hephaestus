/*
** Lua binding: Launcher
** Generated automatically by tolua++-1.0.92 on 02/17/14 08:45:43.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
int tolua_Launcher_open (lua_State* tolua_S);

#include "Launcher.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Launcher");
 tolua_usertype(tolua_S,"std::string");
}

/* method: Instance of class  Launcher */
#ifndef TOLUA_DISABLE_tolua_Launcher_Launcher_Instance00
static int tolua_Launcher_Launcher_Instance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Launcher",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Launcher* tolua_ret = (Launcher*)  Launcher::Instance();
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Launcher");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Instance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: StartSinglePlayerGame of class  Launcher */
#ifndef TOLUA_DISABLE_tolua_Launcher_Launcher_StartSinglePlayerGame00
static int tolua_Launcher_Launcher_StartSinglePlayerGame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Launcher",0,&tolua_err) ||
 !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Launcher* self = (Launcher*)  tolua_tousertype(tolua_S,1,0);
  const std::string map = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'StartSinglePlayerGame'",NULL);
#endif
 {
  self->StartSinglePlayerGame(map);
 tolua_pushcppstring(tolua_S,(const char*)map);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'StartSinglePlayerGame'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HostGame of class  Launcher */
#ifndef TOLUA_DISABLE_tolua_Launcher_Launcher_HostGame00
static int tolua_Launcher_Launcher_HostGame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Launcher",0,&tolua_err) ||
 !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Launcher* self = (Launcher*)  tolua_tousertype(tolua_S,1,0);
  const std::string map = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int port = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HostGame'",NULL);
#endif
 {
  self->HostGame(map,port);
 tolua_pushcppstring(tolua_S,(const char*)map);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HostGame'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: JoinGame of class  Launcher */
#ifndef TOLUA_DISABLE_tolua_Launcher_Launcher_JoinGame00
static int tolua_Launcher_Launcher_JoinGame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Launcher",0,&tolua_err) ||
 !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
 !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Launcher* self = (Launcher*)  tolua_tousertype(tolua_S,1,0);
  const std::string hostname = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const std::string port = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'JoinGame'",NULL);
#endif
 {
  self->JoinGame(hostname,port);
 tolua_pushcppstring(tolua_S,(const char*)hostname);
 tolua_pushcppstring(tolua_S,(const char*)port);
 }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'JoinGame'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CancelHosting of class  Launcher */
#ifndef TOLUA_DISABLE_tolua_Launcher_Launcher_CancelHosting00
static int tolua_Launcher_Launcher_CancelHosting00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Launcher",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Launcher* self = (Launcher*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CancelHosting'",NULL);
#endif
 {
  self->CancelHosting();
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CancelHosting'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
int tolua_Launcher_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_module(tolua_S,"std",0);
 tolua_beginmodule(tolua_S,"std");
  tolua_cclass(tolua_S,"string","std::string","",NULL);
  tolua_beginmodule(tolua_S,"string");
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Launcher","Launcher","",NULL);
 tolua_beginmodule(tolua_S,"Launcher");
  tolua_function(tolua_S,"Instance",tolua_Launcher_Launcher_Instance00);
  tolua_function(tolua_S,"StartSinglePlayerGame",tolua_Launcher_Launcher_StartSinglePlayerGame00);
  tolua_function(tolua_S,"HostGame",tolua_Launcher_Launcher_HostGame00);
  tolua_function(tolua_S,"JoinGame",tolua_Launcher_Launcher_JoinGame00);
  tolua_function(tolua_S,"CancelHosting",tolua_Launcher_Launcher_CancelHosting00);
 tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 int luaopen_Launcher (lua_State* tolua_S) {
 return tolua_Launcher_open(tolua_S);
};
#endif

