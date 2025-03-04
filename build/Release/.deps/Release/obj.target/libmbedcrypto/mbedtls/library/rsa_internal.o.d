cmd_Release/obj.target/libmbedcrypto/mbedtls/library/rsa_internal.o := cc -o Release/obj.target/libmbedcrypto/mbedtls/library/rsa_internal.o ../mbedtls/library/rsa_internal.c '-DMBEDTLS_CONFIG_FILE="node-mbedtls-config.h"' '-DNAPI_CPP_EXCEPTIONS=1' '-DNODE_GYP_MODULE_NAME=libmbedcrypto' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-DV8_DEPRECATION_WARNINGS' '-DV8_IMMINENT_DEPRECATION_WARNINGS' '-D_GLIBCXX_USE_CXX11_ABI=1' '-D_DARWIN_USE_64_BIT_INODE=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DOPENSSL_NO_PINSHARED' '-DOPENSSL_THREADS' -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/src -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/config -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/openssl/include -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/uv/include -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/zlib -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/v8/include -I../mbedtls/include -I../configs -I../mbedtls/include/mbedtls  -O3 -gdwarf-2 -fvisibility=hidden -mmacosx-version-min=10.15 -arch arm64 -Wall -Wendif-labels -W -Wno-unused-parameter -fno-strict-aliasing -MMD -MF ./Release/.deps/Release/obj.target/libmbedcrypto/mbedtls/library/rsa_internal.o.d.raw   -c
Release/obj.target/libmbedcrypto/mbedtls/library/rsa_internal.o: \
  ../mbedtls/library/rsa_internal.c ../configs/node-mbedtls-config.h \
  ../mbedtls/include/mbedtls/check_config.h \
  ../mbedtls/include/mbedtls/rsa.h ../mbedtls/include/mbedtls/bignum.h \
  ../mbedtls/include/mbedtls/md.h \
  ../mbedtls/include/mbedtls/rsa_internal.h
../mbedtls/library/rsa_internal.c:
../configs/node-mbedtls-config.h:
../mbedtls/include/mbedtls/check_config.h:
../mbedtls/include/mbedtls/rsa.h:
../mbedtls/include/mbedtls/bignum.h:
../mbedtls/include/mbedtls/md.h:
../mbedtls/include/mbedtls/rsa_internal.h:
