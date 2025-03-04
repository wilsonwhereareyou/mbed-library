#include "newsupport.h"
#include "defines.h"

NewSupport::NewSupport(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();
    memset(&jpakeInfo.jpakeRequestData, 0, sizeof(JpakeHeaderType));
    memset(jpakeInfo.appKey, 0, sizeof(jpakeInfo.appKey));
    memset(jpakeInfo.updateId, 0, sizeof(jpakeInfo.updateId));

    // Initialize Auth Context
    mbedtls_ecjpake_init(&jpakeInfo.jpakeContext);
    mbedtls_ctr_drbg_init(&jpakeInfo.ctrContext);
    mbedtls_entropy_init(&jpakeInfo.entropyContext);

    unsigned char seed = 4;
    mbedtls_ctr_drbg_seed(&jpakeInfo.ctrContext, mbedtls_entropy_func, &jpakeInfo.entropyContext, &seed, MBEDTLS_CTR_DRBG_ENTROPY_LEN);
}

// Stores the Update ID and Initializes the JPake Module
Napi::Value NewSupport::CreateNumpadCommand(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // This function takes 1 param
    if (info.Length() != 2)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Expected an Array for Pairing Code").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Expected an Array for Output Buffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::ArrayBuffer pairingCode = info[0].As<Napi::ArrayBuffer>();
    Napi::ArrayBuffer outbuf = info[1].As<Napi::ArrayBuffer>();

    // Copy update ID into the jpakeInfo buffer
    memcpy(jpakeInfo.updateId, (uint8_t *)pairingCode.Data(), sizeof(jpakeInfo.updateId));

    int32_t mbedErr = -1;

    mbedErr = mbedtls_ecjpake_setup(&jpakeInfo.jpakeContext, MBEDTLS_ECJPAKE_SERVER, MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_SECP256R1, jpakeInfo.updateId, 4);

    if (mbedErr != 0)
    {
        Napi::TypeError::New(env, "mbedtls_ecjpake_setup failed").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Create the numpad command message
    uint8_t messageSize = USB_MESSAGE_HEADER_SIZE + SHOW_NUMPAD_PAYLOAD_SIZE + USB_TIMESTAMP_SIZE + USB_CHECKSUM_SIZE;
    uint8_t numpadMessage[messageSize];
    uint8_t payload[] = {2};

    constructUSBMessage(SHOW_NUMPAD_CMD, payload, SHOW_NUMPAD_PAYLOAD_SIZE, numpadMessage);

    memcpy(outbuf.Data(), numpadMessage, messageSize);

    return Napi::Number::New(env, mbedErr);
}

Napi::Value NewSupport::HandleJpakeMessage(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    int32_t status = -1;

    // This function takes 4 param
    if (info.Length() != 4)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number for USB Message ID").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Expected an Array Buffer for Payload").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Expected a number for message length").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[3].IsArrayBuffer())
    {
        Napi::TypeError::New(env, "Expected an Array Buffer for Output Buffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Parse the Jpake Response message
    int32_t usbMessageID = info[0].As<Napi::Number>();
    Napi::ArrayBuffer responseMessageJS = info[1].As<Napi::ArrayBuffer>();
    Napi::ArrayBuffer outputBuffer = info[3].As<Napi::ArrayBuffer>();
    int32_t responseMessageLength = info[2].As<Napi::Number>();

    uint8_t response[responseMessageLength];
    memcpy(response, responseMessageJS.Data(), responseMessageLength);
    
    JpakeHeaderType jpakeHeader;
    memcpy(&jpakeHeader, response, sizeof(JpakeHeaderType));

    switch (jpakeHeader.phase)
    {
        case JPAKE_PHASE0:
        {
            uint32_t minValidLen = LEN_JPAKE_PHASE_BYTES * 2 - 2;

            status = mbedtls_ecjpake_write_round_one(&jpakeInfo.jpakeContext, );
            break;
        }
        case JPAKE_PHASE1:
        {
            break;
        }
        case JPAKE_PHASE2:
        {
            break;
        }
    }

}

//////////////////////////////////////////////////////////////////////////////////
////////////  PRIVATE MEMBER FUNCTIONS NOT EXPOSED TO JS  ////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void NewSupport::constructUSBMessage(uint8_t messageID, uint8_t *payload, size_t payloadSize, uint8_t *outbuffer)
{
    // Create a USB message with the given message ID and Payload
    // Store the message in outbuffer
    uint8_t messageSize = USB_MESSAGE_HEADER_SIZE + SHOW_NUMPAD_PAYLOAD_SIZE + USB_TIMESTAMP_SIZE + USB_CHECKSUM_SIZE;

    uint8_t usbMessage[messageSize];
    uint8_t offset = 0;

    // Set header
    usbMessage[offset++] = USB_PACKET_DELIMITER;
    usbMessage[offset++] = messageID;
    usbMessage[offset++] = payloadSize;

    // Set Payload
    for (int i = 0; i < payloadSize; i++)
    {
        usbMessage[offset++] = payload[i];
    }

    // Set timestamp
    usbMessage[4] = (0);
    usbMessage[5] = (0);
    usbMessage[6] = (0);
    usbMessage[7] = (0);

    // Add checksum
    uint16_t checksum = 0;
    checksum = calcChecksum(usbMessage, USB_MESSAGE_HEADER_SIZE, payloadSize);
    usbMessage[8] = (checksum >> 8);
    usbMessage[9] = (checksum & 0xFF);

    memcpy(outbuffer, usbMessage, messageSize);
}

uint16_t NewSupport::calcChecksum(uint8_t *buffer, size_t start, size_t numItems)
{
    uint16_t checksum = 0;
    
    for (int i = 0; i < numItems; i++)
    {
        checksum += buffer[start + i];
    }

    checksum = checksum & 0xFFFF;
    return checksum;
}

NODE_API_MODULE(addon, Init)