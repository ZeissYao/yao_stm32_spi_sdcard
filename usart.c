#include "stm32f10x.h"

vu8 TxBuffer1[255];  //����1���ͻ�����
vu8 RxBuffer1[255];  //����1���ջ�����
vu8 TxCounter1 = 0;  //����1���ͼ�����
vu8 RxCounter1 = 0;  //����1���ռ�����

void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure;     //����USART�ṹ����
    GPIO_InitTypeDef GPIO_InitStructure;		//����GPIO�ṹ����
    NVIC_InitTypeDef NVIC_InitStructure;		//���������жϽṹ����

    //ʹ��A�ں͸���AFIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    //ʹ��USART1ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //USART1��Tx�ڳ�ʼ��:�������,50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1��Rx�ڳ�ʼ��:��������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1��������
    USART_InitStructure.USART_BaudRate = 9600;                   //������115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //����λΪ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;       //ֹͣλΪ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;          //����żУ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      //���������
    USART_Init(USART1, &USART_InitStructure);
    //USART1ʹ��
    USART_Cmd(USART1, ENABLE);

    //USART1�ж����ȼ���ʼ�����ж�ʹ��
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          //ʹ��USART1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //��Ӧ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //ʹ��
    NVIC_Init(&NVIC_InitStructure);

    //ʹ�ܴ���1�ķ��ͺͽ����ж�
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

}

void USART1_IRQHandler(void)
{
    static u8 tmp = 0;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {	//�����жϴ���
        if (RxCounter1 < sizeof(RxBuffer1) - 1)
        { //δ��������RxBuffer1��������򽫽��յ������ݱ��浽����RxBuffer1
            RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
        }
    }

    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    { //�����жϴ���
        if ((TxCounter1 > 0) && (tmp < TxCounter1))
        { //��������δ�����ͳ�
            USART_SendData(USART1, TxBuffer1[tmp++]);
        }
        else
        { //�ѷ��������رշ���
            TxCounter1 = 0;
            tmp = 0;
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}
