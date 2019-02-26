#include <napi.h>

namespace cm256
{
  struct RecoveryState
  {
    uint32_t BlockBytes;

    uint8_t OriginalTotalCount;
    uint8_t RecoveryTotalCount;

    unsigned char *OriginalData;

    uint8_t *BlockIndices; // has to add up to OriginalTotalCount
  };

  bool CalculateRecoveryBlocks(unsigned char *OriginalFileBytes, unsigned char *outBuf, uint32_t BlockBytes, uint8_t OriginalCount, uint8_t RecoveryCount);
  bool RecoverData(RecoveryState info);

  Napi::Boolean RecoverDataWrapped(const Napi::CallbackInfo &info);
  Napi::Boolean CalculateRecoveryBlocksWrapped(const Napi::CallbackInfo &info);
  Napi::Object Init(Napi::Env env, Napi::Object exports);
} // namespace cm256