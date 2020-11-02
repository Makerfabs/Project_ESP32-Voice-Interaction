#include <Arduino.h>

// 16bit, stereo, 16000Hz,  linear PCM
void CreateWavHeader(byte* header, int waveDataSize);  // size of header is 44
