{
    "target_defaults": {
        "include_dirs": ["./mbedtls/include/", "./configs/", "./mbedtls/include/mbedtls/","./crypt/headers/"],
        "defines": [
            "MBEDTLS_CONFIG_FILE=\"node-mbedtls-config.h\"",
                "LTC_EASY=1",
                "LTC_DER=1",
            "NAPI_CPP_EXCEPTIONS=1"
        ]
    },
    "targets": [
        {
            "target_name": "jpakeaddon",
            "dependencies": ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except", "libmbedcrypto", "libtomcrypt"],
            "sources": [ "jpake.cpp" ],
        },
        {
            "target_name": "libmbedcrypto",
            "type": "static_library",
            "sources": [
                "./mbedtls/library/aes.c",
                "./mbedtls/library/aesni.c",
                "./mbedtls/library/arc4.c",
                "./mbedtls/library/aria.c",
                "./mbedtls/library/asn1parse.c",
                "./mbedtls/library/asn1write.c",
                "./mbedtls/library/base64.c",
                "./mbedtls/library/bignum.c",
                "./mbedtls/library/blowfish.c",
                "./mbedtls/library/camellia.c",
                "./mbedtls/library/ccm.c",
                "./mbedtls/library/chacha20.c",
                "./mbedtls/library/chachapoly.c",
                "./mbedtls/library/cipher.c",
                "./mbedtls/library/cipher_wrap.c",
                "./mbedtls/library/cmac.c",
                "./mbedtls/library/ctr_drbg.c",
                "./mbedtls/library/des.c",
                "./mbedtls/library/dhm.c",
                "./mbedtls/library/ecdh.c",
                "./mbedtls/library/ecdsa.c",
                "./mbedtls/library/ecjpake.c",
                "./mbedtls/library/ecp.c",
                "./mbedtls/library/ecp_curves.c",
                "./mbedtls/library/entropy.c",
                "./mbedtls/library/entropy_poll.c",
                "./mbedtls/library/error.c",
                "./mbedtls/library/gcm.c",
                "./mbedtls/library/havege.c",
                "./mbedtls/library/hkdf.c",
                "./mbedtls/library/hmac_drbg.c",
                "./mbedtls/library/md.c",
                "./mbedtls/library/md2.c",
                "./mbedtls/library/md4.c",
                "./mbedtls/library/md5.c",
                "./mbedtls/library/md_wrap.c",
                "./mbedtls/library/memory_buffer_alloc.c",
                "./mbedtls/library/nist_kw.c",
                "./mbedtls/library/oid.c",
                "./mbedtls/library/padlock.c",
                "./mbedtls/library/pem.c",
                "./mbedtls/library/pk.c",
                "./mbedtls/library/pk_wrap.c",
                "./mbedtls/library/pkcs12.c",
                "./mbedtls/library/pkcs5.c",
                "./mbedtls/library/pkparse.c",
                "./mbedtls/library/pkwrite.c",
                "./mbedtls/library/platform.c",
                "./mbedtls/library/platform_util.c",
                "./mbedtls/library/poly1305.c",
                "./mbedtls/library/ripemd160.c",
                "./mbedtls/library/rsa.c",
                "./mbedtls/library/rsa_internal.c",
                "./mbedtls/library/sha1.c",
                "./mbedtls/library/sha256.c",
                "./mbedtls/library/sha512.c",
                "./mbedtls/library/threading.c",
                "./mbedtls/library/timing.c",
                "./mbedtls/library/version.c",
                "./mbedtls/library/version_features.c",
                "./mbedtls/library/xtea.c",
                "./mbedtls/library/ecjpake.c",
                ]
    
        },
        {
            "target_name": "libtomcrypt",
            "type": "static_library",
            "sources": [
                "./crypt/misc/crypt/crypt_argchk.c"
            ]
        }
    ],
    'conditions': [
        ['OS=="mac"', {
            'cflags+': ['-fvisibility=hidden'],
            'xcode_settings': {
                'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES'
            }
        }]
    ]
}