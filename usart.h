#ifndef __USART_H
#define __USART_H 			   

//���±�����stm32f10x_it.c�ж���
extern vu8 TxBuffer1[];  //����1���ͻ�����(������stm32f10x_it.c�ж���)
extern vu8 RxBuffer1[];  //����1���ջ�����(������stm32f10x_it.c�ж���)
extern vu8 TxCounter1;   //����1���ͼ�����
extern vu8 RxCounter1;   //����1���ռ�����

void USART1_Init(void);

#endif
