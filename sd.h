#ifndef __SD_H
#define __SD_H

#include "stm32f10x.h"

// 卡类型
#define MMC    0
#define V1     1
#define V2     2
#define V2HC   4

#define SD_CS_ENABLE()	GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SD_CS_DISABLE()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)

extern u8 SD_Type; //SD卡的类型
extern u16 DeviceCode;	//LCD序列号

u32 SD_GetRoot(u32 nummber);
u32 SD_GetLogic0(void);
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc, u8 reset);
u8 SD_Init(void);
u32 SD_GetCapacity(void);
u8 SD_ReadBlock(u8 *buffer, u32 sector, u16 len);

#endif

