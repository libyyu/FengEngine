-- add target
target("cjson")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("lua", "path")
    set_kind("static")
    -- add the common source files
    add_files("*.c|dtoa.c|g_fmt.c")
    if has_config("luajit") then
        add_includedirs("../../luajit/src")
    else
        add_includedirs("../../lua-5.1.5/src")
    end