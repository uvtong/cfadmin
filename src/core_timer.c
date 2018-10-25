#include "core_socket.h"

/* === 超时器 === */
void
timeout_cb(EV_P_ ev_timer *timer, int revents){
	if (ev_have_watcher_userdata(timer)){
		lua_State *co = (lua_State *) ev_get_watcher_userdata(timer);
		lua_resume(co, NULL, lua_gettop(co) > 0 ? lua_gettop(co) - 1 : 0);
        timer->repeat = 0.;
        ev_timer_again(EV_DEFAULT_ timer);
	}
}

/* === 定时器 === */
void
repeat_cb(EV_P_ ev_timer *timer, int revents){
    if (ev_have_watcher_userdata(timer)){
        lua_State *co = (lua_State *) ev_get_watcher_userdata(timer);
        lua_resume(co, NULL, lua_gettop(co) > 0 ? lua_gettop(co) - 1 : 0);
    }
}



int
timer_stop(lua_State *L){
	ev_timer *timer = (ev_timer *) luaL_testudata(L, 1, "__TIMER__");
	if(timer) {
		lua_settop(L, 1);
		return 0;
	}
	timer->repeat = 0.;
   	ev_timer_again (EV_DEFAULT_ timer);
	return 1;
}

int
timer_start(lua_State *L){

	ev_timer *timer = (ev_timer *) luaL_testudata(L, 1, "__TIMER__");
	if(!timer) return 0;

	lua_Number timeout = luaL_checknumber(L, 2);

	lua_Number repeat = luaL_checknumber(L, 3);

	lua_State *co = lua_tothread(L, 4);
	if(!co) return 0;

	ev_set_watcher_userdata(timer, (void*)co);

	if (timeout > 0 && repeat > 0) {
        ev_init(timer, repeat_cb);
        timer->repeat = repeat;
    }else if (timeout > 0 && repeat <= 0) {
        ev_init(timer, timeout_cb);
        timer->repeat = timeout;
    }else if (timeout <= 0 && repeat > 0) {
        ev_init(timer, timeout_cb);
        timer->repeat = repeat;
	}

	ev_timer_again(EV_DEFAULT_ timer);

	lua_settop(L, 1);

	return 1;

}

int
timer_new(lua_State *L){
	ev_timer *timer = (ev_timer *) lua_newuserdata(L, sizeof(ev_timer));
	if(!timer) {
		lua_settop(L, 1);
		return 0;
	}
	luaL_setmetatable(L, "__TIMER__");
	return 1;
}