# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := libtomcrypt
DEFS_Debug := \
	'-DMBEDTLS_CONFIG_FILE="node-mbedtls-config.h"' \
	'-DNAPI_CPP_EXCEPTIONS=1' \
	'-DNODE_GYP_MODULE_NAME=libtomcrypt' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_IMMINENT_DEPRECATION_WARNINGS' \
	'-D_GLIBCXX_USE_CXX11_ABI=1' \
	'-D_DARWIN_USE_64_BIT_INODE=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DOPENSSL_NO_PINSHARED' \
	'-DOPENSSL_THREADS' \
	'-DLTC_RIJNDAEL=1' \
	'-DDEBUG' \
	'-D_DEBUG' \
	'-DV8_ENABLE_CHECKS'

# Flags passed to all source files.
CFLAGS_Debug := \
	-O0 \
	-gdwarf-2 \
	-fvisibility=hidden \
	-mmacosx-version-min=10.15 \
	-arch \
	arm64 \
	-Wall \
	-Wendif-labels \
	-W \
	-Wno-unused-parameter

# Flags passed to only C files.
CFLAGS_C_Debug := \
	-fno-strict-aliasing

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-std=gnu++17 \
	-stdlib=libc++ \
	-fno-rtti \
	-fno-exceptions \
	-fno-strict-aliasing

# Flags passed to only ObjC files.
CFLAGS_OBJC_Debug :=

# Flags passed to only ObjC++ files.
CFLAGS_OBJCC_Debug :=

INCS_Debug := \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/src \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/config \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/openssl/include \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/uv/include \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/zlib \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/v8/include \
	-I$(srcdir)/libtomcrypt/src/headers

DEFS_Release := \
	'-DMBEDTLS_CONFIG_FILE="node-mbedtls-config.h"' \
	'-DNAPI_CPP_EXCEPTIONS=1' \
	'-DNODE_GYP_MODULE_NAME=libtomcrypt' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-DV8_DEPRECATION_WARNINGS' \
	'-DV8_IMMINENT_DEPRECATION_WARNINGS' \
	'-D_GLIBCXX_USE_CXX11_ABI=1' \
	'-D_DARWIN_USE_64_BIT_INODE=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DOPENSSL_NO_PINSHARED' \
	'-DOPENSSL_THREADS' \
	'-DLTC_RIJNDAEL=1'

# Flags passed to all source files.
CFLAGS_Release := \
	-O3 \
	-gdwarf-2 \
	-fvisibility=hidden \
	-mmacosx-version-min=10.15 \
	-arch \
	arm64 \
	-Wall \
	-Wendif-labels \
	-W \
	-Wno-unused-parameter

# Flags passed to only C files.
CFLAGS_C_Release := \
	-fno-strict-aliasing

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-std=gnu++17 \
	-stdlib=libc++ \
	-fno-rtti \
	-fno-exceptions \
	-fno-strict-aliasing

# Flags passed to only ObjC files.
CFLAGS_OBJC_Release :=

# Flags passed to only ObjC++ files.
CFLAGS_OBJCC_Release :=

INCS_Release := \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/include/node \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/src \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/config \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/openssl/openssl/include \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/uv/include \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/zlib \
	-I/Users/ryanwilson/Library/Caches/node-gyp/18.16.1/deps/v8/include \
	-I$(srcdir)/libtomcrypt/src/headers

OBJS := \
	$(obj).target/$(TARGET)/libtomcrypt/src/ciphers/aes/aes_desc.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/ciphers/aes/aes.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/misc/crypt/crypt_register_cipher.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/misc/crypt/crypt_register_hash.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/misc/crypt/crypt_cipher_is_valid.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/misc/crypt/crypt_find_cipher.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/modes/cbc/cbc_start.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/modes/cbc/cbc_encrypt.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/misc/crypt/crypt_cipher_descriptor.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/misc/crypt/crypt_hash_descriptor.o \
	$(obj).target/$(TARGET)/libtomcrypt/src/hashes/sha2/sha256.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))
$(OBJS): GYP_OBJCFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE)) $(CFLAGS_OBJC_$(BUILDTYPE))
$(OBJS): GYP_OBJCXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE)) $(CFLAGS_OBJCC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-mmacosx-version-min=10.15 \
	-arch \
	arm64 \
	-L$(builddir) \
	-stdlib=libc++

LIBTOOLFLAGS_Debug :=

LDFLAGS_Release := \
	-mmacosx-version-min=10.15 \
	-arch \
	arm64 \
	-L$(builddir) \
	-stdlib=libc++

LIBTOOLFLAGS_Release :=

LIBS :=

$(builddir)/tomcrypt.a: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/tomcrypt.a: LIBS := $(LIBS)
$(builddir)/tomcrypt.a: GYP_LIBTOOLFLAGS := $(LIBTOOLFLAGS_$(BUILDTYPE))
$(builddir)/tomcrypt.a: TOOLSET := $(TOOLSET)
$(builddir)/tomcrypt.a: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,alink)

all_deps += $(builddir)/tomcrypt.a
# Add target alias
.PHONY: libtomcrypt
libtomcrypt: $(builddir)/tomcrypt.a

# Add target alias to "all" target.
.PHONY: all
all: libtomcrypt

# Add target alias
.PHONY: libtomcrypt
libtomcrypt: $(builddir)/tomcrypt.a

# Short alias for building this static library.
.PHONY: tomcrypt.a
tomcrypt.a: $(builddir)/tomcrypt.a

# Add static library to "all" target.
.PHONY: all
all: $(builddir)/tomcrypt.a

