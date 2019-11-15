
    --add_files("/usr/local/Cellar/openssl/1.0.2l/lib/libssl.a")
    --add_files("/usr/local/Cellar/openssl/1.0.2l/lib/libcrypto.a")
target("test")
    set_kind("binary")
    -- set warning all and disable error
    set_warnings("all")
    --c++ 11
    set_languages("cxx11")
    add_files("HTTPHelper.cpp", "test.cpp")
    add_links("curl")
