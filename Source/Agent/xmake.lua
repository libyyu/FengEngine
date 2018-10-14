target("curlAgent")
    set_kind("static")
    -- set warning all and disable error
    set_warnings("all")
    --c++ 11
    set_languages("cxx11")
    add_files("*.cpp|*Demo*")
    add_includedirs("../../3rd/curl/include")


target("test")
    set_kind("binary")
    -- set warning all and disable error
    set_warnings("all")
    --c++ 11
    set_languages("cxx11")
    add_files("HTTPDemo.cpp")
    add_deps("curlAgent")
    add_links("curlAgent", "curl", "ldap")
    add_linkdirs("../../3rd/curl/lib/macosx")
    add_includedirs("../../3rd/curl/include")
