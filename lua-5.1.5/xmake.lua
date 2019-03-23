-- add target
target("lua51")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("path")
    set_kind("static")
    add_files("src/*.c|lua.c|luac.c|print.c") 
	if is_plat("windows") then
		add_defines("WIN32_LEAN_AND_MEAN"
            , "_CRT_SECURE_NO_WARNINGS"
            , "LUA_BUILD_AS_DLL")
	end

target("luac")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("path")
    add_deps("lua51")
    set_kind("binary")
    set_targetdir("../bin/$(plat)/$(arch)")
    add_files("src/luac.c","src/print.c")
    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "LUA_BUILD_AS_DLL")
    elseif is_plat("macosx") then
        add_ldflags("-all_load", "-pagezero_size 10000", "-image_base 100000000")
    end

target("lua")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("path")
    add_deps("lua51")
    set_kind("binary")
    set_targetdir("../bin/$(plat)/$(arch)")
    add_files("src/lua.c")
    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "LUA_BUILD_AS_DLL")
    elseif is_plat("macosx") then
        add_ldflags("-all_load", "-pagezero_size 10000", "-image_base 100000000")
    end


