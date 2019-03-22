
add_subdirs("luasocket", "lua-cjson")
-- add target
target("thirdpart")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("lua", "path")
    set_kind("static")
    add_deps("luasocket", "cjson")
    add_files("lpeg/*.c")
    add_files("pb.c")
	add_files("slua.c")
    add_files("i64lib/*.c")
    if has_config("luajit") then
        add_includedirs("../luajit/src")
    else
        add_includedirs("../lua-5.1.5/src")
    end