#include "napi.h"
#include "tomcrypt.h"
#include "aes.h"
#include "sha256.h"

#define BC_KEY_SIZE_BYTES 16

class EncryptorAddon : public Napi::Addon<EncryptorAddon>
{
public:
    EncryptorAddon(Napi::Env env, Napi::Object exports)
    {
        DefineAddon(exports, {InstanceMethod("Setup", &EncryptorAddon::Setup),
                              InstanceMethod("InitializeEncryption", &EncryptorAddon::InitializeEncryption),
                              InstanceMethod("Encrypt", &EncryptorAddon::Encrypt)});
    }

private:
    Napi::Value SetupEnc(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t status = 0;

        if (register_cipher(&aes_desc) == -1)
        {
            status = -1;
        }

        if (register_hash(&sha256_desc) == -1)
        {
            status = -1;
        }

        this->cipher_idx = find_cipher("aes");
        if (this->cipher_idx == -1)
        {
            status = -1;
        }

        // Generate random IV, CHANGE THIS LATER
        for (int i = 0; i < 16; i++)
        {
            random_iv[i] = i * 100 / 6;
        }

        return Napi::Number::New(info.Env(), status);
    }

    Napi::Value InitializeEncryption(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        bool success = true;

        // This function takes 2 params (phase: Number, buffer: ArrayBuffer)
        if (info.Length() != 2)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::ArrayBuffer keyBytes = info[0].As<Napi::ArrayBuffer>();
        Napi::ArrayBuffer ivBytes = info[1].As<Napi::ArrayBuffer>();

        unsigned char key[BC_KEY_SIZE_BYTES];
        unsigned char IV[BC_KEY_SIZE_BYTES];

        memcpy(key, keyBytes.Data(), BC_KEY_SIZE_BYTES);
        memcpy(IV, ivBytes.Data(), BC_KEY_SIZE_BYTES);

        if ((cbc_start(this->cipher_idx, IV, key, BC_KEY_SIZE_BYTES, 0, &cbc)) != CRYPT_OK)
        {
            success = false;
        }

        return Napi::Boolean::New(env, success);
    }

    Napi::Value Encrypt(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        bool success = true;

        if (info.Length() != 3)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }
        if (!info[2].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::ArrayBuffer inBytes = info[0].As<Napi::ArrayBuffer>();
        Napi::ArrayBuffer outBytes = info[1].As<Napi::ArrayBuffer>();
        Napi::ArrayBuffer keyBytes = info[0].As<Napi::ArrayBuffer>();

        unsigned long y;
        double f = 0;

        // Plain text will be encypted into ciphertext
        unsigned char ciphertext[512];
        // fixed length key (hash of user entered key).. what we actually use
        unsigned char key[BC_KEY_SIZE_BYTES];
        // initialization vector
        unsigned char IV[BC_KEY_SIZE_BYTES];
        // i/o block size
        unsigned char inbuf[512];

        memset(ciphertext, 0, sizeof(ciphertext));

        memcpy(key, keyBytes.Data(), BC_KEY_SIZE_BYTES);
        memcpy(IV, random_iv, BC_KEY_SIZE_BYTES);

        if ((cbc_start(this->cipher_idx, IV, key, BC_KEY_SIZE_BYTES, 0, &cbc)) != CRYPT_OK)
        {
            success = false;
        }
        else
        {
            do
            {
                memset(inbuf, 0, sizeof(inbuf));
                memcpy(inbuf, inBytes.Data(), inBytes.ByteLength());
                y = inBytes.ByteLength();

                if (y % BC_KEY_SIZE_BYTES != 0)
                {
                    f = (double)y;

                    y = ceil(f / BC_KEY_SIZE_BYTES) * BC_KEY_SIZE_BYTES;
                }

                if ((cbc_encrypt(inbuf, ciphertext, y, &cbc)) != CRYPT_OK)
                {
                    success = false;
                }

            } while (success && (y == sizeof(inbuf)));
        }

        return Napi::Number::New(env, success);
    }

    int32_t cipher_idx;
    uint8_t random_iv[16];

    symmetric_CBC cbc;
};

// The macro announces that instances of the class `ExampleAddon` will be
// created for each instance of the add-on that must be loaded into Node.js.
NODE_API_NAMED_ADDON(NODE_GYP_MODULE_NAME, EncryptorAddon)