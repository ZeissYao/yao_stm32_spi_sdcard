#include "stm32f10x.h"

vu8 TxBuffer1[255];  //串口1发送缓冲区
vu8 RxBuffer1[255];  //串口1接收缓冲区
vu8 TxCounter1 = 0;  //串口1发送计数器
vu8 RxCounter1 = 0;  //串口1接收计数器

void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure;     //定义USART结构变量
    GPIO_InitTypeDef GPIO_InitStructure;		//定义GPIO结构变量
    NVIC_InitTypeDef NVIC_InitStructure;		//定义向量中断结构变量

    //使能A口和复用AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    //使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //USART1的Tx口初始化:推挽输出,50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1的Rx口初始化:浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1参数配置
    USART_InitStructure.USART_BaudRate = 9600;                   //波特率115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //数据位为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;       //停止位为1位
    USART_InitStructure.USART_Parity = USART_Parity_No;          //无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      //发送与接收
    USART_Init(USART1, &USART_InitStructure);
    //USART1使能
    USART_Cmd(USART1, ENABLE);

    //USART1中断优先级初始化与中断使能
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;          //使用USART1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //使能
    NVIC_Init(&NVIC_InitStructure);

    //使能串口1的发送和接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

}

void USART1_IRQHandler(void)
{
    static u8 tmp = 0;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {	//接收中断处理
        if (RxCounter1 < sizeof(RxBuffer1) - 1)
        { //未超出数组RxBuffer1最大容量则将接收到的数据保存到数组RxBuffer1
            RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
        }
    }

    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    { //发送中断处理
        if ((TxCounter1 > 0) && (tmp < TxCounter1))
        { //待发送数未完则送出
            USART_SendData(USART1, TxBuffer1[tmp++]);
        }
        else
        { //已发送完毕则关闭发送
            TxCounter1 = 0;
            tmp = 0;
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}
