// // #include "arm_math.h"
// // #include "arm_common_tables.h"

// // arm_rfft_fast_instance_f32 fft;


// #include "kiss_fftr.h"
// #include "kiss_fft.h"
// #include "twiddles.h"

// kiss_fftr_state rfft;
// kiss_fftr_state rffti;

// kiss_fft_state cfft;
// kiss_fft_state cffti;

// // kiss_fft_cpx tempbuffer[1025];
// /*
// struct kiss_fftr_state{
//     kiss_fft_cfg substate;
//     kiss_fft_cpx * tmpbuf;
//     kiss_fft_cpx * super_twiddles;
// #ifdef USE_SIMD
//     void * pad;
// #endif
// };
// */

// void FFT_Init() {
  
//   // fft = kiss_fftr_alloc(256,0,NULL,NULL);
//   // ffti = kiss_fftr_alloc(256,1,NULL,NULL);
//   // cfft.inverse = 0;
//   cfft.nfft = 512;
//   // cfft.twiddles = c_twiddles512;
//   kf_factor(512,cfft.factors);

//   rfft.substate = &cfft;
//   // rfft.super_twiddles = r_twiddles512;
//   // rfft.tmpbuf = tempbuffer;


//   // fft.substate = 

// }

// void RFFT_Convert(kiss_fft_scalar* src, kiss_fft_cpx* dst) {
//   cfft.twiddles = c_twiddles512;
//   cfft.inverse = 0;
//   rfft.super_twiddles = r_twiddles512;
//   kiss_fftr(&rfft,src,dst);
// }

// void RIFFT_Convert(kiss_fft_cpx* src, kiss_fft_scalar* dst) {
//   cfft.twiddles = ci_twiddles512;
//   cfft.inverse = 1;
//   rfft.super_twiddles = ri_twiddles512;
//   kiss_fftri(&rfft,src,dst);
// }
