
-----------------------------------------------------
option("socket_common")
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
target("luasocket")
    add_options("socket_common")
    add_options("luajit", "genxcode")
    -- make as a static library
    set_kind("static")
    --设置最终文件目录
    set_targetdir("../../libs/$(plat)/$(arch)")
    set_objectdir("./build/$(plat)/$(arch)/.objs")
    --设置链接目录
    add_linkdirs("../../libs/$(plat)/$(arch)")
    -- add the common source files
    if is_plat("windows") then
        add_files("src/*.c|serial.c|unix.c|usocket.c") 
    else
        add_files("src/*.c|wsocket.c") 
    end
    add_options("luajit")
    if is_option("luajit") then
		add_includedirs("../../luajit/src")
	else
		add_includedirs("../../lua-5.1.5/src")
    end