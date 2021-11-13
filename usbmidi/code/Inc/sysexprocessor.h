#ifndef SYSEXPROCESSOR_H_
#define SYSEXPROCESSOR_H_

#include <stdint.h>
#include <stdbool.h>

void Sysex_Process(uint8_t input);

// void Sysex_ConfigurePush();
// void Sysex_CalibratePush();
bool Sysex_DumpConfiguration();
bool Sysex_DumpCalibration();

void Sysex_CalibrationLow();
void Sysex_CalibrationHigh();

void Sysex_SendMidiMessage(uint8_t usbmessagetype, uint8_t byte1, uint8_t byte2, uint8_t byte3);

#endif // SYSEXPROCESSOR_H_