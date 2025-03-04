#include <stdio.h>
#include <stdint.h>
#include <cstring>
#include "ecjpake.h"
#include "ctr_drbg.h"
#include "entropy.h"

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

typedef enum : uint8_t
{
    JPAKE_REQUEST_DATA,
    JPAKE_SEND_DATA,
} JpakeStepType;

typedef enum : uint8_t
{
    JPAKE_ROUND1,
    JPAKE_ROUND2,
} JpakeRoundType;

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

struct JpakeHeaderType
{
    JpakePhaseType phase;
    JpakeStepType step;
    JpakeRoundType round;
};

struct JpakeRound2ToClient
{
    uint8_t curveInfo[LEN_CURVE_INFO];
    JpakeRoundData data;
};

class JPake
{
public:
    JPake();
    int32_t Init(uint8_t *updateID);
    int32_t BuildRoundMessage(uint8_t *, size_t);
    int32_t ValidateRoundMessage(uint8_t *, size_t);
    int32_t DeriveKeyMaterial(uint8_t *, size_t, size_t);
    int32_t GenerateEncryptedChallenge(uint8_t *, uint8_t *, size_t, uint8_t *);
    int32_t DecryptChallenge(uint8_t *, uint8_t *, size_t, uint8_t *);

    JpakeHeaderType jpakeInfo;

private:
    int32_t PadPhaseData(uint8_t *, size_t, JpakePhaseType);

    // Properties
    uint8_t updateId[4];
    mbedtls_ecjpake_context jpakeContext;
    mbedtls_ctr_drbg_context ctrContext;
    mbedtls_entropy_context entropyContext;
    unsigned char seed;
};