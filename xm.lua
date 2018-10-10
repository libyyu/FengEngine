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
    add_cxflags(
		"-EHsc"
		, "-nologo"
	)
    -- no msvcrt.lib
    add_ldflags(
		"-nologo"
	    , "-NOD"
    )
    if is_mode("debug") then
        add_cxflags("-MDd", "-FS")
        if is_arch("x86") then
            add_cxflags("-ZI")
        else
            add_cxflags("-Zi")
        end
        add_defines("_DEBUG","DEBUG")
        --if not is_kind("static") then
        --    set_symbols("debug")
        -- end
    else
        add_cxflags("-Zi", "-MD", "-FS")
        add_defines("NDEBUG")
        --if not is_kind("static") then
        --    set_symbols("debug")
		--end
    end
    --添加全局宏
    add_defines(
    	"_CRT_SECURE_NO_WARNINGS"
    	, "WIN32"
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
    add_defines("__WINDOWS__")
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

option("luajit")
    set_default(false)
    set_showmenu(true)
option("genxcode")
    set_default(false)
    set_showmenu(true)
option("FengEngineBundle")
    set_default(false)
    set_showmenu(true)

if not is_option("luajit") then
    add_subfiles("lua-5.1.5/xmake.lua") 
end
add_subfiles("3rd/luasocket/xmake.lua"
    , "3rd/lua-cjson/xmake.lua")

target("FengEngine")
	if is_plat("iphoneos") then
        set_kind("static")
    else
        set_kind("shared")
    end
	--设置最终文件目录
	set_targetdir("./bin/$(plat)/$(arch)")
	--设置中间输出目录
	set_objectdir("./build/$(plat)/$(arch)/.objs")
	--设置链接目录
	add_linkdirs("./libs/$(plat)/$(arch)")

	add_options("luajit", "genxcode", "FengEngineBundle")
	if is_option("luajit") then
		add_includedirs("luajit/src")
		add_links("luajit")
	else
		add_includedirs("lua-5.1.5/src")
		add_deps("lua51")
        add_links("lua51")
    end
    add_deps("luasocket", "cjson")
    add_links("luasocket", "cjson")
    add_defines("_RZ_DLL_")
    add_includedirs("rzlib")
    --3rd
    add_files("3rd/lpeg/*.c")
    add_files("3rd/pb.c")
	add_files("3rd/slua.c")

    add_files("Source/Common/*.cpp")
    add_files("Source/AnyLog/*.cpp")
    add_files("Source/*.cpp")
	