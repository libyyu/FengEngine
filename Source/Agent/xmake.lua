option("genxcode")
    set_default(false)
    set_showmenu(true)

target("curlAgent")
    set_kind("static")
    -- set warning all and disable error
    set_warnings("all")
    --c++ 11
    set_languages("cxx11")
    add_files("*.cpp|*Demo*")
    add_includedirs("../../3rd/curl/include")
    add_options("genxcode")
    if is_option("genxcode") then
        add_headers("*.hpp")
    else
        add_files("../../3rd/curl/lib/macosx/libcurl.a")
        if is_plat("macosx") then
            add_files("/usr/local/Cellar/openssl/1.0.2l/lib/libssl.a")
            add_files("/usr/local/Cellar/openssl/1.0.2l/lib/libcrypto.a")
        end
    end

target("test")
    set_kind("binary")
    -- set warning all and disable error
    set_warnings("all")
    --c++ 11
    set_languages("cxx11")
    add_files("HTTPDemo.cpp")
    add_links("curlAgent", "ldap")
    add_linkdirs("./lib/$(plat)")
    add_includedirs("../../3rd/curl/include")
    add_options("genxcode")
