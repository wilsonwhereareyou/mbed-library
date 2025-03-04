#pragma once

#include "napi.h"
#include "tomcrypt.h"
#include <vector>

using namespace Napi;

class SupportPackage : public ObjectWrap<SupportPackage>
{
public:
    SupportPackage(const CallbackInfo &);
    Napi::Value SetupEncryption(const CallbackInfo &);
    Napi::Value SetFileData(const CallbackInfo &);
    Napi::Value GetFileData(const CallbackInfo &);
    Napi::Value ResetFileData(const CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);

private:
    bool Encrypt(uint8_t *, size_t, uint8_t *);
    uint16_t Calc_CRC(uint8_t *, size_t);
    uint16_t calcCrc(uint16_t sum, uint8_t *p, uint32_t len);

    uint32_t cipher_idx;
    uint8_t appKey[32];
    uint8_t random_iv[16];

    // uint8_t fileBuffer[1000];
    std::vector<uint8_t> encryptedFile;
    uint16_t crc = 0;
    uint8_t dataType = 4;

    bool isEncryptionInitialized = false;

    symmetric_CBC cbc;
};

// JavaScript calls this function to get a JS class holding our
// native class. Must register member functions here
Napi::Function SupportPackage::GetClass(Napi::Env env)
{
    return DefineClass(
        env,
        "SupportPackage",
        {SupportPackage::InstanceMethod("SetupEncryption", &SupportPackage::SetupEncryption),
         SupportPackage::InstanceMethod("SetFileData", &SupportPackage::SetFileData),
         SupportPackage::InstanceMethod("GetFileData", &SupportPackage::GetFileData),
         SupportPackage::InstanceMethod("ResetFileData", &SupportPackage::ResetFileData)});
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "SupportPackage");
    exports.Set(name, SupportPackage::GetClass(env));
    return exports;
}