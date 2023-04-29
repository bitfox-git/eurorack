#ifndef FFT_H_
#define FFT_H_


void FFT_Init();
void RFFT_Convert(float* src, float* dst);
void RIFFT_Convert(float* src, float* dst);

#endif // FFT_H_