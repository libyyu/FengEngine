
-- add target
target("luasocket")
    set_options("luajit", "genproj", "FengEngineBundle")
    add_rules("lua", "path")
    set_kind("static")
    -- add the common source files
    if is_plat("windows") then
        add_files("src/*.c|serial.c|unix.c|usocket.c") 
    else
        add_files("src/*.c|wsocket.c") 
    end
    if has_config("luajit") then
        add_includedirs("../../luajit/src")
    else
        add_includedirs("../../lua-5.1.5/src")
    end

    if is_plat("windows") then
        set_languages("c89")
    else
        set_languages("gnu99")
    end