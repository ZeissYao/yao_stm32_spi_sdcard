#ifndef __USART_H
#define __USART_H 			   

//以下变量在stm32f10x_it.c中定义
extern vu8 TxBuffer1[];  //串口1发送缓冲区(长度在stm32f10x_it.c中定义)
extern vu8 RxBuffer1[];  //串口1接收缓冲区(长度在stm32f10x_it.c中定义)
extern vu8 TxCounter1;   //串口1发送计数器
extern vu8 RxCounter1;   //串口1接收计数器

void USART1_Init(void);

#endif
