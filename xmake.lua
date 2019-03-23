-- project
set_project("FengEngine")
-- version
set_version("2.1.5")
-- set xmake min version
set_xmakever("2.1.2")
--disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing")
--add defines
add_defines("_GNU_SOURCE=1", "_FILE_OFFSET_BITS=64", "_LARGEFILE_SOURCE")
if is_plat("windows") then 
    -- add some defines only for windows
    add_cxflags("-EHsc", "-nologo")
    -- no msvcrt.lib
    add_ldflags("-nologo", "-NOD")
    if is_mode("debug") then
        add_cxflags("-MDd", "-FS")
        add_cxflags("-ZI")
        add_defines("_DEBUG","DEBUG")
    else
        add_cxflags("-MD", "-FS")
        add_defines("NDEBUG")
        add_cxflags("-Zi")
    end
    --添加全局宏
    add_defines(
    	"_CRT_SECURE_NO_WARNINGS"
        , "WIN32"
        , "__WINDOWS__"
    )
    --添加Windows全局依赖库
    add_links("ws2_32",
        "user32", 
        "kernel32",
        "gdi32",
        "winspool",
        "comdlg32",
        "advapi32",
        "shell32",
        "ole32",
        "oleaut32",
        "uuid",
        "odbc32",
        "odbccp32"
    )
elseif is_plat("android") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("__ANDROID__")
elseif is_plat("iphoneos") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("__IOS__")
elseif is_plat("macosx") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("__MACOSX__")
end
-- the debug mode
if is_mode("debug") then
    -- enable the debug symbols
    set_symbols("debug")
    -- disable optimization
    set_optimize("none")
elseif is_mode("release") then
    -- set the symbols visibility: hidden
    set_symbols("hidden")
    -- enable fastest optimization
    set_optimize("fastest")
    -- strip all symbols
    set_strip("all")
end
-- path
if is_kind("static") then
    set_targetdir("libs/$(plat)/$(arch)")
else
    set_targetdir("bin/$(plat)/$(arch)")
end
set_objectdir("$(buildir)/.objs")
-- options
option("luajit")
    set_default(false)
    set_showmenu(true)
option("genproj")
    set_default(false)
    set_showmenu(true)
option("FengEngineBundle")
    set_default(false)
    set_showmenu(true)
option_end()
if not has_config("luajit") then
    add_subfiles("lua-5.1.5") 
end
add_subfiles("3rd")

target("FengEngine")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("lua", "path")
	if is_plat("iphoneos") then
        set_kind("static")
    else
        set_kind("shared")
    end
    add_deps("thirdpart")

    if is_kind("static") then
        set_targetdir("libs/$(plat)/$(arch)")
    else
        set_targetdir("bin/$(plat)/$(arch)")
    end

    add_defines("_RZ_DLL_")
    add_includedirs("rzlib")
    add_files("Source/Common/*.cpp")
    add_files("Source/AnyLog/*.cpp")
    add_files("Source/*.cpp")

    if has_config("luajit") then
        add_includedirs("luajit/src")
        add_links("luajit")
    else
        add_includedirs("lua-5.1.5/src")
        add_deps("lua51")
    end


