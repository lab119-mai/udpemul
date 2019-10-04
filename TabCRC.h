/*-------------------------------------------------------------------------*/
/* TabCRC.h                                            Created by Elephant */
/*-------------------------------------------------------------------------*/

#ifndef __CRC_H
#define __CRC_H

#define ACCUMULATOR_INIT_VALUE         0xffff

#define CalculateCRC(Buffer, BufferSize) CalculateCRCEx(ACCUMULATOR_INIT_VALUE, Buffer, BufferSize)

#ifdef __cplusplus
extern "C" {
#endif

unsigned short int CalculateCRCEx(unsigned short int Accumulator, void *Buffer, int Size);

#ifdef __cplusplus
}
#endif

#endif
