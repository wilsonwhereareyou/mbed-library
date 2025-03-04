cmd_Release/obj.target/jpakeaddon/jpake.o := c++ -o Release/obj.target/jpakeaddon/jpake.o ../jpake.cpp '-DMBEDTLS_CONFIG_FILE="node-mbedtls-config.h"' '-DNAPI_CPP_EXCEPTIONS=1' '-DNODE_GYP_MODULE_NAME=jpakeaddon' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-DV8_DEPRECATION_WARNINGS' '-DV8_IMMINENT_DEPRECATION_WARNINGS' '-D_GLIBCXX_USE_CXX11_ABI=1' '-D_DARWIN_USE_64_BIT_INODE=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DOPENSSL_NO_PINSHARED' '-DOPENSSL_THREADS' '-DNAPI_CPP_EXCEPTIONS' '-DBUILDING_NODE_EXTENSION' -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/src -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/config -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/openssl/include -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/uv/include -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/zlib -I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/v8/include -I../libtomcrypt/src/headers -I../mbedtls/include/mbedtls -I../mbedtls/include -I../configs -I../node_modules/node-addon-api  -O3 -gdwarf-2 -fvisibility=hidden -mmacosx-version-min=10.7 -arch arm64 -Wall -Wendif-labels -W -Wno-unused-parameter -std=gnu++17 -stdlib=libc++ -fno-rtti -fno-strict-aliasing -MMD -MF ./Release/.deps/Release/obj.target/jpakeaddon/jpake.o.d.raw   -c
Release/obj.target/jpakeaddon/jpake.o: ../jpake.cpp \
  ../node_modules/node-addon-api/napi.h \
  /Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/node_api.h \
  /Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/js_native_api.h \
  /Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/js_native_api_types.h \
  /Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/node_api_types.h \
  ../node_modules/node-addon-api/napi-inl.h \
  ../node_modules/node-addon-api/napi-inl.deprecated.h \
  ../mbedtls/include/mbedtls/ecjpake.h ../configs/node-mbedtls-config.h \
  ../mbedtls/include/mbedtls/check_config.h \
  ../mbedtls/include/mbedtls/ecp.h ../mbedtls/include/mbedtls/bignum.h \
  ../mbedtls/include/mbedtls/md.h ../mbedtls/include/mbedtls/ctr_drbg.h \
  ../mbedtls/include/mbedtls/aes.h ../mbedtls/include/mbedtls/entropy.h \
  ../mbedtls/include/mbedtls/sha512.h \
  ../libtomcrypt/src/headers/tomcrypt.h \
  ../libtomcrypt/src/headers/tomcrypt_custom.h \
  ../libtomcrypt/src/headers/tomcrypt_cfg.h \
  ../libtomcrypt/src/headers/tomcrypt_macros.h \
  ../libtomcrypt/src/headers/tomcrypt_cipher.h \
  ../libtomcrypt/src/headers/tomcrypt_hash.h \
  ../libtomcrypt/src/headers/tomcrypt_mac.h \
  ../libtomcrypt/src/headers/tomcrypt_prng.h \
  ../libtomcrypt/src/headers/tomcrypt_pk.h \
  ../libtomcrypt/src/headers/tomcrypt_math.h \
  ../libtomcrypt/src/headers/tomcrypt_misc.h \
  ../libtomcrypt/src/headers/tomcrypt_argchk.h \
  ../libtomcrypt/src/headers/tomcrypt_pkcs.h
../jpake.cpp:
../node_modules/node-addon-api/napi.h:
/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/node_api.h:
/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/js_native_api.h:
/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/js_native_api_types.h:
/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node/node_api_types.h:
../node_modules/node-addon-api/napi-inl.h:
../node_modules/node-addon-api/napi-inl.deprecated.h:
../mbedtls/include/mbedtls/ecjpake.h:
../configs/node-mbedtls-config.h:
../mbedtls/include/mbedtls/check_config.h:
../mbedtls/include/mbedtls/ecp.h:
../mbedtls/include/mbedtls/bignum.h:
../mbedtls/include/mbedtls/md.h:
../mbedtls/include/mbedtls/ctr_drbg.h:
../mbedtls/include/mbedtls/aes.h:
../mbedtls/include/mbedtls/entropy.h:
../mbedtls/include/mbedtls/sha512.h:
../libtomcrypt/src/headers/tomcrypt.h:
../libtomcrypt/src/headers/tomcrypt_custom.h:
../libtomcrypt/src/headers/tomcrypt_cfg.h:
../libtomcrypt/src/headers/tomcrypt_macros.h:
../libtomcrypt/src/headers/tomcrypt_cipher.h:
../libtomcrypt/src/headers/tomcrypt_hash.h:
../libtomcrypt/src/headers/tomcrypt_mac.h:
../libtomcrypt/src/headers/tomcrypt_prng.h:
../libtomcrypt/src/headers/tomcrypt_pk.h:
../libtomcrypt/src/headers/tomcrypt_math.h:
../libtomcrypt/src/headers/tomcrypt_misc.h:
../libtomcrypt/src/headers/tomcrypt_argchk.h:
../libtomcrypt/src/headers/tomcrypt_pkcs.h:
