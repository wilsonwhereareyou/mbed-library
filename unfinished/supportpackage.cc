#include "supportpackage.h"

#define BC_KEY_SIZE_BYTES 16
#define MAX_FILE_DATA_BYTES 241
#define DATA_SIZE_BYTES 2
#define DATA_CRC_BYTES 2

using namespace Napi;

SupportPackage::SupportPackage(const CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (register_cipher(&aes_desc) == -1)
    {
        Napi::TypeError::New(env, "Unable to set up AES")
            .ThrowAsJavaScriptException();
    }

    if (register_hash(&sha256_desc) == -1)
    {
        Napi::TypeError::New(env, "Unable to set up SHA256")
            .ThrowAsJavaScriptException();
    }

    this->cipher_idx = find_cipher("aes");
    if (this->cipher_idx == -1)
    {
        Napi::TypeError::New(env, "Unable to find cipher")
            .ThrowAsJavaScriptException();
    }

    // Generate random IV, CHANGE THIS LATER
    for (int i = 0; i < 16; i++)
    {
        random_iv[i] = i * 100 / 6;
    }
}

Napi::Value SupportPackage::SetupEncryption(const CallbackInfo &info)
{
    Napi::Env env = info.Env();

    bool success = true;

    // This function takes 1 params ()
    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Expected an Array").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::ArrayBuffer keyBytes = info[0].As<Napi::ArrayBuffer>();

    unsigned char key[BC_KEY_SIZE_BYTES];

    memcpy(key, keyBytes.Data(), BC_KEY_SIZE_BYTES);
    memcpy(this->appKey, keyBytes.Data(), keyBytes.ByteLength());

    if ((cbc_start(this->cipher_idx, random_iv, key, BC_KEY_SIZE_BYTES, 0, &cbc)) != CRYPT_OK)
    {
        success = false;
    }

    if (true == success)
    {
        isEncryptionInitialized = true;
    }

    return Napi::Boolean::New(info.Env(), success);
}

// Params
// Array of file data
// Length of array
Napi::Value SupportPackage::SetFileData(const Napi::CallbackInfo &info)
{
    // This function will encrypt the file data and store it so that we can
    // send to the pump in chunks
    Napi::Env env = info.Env();

    int32_t status = -1;

    // This function takes 1 params ()
    if (info.Length() != 3)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, status);
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "File Data must be a buffer").ThrowAsJavaScriptException();
        return Napi::Number::New(env, status);
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "File Length must be a number").ThrowAsJavaScriptException();
        return Napi::Number::New(env, status);
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Cert Type must be a number").ThrowAsJavaScriptException();
        return Napi::Number::New(env, status);
    }

    Napi::ArrayBuffer inputFile = info[0].As<Napi::ArrayBuffer>();
    size_t inputFileLength = info[1].As<Napi::Number>().Int32Value();

    this->dataType = info[2].As<Napi::Number>().Int32Value();

    status = Encrypt((uint8_t *)inputFile.Data(), inputFileLength, this->appKey);

    this->crc = Calc_CRC(this->encryptedFile.data(), this->encryptedFile.size());
    printf("CRC after encryption: %10d \n", this->crc);

    return Napi::Number::New(env, status);
}

typedef struct
{
    uint16_t dataSize;
    uint16_t dataCrc;
    uint8_t dataType;
} PkiTransferHeaderType;

Napi::Value SupportPackage::GetFileData(const CallbackInfo &info)
{
    Napi::Env env = info.Env();

    bool success = true;
    PkiTransferHeaderType transferHeader;

    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Output must be a buffer").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    Napi::ArrayBuffer outBuffer = info[0].As<Napi::ArrayBuffer>();

    transferHeader.dataSize = (uint16_t)this->encryptedFile.size();
    transferHeader.dataCrc = this->crc;
    transferHeader.dataType = this->dataType;

    memcpy(outBuffer.Data(), (void *)&transferHeader, sizeof(PkiTransferHeaderType));

    memcpy((uint8_t *)outBuffer.Data() + sizeof(PkiTransferHeaderType), this->encryptedFile.data(), this->encryptedFile.size());

    return Napi::Boolean::New(env, success);
}

Napi::Value SupportPackage::ResetFileData(const CallbackInfo &info)
{
    Napi::Env env = info.Env();

    this->encryptedFile.clear();
    this->crc = 0;
    this->dataType = 4;

    return Napi::Boolean::New(env, true);
}
///////////////////////////////////////////////////////////////////////////
////////////  PRIVATE MEMBER FUNCTIONS NOT EXPOSED TO JS  /////////////////
///////////////////////////////////////////////////////////////////////////

bool SupportPackage::Encrypt(uint8_t *fileData, size_t fileLength, uint8_t *appKey)
{
    bool success = true;

    if (false == isEncryptionInitialized)
    {
        return false;
    }

    // TODO: ADD PARAMETER CHECKING HERE

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

    memcpy(key, appKey, BC_KEY_SIZE_BYTES);
    memcpy(IV, this->random_iv, BC_KEY_SIZE_BYTES);

    uint32_t fileDataLeft = fileLength;
    uint32_t bytesRead = 0;

    // write BC_KEY_SIZE_BYTES bytes of IV to encrypted arary
    for (int i = 0; i < BC_KEY_SIZE_BYTES; i++)
    {
        this->encryptedFile.push_back(IV[i]);
    }

    if ((cbc_start(this->cipher_idx, IV, key, BC_KEY_SIZE_BYTES, 0, &cbc)) != CRYPT_OK)
    {
        success = false;
    }
    else
    {
        do
        {
            memset(inbuf, 0, sizeof(inbuf));
            if (fileDataLeft > 512)
            {
                // Can only do 512 bytes at a time
                y = 512;
            }
            else
            {
                y = fileDataLeft;
            }

            memcpy(inbuf, fileData + bytesRead, y);

            // We just read in
            fileDataLeft -= y;

            if (y % BC_KEY_SIZE_BYTES != 0)
            {
                f = (double)y;

                y = ceil(f / BC_KEY_SIZE_BYTES) * BC_KEY_SIZE_BYTES;
            }

            if ((cbc_encrypt(inbuf, ciphertext, y, &cbc)) != CRYPT_OK)
            {
                success = false;
            }

            // Copy the newly encrypted data
            for (int i = 0; i < y; i++)
            {
                this->encryptedFile.push_back(ciphertext[i]);
            }
            printf("Size of EncryptedFile: %10d \n", this->encryptedFile.size());

        } while (success && (y == sizeof(inbuf)));
    }

    return success;
}

uint16_t SupportPackage::Calc_CRC(uint8_t *buf, size_t length)
{
    uint16_t calculatedCrc = 0;

    calculatedCrc = calcCrc(calculatedCrc, buf, length);

    unsigned char zeroes[2] = {0, 0};
    calculatedCrc = calcCrc(calculatedCrc, zeroes, 2);

    return calculatedCrc;
}

uint16_t SupportPackage::calcCrc(uint16_t sum, uint8_t *p, uint32_t len)
{
    int32_t i;
    uint8_t byte;

    while (len--)
    {
        byte = *(p++);
        for (i = 0; i < 8; ++i)
        {
            unsigned long osum = sum;
            sum <<= 1;
            if (byte & 0x80)
            {
                sum |= 1;
            }
            if (osum & 0x8000)
            {
                sum ^= 0x1021;
            }
            byte <<= 1;
        }
    }
    return sum;
}

NODE_API_MODULE(addon, Init)