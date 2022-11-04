namespace Kitsune::API::AppletEvents {
    static char* GetKeyName(char *dst, int sym) {
        strcpy(dst, SDL_GetKeyName(sym));
        char *p = dst;
        while (*p) {
            *p = tolower(*p);
            p++;
        }
        return dst;
    }

    static int PollEvent(lua_State *L) {
        char buffer[16];
        SDL_Event e;
    poll:
        if ( !SDL_PollEvent(&e) ) {
            return 0;
        }
        switch (e.type) {
            case SDL_QUIT:
                lua_pushstring(L, "AppletQuit");
                return 1;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    lua_pushstring(L, "AppletResized");
                    lua_pushnumber(L, e.window.data1);
                    lua_pushnumber(L, e.window.data2);
                    return 3;
                } else if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
                    lua_pushstring(L, "AppletExposed");
                    return 1;
                } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) { // For compatibility
                    SDL_FlushEvent(SDL_KEYDOWN);
                }
                goto poll;
            case SDL_KEYDOWN:
                lua_pushstring(L, "AppletKeyDown");
                lua_pushstring(L, GetKeyName(buffer, e.key.keysym.sym));
                return 2;
            case SDL_KEYUP:
                lua_pushstring(L, "AppletKeyUp");
                lua_pushstring(L, GetKeyName(buffer, e.key.keysym.sym));
                return 2;
            case SDL_TEXTINPUT:
                lua_pushstring(L, "AppletText");
                lua_pushstring(L, e.text.text);
                return 2;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == 1) { SDL_CaptureMouse((SDL_bool)1); }
                lua_pushstring(L, "AppletMouseDown");
                lua_pushnumber(L, e.button.button);
                lua_pushnumber(L, e.button.x);
                lua_pushnumber(L, e.button.y);
                lua_pushnumber(L, e.button.clicks);
                return 5;
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == 1) { SDL_CaptureMouse((SDL_bool)0); }
                lua_pushstring(L, "AppletMouseUp");
                lua_pushnumber(L, e.button.button);
                lua_pushnumber(L, e.button.x);
                lua_pushnumber(L, e.button.y);
                return 4;
            case SDL_MOUSEMOTION:
                lua_pushstring(L, "AppletMouseMoved");
                lua_pushnumber(L, e.motion.x);
                lua_pushnumber(L, e.motion.y);
                lua_pushnumber(L, e.motion.xrel);
                lua_pushnumber(L, e.motion.yrel);
                return 5;
            case SDL_MOUSEWHEEL:
                lua_pushstring(L, "AppletMouseScroll");
                lua_pushnumber(L, e.wheel.y);
                return 2;
            default:
                goto poll;
        }
        return 0;
    }

    static const luaL_Reg lib[] = {
        {"PollEvent", PollEvent},
        {NULL, NULL}
    };

    static int LuaOpen(lua_State *L) {
        luaL_newlib(L, lib);
        return 1;
    }
}