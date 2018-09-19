
-----------------------------------------------------
option("lua_common")
    -- disable c99(/TP) for windows
    if is_plat("windows") then
        set_languages("c89")
    else
        set_languages("gnu99")
    end
    -- set warning all and disable error
    set_warnings("all")
    set_default(true)
    set_showmenu(false)

-- add target
target("lua51")
    add_options("lua_common")
    -- make as a static library
    set_kind("static")
    --设置最终文件目录
    set_targetdir("../libs/$(plat)/$(arch)")
    set_objectdir("./build/$(plat)/$(arch)/.objs")
    --设置链接目录
    add_linkdirs("../libs/$(plat)/$(arch)")
    -- add the common source files
    add_files("src/*.c|lua.c|luac.c|print.c") 
	if is_plat("windows") then
		add_defines("WIN32_LEAN_AND_MEAN"
            , "_CRT_SECURE_NO_WARNINGS"
            , "LUA_BUILD_AS_DLL")
	end

target("luac")
    add_options("lua_common")
    add_deps("lua51")
    set_kind("binary")
    --设置最终文件目录
    set_targetdir("../bin/$(plat)/$(arch)")
    set_objectdir("./build/$(plat)/$(arch)/.objs")
    --设置链接目录
    add_linkdirs("../libs/$(plat)/$(arch)")
    add_files("src/luac.c","src/print.c")
    add_links("lua51")
    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "LUA_BUILD_AS_DLL")
    elseif is_plat("macosx") then
        add_ldflags("-all_load", "-pagezero_size 10000", "-image_base 100000000")
    end

target("lua")
    add_options("lua_common")
    add_deps("lua51")
    set_kind("binary")
    --设置最终文件目录
    set_targetdir("../bin/$(plat)/$(arch)")
    set_objectdir("./build/$(plat)/$(arch)/.objs")
    --设置链接目录
    add_linkdirs("../libs/$(plat)/$(arch)")
    add_files("src/lua.c")
    add_links("lua51")
    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "LUA_BUILD_AS_DLL")
    elseif is_plat("macosx") then
        add_ldflags("-all_load", "-pagezero_size 10000", "-image_base 100000000")
    end


