#include "newJpake.h"

JPake::JPake()
{
    // Initialize Context
    memset(&jpakeContext, 0, sizeof(jpakeContext));
    memset(&ctrContext, 0, sizeof(ctrContext));
    memset(&entropyContext, 0, sizeof(entropyContext));

    jpakeInfo.phase = JPAKE_PHASE0;
    jpakeInfo.step = JPAKE_REQUEST_DATA;
    jpakeInfo.round = JPAKE_ROUND1;

    seed = 0xFF;
}

int32_t JPake::Init(uint8_t *updateID)
{
    int32_t status = -1;

    memset(this->updateId, 0, sizeof(updateId));
    memcpy(this->updateId, updateID, sizeof(updateId));

    mbedtls_ecjpake_init(&jpakeContext);
    mbedtls_ctr_drbg_init(&ctrContext);
    mbedtls_entropy_init(&entropyContext);

    jpakeInfo.phase = JPAKE_PHASE0;
    jpakeInfo.step = JPAKE_REQUEST_DATA;
    jpakeInfo.round = JPAKE_ROUND1;

    mbedtls_ctr_drbg_seed(&ctrContext, mbedtls_entropy_func, &entropyContext, &seed, MBEDTLS_CTR_DRBG_ENTROPY_LEN);

    mbedtls_ecjpake_setup(&jpakeContext, MBEDTLS_ECJPAKE_SERVER, MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_BP256R1, this->updateId, 4);
}
int32_t JPake::BuildRoundMessage(uint8_t *buf, size_t bufSize)
{
    int32_t status = -1;
    size_t msgLen;
    bool validLen = true;

    // TODO: CHECK IF BUFSIZE NEEDS TO BE 330 BYTES

    switch (jpakeInfo.phase)
    {
    case JPAKE_PHASE0:
    {
        uint32_t minValidLen = (LEN_JPAKE_PHASE_BYTES * 2) - 2;

        status = mbedtls_ecjpake_write_round_one(&jpakeContext, (unsigned char *)buf, bufSize, &msgLen, mbedtls_ctr_drbg_random, &ctrContext);

        // If the round message was built successfully but one or both of the zkp's are 1 byte less than expected, pad the message to make it the expected size (please see Jira ticket OXEN-3351 for more information)
        if ((((LEN_JPAKE_PHASE_BYTES * 2) > msgLen) && (minValidLen <= msgLen)) && (0 == status))
        {
            status = PadPhaseData(buf, MAX_JPAKE_DATA_BYTES, JPAKE_PHASE0);
        }
        else if (((LEN_JPAKE_PHASE_BYTES * 2) < msgLen) || (minValidLen > msgLen))
        {
            validLen = false;
        }
        break;
    }
    case JPAKE_PHASE1:
    {
        status = PadPhaseData(buf, LEN_JPAKE_PHASE_BYTES, JPAKE_PHASE1);
        break;
    }
    case JPAKE_PHASE2:
    {
        status = mbedtls_ecjpake_write_round_two(&jpakeContext, (unsigned char *)buf, bufSize, &msgLen, mbedtls_ctr_drbg_random, &ctrContext);

        uint32_t minValidLen = LEN_JPAKE_PHASE_BYTES + LEN_CURVE_INFO_BYTES - 1;
        if ((minValidLen == msgLen) && (0 == status))
        {
            status = PadPhaseData(buf, LEN_JPAKE_PHASE_BYTES + LEN_CURVE_INFO_BYTES, JPAKE_PHASE2);
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

    return status;
}

int32_t JPake::ValidateRoundMessage(uint8_t *buf, size_t bufSize)
{
    int32_t status = 0;

    switch (jpakeInfo.phase)
    {
    case JPAKE_PHASE0:
    {
        status = 0;
        break;
    }
    case JPAKE_PHASE1:
    {
        status = mbedtls_ecjpake_read_round_one(&jpakeContext, (unsigned char *)buf, 2 * LEN_JPAKE_PHASE_BYTES);
        break;
    }
    case JPAKE_PHASE2:
    {
        status = mbedtls_ecjpake_read_round_two(&jpakeContext, (unsigned char *)buf, LEN_JPAKE_PHASE_BYTES);
        break;
    }

    default:
        status = 2;
    }

    return status;
}

int32_t JPake::DeriveKeyMaterial(uint8_t *keyBuf, size_t keyBufSize, size_t keyLen)
{
    int32_t status = -1;

    status = mbedtls_ecjpake_derive_secret(&jpakeContext, (unsigned char *)keyBuf, keyBufSize, &keyLen, mbedtls_ctr_drbg_random, &ctrContext);

    return status;
}

int32_t JPake::GenerateEncryptedChallenge(uint8_t *key, uint8_t *text, size_t textLen, uint8_t *encryptedText)
{
    int32_t status = -1;

    mbedtls_aes_context aesCtx;
    mbedtls_aes_init(&aesCtx);

    if ((AES128_BLOCK_LEN == textLen) && (0 == mbedtls_aes_setkey_enc(&aesCtx, (unsigned char *)key, AES128_BLOCK_LEN * BITS_IN_BYTE)))
    {
        status = mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_ENCRYPT, (unsigned char *)text, (unsigned char *)encryptedText);
    }

    return status;
}

int32_t JPake::DecryptChallenge(uint8_t *key, uint8_t *text, size_t textLen, uint8_t *encryptedText)
{
    int32_t status = -1;

    mbedtls_aes_context aesCtx;
    mbedtls_aes_init(&aesCtx);

    if ((AES128_BLOCK_LEN == textLen) && (0 == mbedtls_aes_setkey_dec(&aesCtx, (unsigned char *)key, AES128_BLOCK_LEN * BITS_IN_BYTE)))
    {
        status = mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_DECRYPT, (unsigned char *)text, (unsigned char *)encryptedText);
    }

    return status;
}

int32_t JPake::PadPhaseData(uint8_t *buf, size_t bufSize, JpakePhaseType phase)
{
    int32_t status = 0;

    JpakeRoundData unpaddedMessage;
    JpakeRoundData *paddedMessage;

    uint8_t *messageStart;
    uint8_t zkpShift = 0;

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
        status = 1;
    }

    return status;
}
