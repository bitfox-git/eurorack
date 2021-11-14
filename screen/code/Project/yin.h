#ifndef Yin_h
#define Yin_h

#include <stdint.h>
#include <stdbool.h>

#define YIN_SAMPLING_RATE 25000
#define YIN_DEFAULT_THRESHOLD 0.15
#define MAXIMUM_SAMPLESIZE 1024 
/**
 * @struct  Yin
 * @breif	Object to encapsulate the parameters for the Yin pitch detection algorithm 
 */
typedef struct _Yin {
	int16_t bufferSize;			/**< Size of the audio buffer to be analysed */
	int16_t halfBufferSize;		/**< Half the buffer length */
	float yinBuffer[MAXIMUM_SAMPLESIZE/2];		/**< Buffer that stores the results of the intermediate processing steps of the algorithm */
	float probability;		/**< Probability that the pitch found is correct as a decimal (i.e 0.85 is 85%) */
	float threshold;		/**< Allowed uncertainty in the result as a decimal (i.e 0.15 is 15%) */
} Yin;
 
/**
  * @return        Fundamental frequency of the signal in Hz. Returns -1 if pitch can't be found
  */
float Yin_getPitch(float* buffer, uint16_t buffersize, float threshold, bool fft);

/**
 * @return     Returns the certainty of the note found as a decimal (i.e 0.3 is 30%)
 */
float Yin_getProbability();

#endif
