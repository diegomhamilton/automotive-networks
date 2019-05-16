#ifndef USER_CAN_H
#define USER_CAN_H
#include "mbed.h"

void decoder_exe();
void frame_builder();
void crc_execute();
void bit_destf();

void encoder_exe();
void frame_interpreter();
void crc_calc();
void error_handler_exe();


#endif