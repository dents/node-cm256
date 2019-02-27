# node-cm256
Simple node wrapper around [cm256](https://github.com/catid/cm256)

**NB:** This project will only work on CPUs that support SSE 4.1

Example usage:

    const cm256 = require('cm256');

    const BLOCK_SIZE = 5;
    const ORIGINAL_BLOCK_COUNT = 2;
    const RECOVERY_BLOCK_COUNT = 4;

    // create buffer to keep original data
    const originalData = Buffer.alloc(BLOCK_SIZE*ORIGINAL_BLOCK_COUNT);
    originalData.fill(0xAA); // normally this data would come from a file or something

    // create buffer that will contain the recovery data calculated by cm256
    const recoveryData = Buffer.alloc(BLOCK_SIZE*RECOVERY_BLOCK_COUNT);

    // throw our data to the C code and let it do the math
    const resultCalc = cm256.CalculateRecoveryBlocks({
        originalBytes: originalData,
        recoveryBytes: recoveryData,
        blockBytes: BLOCK_SIZE,
        originalCount: ORIGINAL_BLOCK_COUNT,
        recoveryCount: RECOVERY_BLOCK_COUNT
    });
    // this will be false if cm256 is not happy. invalid paremeters will cause an exception.
    if(resultCalc !== true) {
        console.error('CalculateRecoveryBlocks FAILED');
        return;
    }

    // at this point our recoveryData buffer contains recovery blocks
    console.log(`Original data : ${originalData.toString('hex')}`);
    console.log(`Recovery data : ${recoveryData.toString('hex')}`);


    // now an example to get original data using recovery blocks:

    // corrupt first block of original data
    originalData.fill(0xBB, 0, BLOCK_SIZE);

    console.log(`Corrupted data: ${originalData.toString('hex')}`)

    // copy first block of recovery data into the place of corrupted original data
    recoveryData.copy(originalData, 0, 0, BLOCK_SIZE);

    // have cm256 do the math and recover our original data
    const resultRec = cm256.RecoverData({
        data: originalData,
        blockIndices: new Uint8Array([ORIGINAL_BLOCK_COUNT+0, 1]), // since we used first recovery block
        blockSize: BLOCK_SIZE,
        originalTotalCount: ORIGINAL_BLOCK_COUNT,
        recoveryTotalCount: RECOVERY_BLOCK_COUNT,
    });
    if(resultRec !== true) {
        console.error('RecoverData FAILED')
        return;
    }

    console.log(`Recovered data: ${originalData.toString('hex')}`)
