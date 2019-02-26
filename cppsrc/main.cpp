/* cppsrc/main.cpp */
#include "cm256.h"
#include "main.h"

bool cm256::CalculateRecoveryBlocks(unsigned char *OriginalBytes, unsigned char *RecoveryBytes, uint32_t BlockBytes, uint8_t OriginalCount, uint8_t RecoveryCount)
{
  cm256_encoder_params params;
  params.BlockBytes = BlockBytes;
  params.OriginalCount = OriginalCount;
  params.RecoveryCount = RecoveryCount;

  cm256_block blocks[256];
  for (int i = 0; i < OriginalCount; ++i)
  {
    blocks[i].Block = OriginalBytes + i * BlockBytes;
  }

  if (cm256_encode(params, blocks, RecoveryBytes))
  {
    return false;
  }

  return true;
}

bool cm256::RecoverData(RecoveryState data)
{
  cm256_encoder_params params;
  params.BlockBytes = data.BlockBytes;
  params.OriginalCount = data.OriginalTotalCount;
  params.RecoveryCount = data.RecoveryTotalCount;

  const uint8_t maxBlocks = data.OriginalTotalCount + data.RecoveryTotalCount;

  cm256_block blocks[256];
  for (int i = 0; i < params.OriginalCount; ++i)
  {
    unsigned char blockIndex = static_cast<unsigned char>(data.BlockIndices[i]);

    if(blockIndex >= maxBlocks) {
      return false; // one of the indices is out of bounds, gtfo
    }
    
    blocks[i].Block = data.OriginalData + i * params.BlockBytes;
    blocks[i].Index = blockIndex;
  }

  if (cm256_decode(params, blocks))
  {
      return false;
  }

  return true;
}

Napi::Boolean cm256::CalculateRecoveryBlocksWrapped(const Napi::CallbackInfo &info)
{
  const Napi::Env env = info.Env();

  Napi::Object params = info[0].As<Napi::Object>();
  if(!params.Has("originalBytes")) {
    Napi::Error::New(env, "missing param: originalBytes").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("recoveryBytes")) {
    Napi::Error::New(env, "missing param: recoveryBytes").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("blockBytes")) {
    Napi::Error::New(env, "missing param: blockBytes").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("originalCount")) {
    Napi::Error::New(env, "missing param: originalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("recoveryCount")) {
    Napi::Error::New(env, "missing param: recoveryCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  const Napi::Buffer<unsigned char> originalBytes = Napi::Buffer<unsigned char>(env, params.Get("originalBytes"));
  const Napi::Buffer<unsigned char> recoveryBytes = Napi::Buffer<unsigned char>(env, params.Get("recoveryBytes"));
  const Napi::Number blockBytesParam = Napi::Number(env, params.Get("blockBytes"));
  const Napi::Number originalCountParam = Napi::Number(env, params.Get("originalCount"));
  const Napi::Number recoveryCountParam = Napi::Number(env, params.Get("recoveryCount"));

  const uint32_t blockBytes = blockBytesParam.Uint32Value();

  if (originalCountParam.Uint32Value() > 0xFF)
  {
    Napi::Error::New(env, "originalCount must be < 255").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  const uint8_t originalCount = (uint8_t)originalCountParam.Uint32Value();

  if ((uint8_t)recoveryCountParam.Uint32Value() > 0xFF - originalCount)
  {
    Napi::Error::New(env, "recoveryCount must be < 255-originalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  const uint8_t recoveryCount = (uint8_t)recoveryCountParam.Uint32Value();

  if (originalBytes.ByteLength() != blockBytes * originalCount)
  {
    Napi::Error::New(env, "originalBytes not same size as blockBytes*originalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (recoveryBytes.ByteLength() != blockBytes * recoveryCount)
  {
    Napi::Error::New(env, "recoveryBytes not same size as blockBytes*recoveryCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if (!cm256::CalculateRecoveryBlocks(originalBytes.Data(), recoveryBytes.Data(), blockBytes, originalCount, recoveryCount))
  {
    Napi::Error::New(env, "failed to create blocks").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  return Napi::Boolean::New(env, true);
}

Napi::Boolean cm256::RecoverDataWrapped(const Napi::CallbackInfo &info)
{
  const Napi::Env env = info.Env();

  Napi::Object params = info[0].As<Napi::Object>();

  if(!params.Has("data")) {
    Napi::Error::New(env, "missing param: data").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("blockIndices")) {
    Napi::Error::New(env, "missing param: blockIndices").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("blockSize")) {
    Napi::Error::New(env, "missing param: blockSize").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("originalTotalCount")) {
    Napi::Error::New(env, "missing param: originalTotalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  if(!params.Has("recoveryTotalCount")) {
    Napi::Error::New(env, "missing param: recoveryTotalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  const Napi::Buffer<unsigned char> data = Napi::Buffer<unsigned char>(env, params.Get("data"));
  const Napi::Buffer<uint8_t> blockIndices = Napi::Buffer<uint8_t>(env, params.Get("blockIndices"));
  const Napi::Number blockSizeParam = Napi::Number(env, params.Get("blockSize"));
  const Napi::Number originalTotalCountParam = Napi::Number(env, params.Get("originalTotalCount"));
  const Napi::Number recoveryTotalCountParam = Napi::Number(env, params.Get("recoveryTotalCount"));

  const uint32_t blockSize = blockSizeParam.Uint32Value(); // TODO: make sure machine has enough memory

  if (originalTotalCountParam.Uint32Value() > 0xFF)
  {
    Napi::Error::New(env, "originalTotalCountParam must be <= 256").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  const uint8_t originalTotalCount = (uint8_t)originalTotalCountParam.Uint32Value();

  if ((uint8_t)recoveryTotalCountParam.Uint32Value() > 0xFF - originalTotalCount)
  {
    Napi::Error::New(env, "recoveryTotalCountParam must be <= 256-originalTotalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  const uint8_t recoveryTotalCount = (uint8_t)recoveryTotalCountParam.Uint32Value();

  if (data.ByteLength() != blockSize * originalTotalCount)
  {
    Napi::Error::New(env, "data not same size as blockBytes*originalTotalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  if(blockIndices.ByteLength() != originalTotalCount) {
    Napi::Error::New(env, "blockIndices not same size as originalTotalCount").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  RecoveryState state = {
    blockSize,
    originalTotalCount,
    recoveryTotalCount,
    data.Data(),
    blockIndices.Data(),
  };

  if (!cm256::RecoverData(state))
  {
    Napi::Error::New(env, "failed to create blocks").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  return Napi::Boolean::New(env, true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  if (cm256_init())
  {
    // Wrong static library
    Napi::Error::New(env, "Failed initializing cm256").ThrowAsJavaScriptException();
    return exports;
  }

  exports.Set("CalculateRecoveryBlocks", Napi::Function::New(env, cm256::CalculateRecoveryBlocksWrapped));
  exports.Set("RecoverData", Napi::Function::New(env, cm256::RecoverDataWrapped));
  return exports;
}

NODE_API_MODULE(cm256, Init)