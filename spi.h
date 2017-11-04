#ifndef __SPI_H
#define __SPI_H

void SPI1_GPIOInit(void);
u8 SPI1_Init(u16 speed);
u8 SPI1_ReadWriteByte(u8 TxData);

#endif

