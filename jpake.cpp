// #include <assert.h>
#include "napi.h"
#include "ecjpake.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "md.h"
#include "ecp.h"
#include "aes.h"
#include <stdio.h>
// #include "jpake.h"
#include "tomcrypt.h"
#define LEN_JPAKE_PHASE_BYTES 165
#define MAX_JPAKE_DATA_BYTES LEN_JPAKE_PHASE_BYTES * 2
#define LEN_EC_POINT 64
#define LEN_ZKP 32
#define LEN_CURVE_INFO 3
#define LEN_CURVE_INFO_BYTES 3
#define AES128_BLOCK_LEN 16
#define BITS_IN_BYTE 8
#define BC_KEY_SIZE_BYTES 16

typedef enum : uint8_t
{
    JPAKE_PHASE0,
    JPAKE_PHASE1,
    JPAKE_PHASE2,

    JPAKE_MAX_PHASES,
} JpakePhaseType;

struct ecPoint
{
    uint8_t type;
    uint8_t point[LEN_EC_POINT];
};

typedef struct
{
    // Point X
    uint8_t xLengthBytes;
    ecPoint x;
    // Zero Knowledge Proof Point
    uint8_t vLengthBytes;
    ecPoint v;
    // Zero Knowledge Proof Data
    uint8_t zkpLen;
    uint8_t zkp[LEN_ZKP];
} JpakeRoundData;

// Round 2 data with curve information expected by the client
struct JpakeRound2ToClient
{
    uint8_t curveInfo[LEN_CURVE_INFO];
    JpakeRoundData data;
};

class JPakeAddon : public Napi::Addon<JPakeAddon>
{
public:
    JPakeAddon(Napi::Env env, Napi::Object exports)
    {
        // In this constructor, we declare functions we make available
        // to Javascript
        DefineAddon(exports, {InstanceMethod("Setup", &JPakeAddon::Setup),
                              // Here we attach a subobject with the function Decrement
                              InstanceMethod("BuildRoundMessage", &JPakeAddon::BuildRoundMessage),
                              InstanceMethod("ValidateRoundMessage", &JPakeAddon::ValidateRoundMessage),
                              InstanceMethod("DeriveKeyMaterial", &JPakeAddon::DeriveKeyMaterial),
                              InstanceMethod("GetRandomBytes", &JPakeAddon::GetRandomBytes),
                              InstanceMethod("GenerateEncryptedChallenge", &JPakeAddon::GenerateEncryptedChallenge),
                              InstanceMethod("DecryptChallenge", &JPakeAddon::DecryptChallenge),
                              InstanceMethod("SetupEnc", &JPakeAddon::SetupEnc),
                              InstanceMethod("InitializeEncryption", &JPakeAddon::InitializeEncryption),
                              InstanceMethod("Encrypt", &JPakeAddon::Encrypt)});

        // Initialize Context
        mbedtls_ecjpake_init(&jpakeContext);
        mbedtls_ctr_drbg_init(&ctrContext);
        mbedtls_entropy_init(&entropyContext);

        unsigned char seed = 4;
        mbedtls_ctr_drbg_seed(&ctrContext, mbedtls_entropy_func, &entropyContext, &seed, MBEDTLS_CTR_DRBG_ENTROPY_LEN);
    }

private:
    // This function takes in a buffer of characters (Update ID)
    // Returns the status
    Napi::Value Setup(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        // if (info.Length() != 1)
        // {
        //     Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        //     return env.Null();
        // }

        // if (!info[0].IsArrayBuffer())
        // {        //     Napi::Error::New(info.Env(), "Expected an Array").ThrowAsJavaScriptException();
        // }

        // Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();

        updateId[0] = 51;
        updateId[1] = 51;
        updateId[2] = 51;
        updateId[3] = 3;

        // ArrayConsumer(reinterpret_cast<uint8_t *>(buf.Data()), buf.ByteLength());

        // return info.Env().Undefined();

        int32_t mbedErr = -1;
        mbedErr = mbedtls_ecjpake_setup(&jpakeContext, MBEDTLS_ECJPAKE_SERVER, MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_SECP256R1, updateId, 4);
        return Napi::Number::New(info.Env(), mbedErr);
    }

    // Using the created context, create the Jpake round 1 message
    // Return new data
    Napi::Value BuildRoundMessage(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t status = -1;
        bool validLen = true;
        // Params = Round, Phase, buf

        if (info.Length() != 3)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "Round must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "Phase must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }

        uint32_t round = Napi::Number::New(env, info[0].As<Napi::Number>());
        uint32_t phase = Napi::Number::New(env, info[1].As<Napi::Number>());
        Napi::ArrayBuffer buf = info[2].As<Napi::ArrayBuffer>();
        size_t bufSize = 330;
        size_t msgLen;

        switch (phase)
        {
        case JPAKE_PHASE0:
        {
            uint32_t minValidLen = LEN_JPAKE_PHASE_BYTES * 2 - 2;

            status = mbedtls_ecjpake_write_round_one(&jpakeContext, (unsigned char *)buf.Data(),
                                                     bufSize, &msgLen, mbedtls_ctr_drbg_random, &ctrContext);

            // If the round message was built successfully but one or both of the zkp's are 1 byte less than expected,
            // pad the message to make it the expected size (please see Jira ticket OXEN-3351 for more information)
            if ((((LEN_JPAKE_PHASE_BYTES * 2) > msgLen) && (minValidLen <= msgLen)) && (0 == status))
            {
                status = PadPhaseData((uint8_t *)buf.Data(), MAX_JPAKE_DATA_BYTES, JPAKE_PHASE0);
            }
            else if (((LEN_JPAKE_PHASE_BYTES * 2) < msgLen) || (minValidLen > msgLen))
            {
                validLen = false;
            }
            break;
        }
        case JPAKE_PHASE1:
        {
            status = PadPhaseData((uint8_t *)buf.Data(), LEN_JPAKE_PHASE_BYTES, JPAKE_PHASE1);
            break;
        }
        case JPAKE_PHASE2:
        {
            // Make sure we get message length
            status = mbedtls_ecjpake_write_round_two(&jpakeContext, (unsigned char *)buf.Data(), bufSize, &msgLen, mbedtls_ctr_drbg_random, &ctrContext);

            uint32_t minValidLen = LEN_JPAKE_PHASE_BYTES + LEN_CURVE_INFO_BYTES - 1;
            if ((minValidLen == msgLen) && (0 == status))
            {
                status = PadPhaseData((uint8_t *)buf.Data(), LEN_JPAKE_PHASE_BYTES + LEN_CURVE_INFO_BYTES, JPAKE_PHASE2);
            }
            else if (((LEN_JPAKE_PHASE_BYTES + LEN_CURVE_INFO_BYTES) < msgLen) || (minValidLen > msgLen))
            {
                validLen = false;
            }
            break;
        }
        default:
        {
            status = 1;
            break;
        }
        }

        if (false == validLen)
        {
            status = 2;
        }

        // if (status != 0)
        // {
        //     Napi::Error::New(info.Env(), "Jpake Error").ThrowAsJavaScriptException();
        // }

        return Napi::Number::New(info.Env(), status);
    }

    // Using the created context, create the Jpake round 1 message
    // Return new data
    Napi::Value ValidateRoundMessage(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t jpakeStatus = -1;

        // This function takes 2 params (phase: Number, buffer: ArrayBuffer)
        if (info.Length() != 2)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "Phase must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }

        uint32_t phase = Napi::Number::New(env, info[0].As<Napi::Number>());
        Napi::ArrayBuffer buf = info[1].As<Napi::ArrayBuffer>();

        switch (phase)
        {
        case JPAKE_PHASE0:
        {
            jpakeStatus = 0;
            break;
        }
        case JPAKE_PHASE1:
        {
            jpakeStatus = mbedtls_ecjpake_read_round_one(&jpakeContext, (unsigned char *)buf.Data(), 2 * LEN_JPAKE_PHASE_BYTES);
            break;
        }
        case JPAKE_PHASE2:
        {
            jpakeStatus = mbedtls_ecjpake_read_round_two(&jpakeContext, (unsigned char *)buf.Data(), LEN_JPAKE_PHASE_BYTES);
            break;
        }
        }

        return Napi::Number::New(info.Env(), jpakeStatus);
    }

    // Using the created context, create the Jpake round 1 message
    // Return new data
    Napi::Value DeriveKeyMaterial(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t mbedErr = -1;

        // This function takes in 3 params (keyBuf: ArrayBuffer, keyBufSize: Number, keyLen: Number)
        if (info.Length() != 3)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "key buffer must be an ArrayBuffer").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "Buffer Size must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "Key Length must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::ArrayBuffer keyBuf = info[0].As<Napi::ArrayBuffer>();
        uint32_t keyBufSize = Napi::Number::New(env, info[1].As<Napi::Number>());
        // uint32_t keyLength = Napi::Number::New(env, info[2].As<Napi::Number>());
        Napi::Number keyLength = info[2].As<Napi::Number>();

        mbedErr = mbedtls_ecjpake_derive_secret(&jpakeContext, (unsigned char *)keyBuf.Data(),
                                                (size_t)keyBufSize, (size_t *)&keyLength, mbedtls_ctr_drbg_random, &ctrContext);

        return Napi::Number::New(info.Env(), mbedErr);
    }

    uint32_t PadPhaseData(uint8_t *buf, size_t bufSize, JpakePhaseType phase)
    {
        JpakeRoundData unpaddedMessage;
        JpakeRoundData *paddedMessage;
        uint8_t *messageStart;
        uint8_t zkpShift = 0;
        uint32_t jpakeStatus = 0;

        if (JPAKE_PHASE2 == phase)
        {
            messageStart = &buf[LEN_CURVE_INFO_BYTES];
        }
        else
        {
            messageStart = buf;
        }

        paddedMessage = (JpakeRoundData *)messageStart;

        if (((bufSize >= sizeof(JpakeRound2ToClient)) || ((bufSize >= sizeof(JpakeRoundData)) && (phase != JPAKE_PHASE2))) && (NULL != buf))
        {
            memcpy(&unpaddedMessage, messageStart, sizeof(JpakeRoundData));

            if (unpaddedMessage.zkpLen < LEN_ZKP)
            {
                zkpShift = LEN_ZKP - unpaddedMessage.zkpLen;

                if (JPAKE_PHASE0 == phase)
                {
                    uint8_t *newLoc = messageStart + LEN_JPAKE_PHASE_BYTES;
                    uint8_t *oldLoc = messageStart + LEN_JPAKE_PHASE_BYTES - zkpShift;

                    memmove(newLoc, oldLoc, LEN_JPAKE_PHASE_BYTES);
                }

                // Shift the zkp over and pad in front of the zkp with zeroes
                memcpy((paddedMessage->zkp) + zkpShift, unpaddedMessage.zkp, unpaddedMessage.zkpLen);
                memset(paddedMessage->zkp, 0, zkpShift);

                // Adjust the zkp length appropriately
                paddedMessage->zkpLen += zkpShift;
            }
        }
        else
        {
            jpakeStatus = 1;
        }

        return jpakeStatus;
    }

    Napi::Value GetRandomBytes(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t mbedErr = -1;

        // This function takes 2 params (buffer: ArrayBuffer, bufferLength: Number)
        if (info.Length() != 2)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "Expected an Array").ThrowAsJavaScriptException();
            return env.Null();
        }
        if (!info[1].IsNumber())
        {
            Napi::TypeError::New(env, "Phase must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        uint32_t bufLength = Napi::Number::New(env, info[1].As<Napi::Number>());
        Napi::ArrayBuffer buf = info[0].As<Napi::ArrayBuffer>();

        mbedErr = mbedtls_ctr_drbg_random(&ctrContext, (unsigned char *)buf.Data(), bufLength);

        return Napi::Number::New(info.Env(), mbedErr);
    }

    Napi::Value GenerateEncryptedChallenge(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t mbedErr = -1;

        // This function takes 2 params (buffer: ArrayBuffer, bufferLength: Number)
        if (info.Length() != 4)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "Expected an Array for key").ThrowAsJavaScriptException();
            return env.Null();
        }
        if (!info[1].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array for text").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "textLength must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array for encryptedText").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::ArrayBuffer key = info[0].As<Napi::ArrayBuffer>();
        Napi::ArrayBuffer text = info[1].As<Napi::ArrayBuffer>();
        uint32_t textLength = Napi::Number::New(env, info[2].As<Napi::Number>());
        Napi::ArrayBuffer encryptedText = info[3].As<Napi::ArrayBuffer>();

        mbedtls_aes_context aesCtx;
        mbedtls_aes_init(&aesCtx);

        if ((textLength == AES128_BLOCK_LEN) &&
            (0 == mbedtls_aes_setkey_enc(&aesCtx, (unsigned char *)key.Data(), AES128_BLOCK_LEN * BITS_IN_BYTE)))
        {
            mbedErr = mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_ENCRYPT, (unsigned char *)text.Data(), (unsigned char *)encryptedText.Data());
        }

        return Napi::Number::New(info.Env(), mbedErr);
    }

    Napi::Value DecryptChallenge(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();

        int32_t mbedErr = -1;

        // This function takes 2 params (buffer: ArrayBuffer, bufferLength: Number)
        if (info.Length() != 4)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsArrayBuffer())
        {
            Napi::Error::New(info.Env(), "Expected an Array for key").ThrowAsJavaScriptException();
            return env.Null();
        }
        if (!info[1].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array for text").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[2].IsNumber())
        {
            Napi::TypeError::New(env, "textLength must be a Number").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[3].IsArrayBuffer())
        {
            Napi::TypeError::New(env, "Expected an Array for encryptedText").ThrowAsJavaScriptException();
            return env.Null();
        }

        Napi::ArrayBuffer key = info[0].As<Napi::ArrayBuffer>();
        Napi::ArrayBuffer text = info[1].As<Napi::ArrayBuffer>();
        uint32_t textLength = Napi::Number::New(env, info[2].As<Napi::Number>());
        Napi::ArrayBuffer encryptedText = info[3].As<Napi::ArrayBuffer>();

        mbedtls_aes_context aesCtx;
        mbedtls_aes_init(&aesCtx);

        if ((textLength == AES128_BLOCK_LEN) &&
            (0 == mbedtls_aes_setkey_dec(&aesCtx, (unsigned char *)key.Data(), AES128_BLOCK_LEN * BITS_IN_BYTE)))
        {
            mbedErr = mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_DECRYPT, (unsigned char *)text.Data(), (unsigned char *)encryptedText.Data());
        }

        return Napi::Number::New(info.Env(), mbedErr);
    }

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

    // Properties
    // uint32_t value = 40;
    uint8_t updateId[4];
    mbedtls_ecjpake_context jpakeContext;
    mbedtls_ctr_drbg_context ctrContext;
    mbedtls_entropy_context entropyContext;

    // Encryption
    int32_t cipher_idx;
    uint8_t random_iv[16];

    symmetric_CBC cbc;
};

// The macro announces that instances of the class `ExampleAddon` will be
// created for each instance of the add-on that must be loaded into Node.js.
NODE_API_NAMED_ADDON(NODE_GYP_MODULE_NAME, JPakeAddon)
