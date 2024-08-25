cmd_Release/tomcrypt.a := rm -f Release/tomcrypt.a && ./gyp-mac-tool filter-libtool libtool  -static -o Release/tomcrypt.a Release/obj.target/libtomcrypt/crypt/misc/crypt/*.o
