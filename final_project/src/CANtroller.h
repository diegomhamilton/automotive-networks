#ifndef USER_CAN_H
#define USER_CAN_H
#include "mbed.h"
#include "BitTiming.h"
#include "user_functions.h"

extern bool bus_idle;
extern int ReadBuffer[200];
extern bool Sending;
extern bool repeat_frame;
extern int sent_Bit;
extern bool WhenACK;
extern bool recessive_error_state;
extern bool buss_off_state;
extern bool error_state;
extern int ActiveErrorFrame[15];
extern int PassiveErrorFrame[15];

int decoder_step(struct Frame* Fr, int Bit);
void frame_builder();
int bit_destf(int Bit);

void encoder_exe();
void frame_interpreter(struct Frame Fr, int* EncFrame);
int* bit_stf(int FrameS[200]);
void error_handler_exe(int Flag);
int error_step(int Bit, int Flag);

#endif