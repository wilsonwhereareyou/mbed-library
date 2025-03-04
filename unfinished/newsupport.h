#include "napi.h"
#include "ecjpake.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "md.h"
#include "ecp.h"
#include "aes.h"
#include <vector>
#include "newJpake.h"


struct JpakeInfoObj
{
    JpakeHeaderType jpakeRequestData;
    mbedtls_ecjpake_context jpakeContext;
    mbedtls_ctr_drbg_context ctrContext;
    mbedtls_entropy_context entropyContext;
    uint8_t appKey[32];
    uint8_t updateId[4];
};

class NewSupport : public Napi::ObjectWrap<NewSupport>
{
public:
    NewSupport(const Napi::CallbackInfo &);
    Napi::Value Setup(const Napi::CallbackInfo &);
    Napi::Value CreateNumpadCommand(const Napi::CallbackInfo &);
    Napi::Value HandleJpakeMessage(const Napi::CallbackInfo &);

    static Napi::Function GetClass(Napi::Env);

private:
    uint16_t calcChecksum(uint8_t *, size_t, size_t);
    void constructUSBMessage(uint8_t, uint8_t *, size_t, uint8_t *);
    uint8_t appKey[32];
};

// JavaScript calls this function to get a JS class holding our
// native class. Must register member functions here
Napi::Function NewSupport::GetClass(Napi::Env env)
{
    return DefineClass(
        env,
        "NewSupport",
        {NewSupport::InstanceMethod("CreateNumpadCommand", &NewSupport::CreateNumpadCommand)});
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "NewSupport");
    exports.Set(name, NewSupport::GetClass(env));
    return exports;
}