xcrun -sdk macosx clang++ \
-o libs/macosx/x86_64/libFengEngine.dylib build/.objs/macosx/x86_64/release/Source/AndroidExport.cpp.o \
build/.objs/macosx/x86_64/release/Source/AnyLog/ILog.cpp.o \
build/.objs/macosx/x86_64/release/Source/PCH.cpp.o \
build/.objs/macosx/x86_64/release/Source/WindowsExport.cpp.o \
build/.objs/macosx/x86_64/release/Source/FLuaExport.cpp.o \
-Llibs/macosx/x86_64 \
-Wl -llua51 -lthirdpart -lcjson -lluasocket \
-ObjC -shared -fPIC -arch x86_64 \
-mmacosx-version-min=10.14 \
-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk \
-stdlib=libc++ -lz
