#include "stm32f10x.h"
#include "spi.h"

//--------------------------------------------------------------------------------------------
//SPI1端口初始化
void SPI1_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //使能PA时钟,PC时钟,SPI1时钟和APB2复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);

    //关闭SPI所有相关器件:SD_CS(SD卡),W25_CS(W25X16),T_CS(触摸屏),N_CS(RF24L01无线模块)
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOC, GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);

    // 关闭SPI1 CS脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    //使能SPI1引脚(SCK和MOSI)推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //使能SPI1引脚(MISO)上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//--------------------------------------------------------------------------------------------
//SPI1初始化
//speed:分频系数2,4,8,16,32,64,128,256中的一个
//成功返回1,失败返回0
u8 SPI1_Init(u16 speed)
{
    u16 tmp;
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1设置为两线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                     //设置SPI1为主模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		             //串行时钟在不操作时，时钟为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		                 //第二个时钟沿开始采样数据
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			                 //NSS信号由软件（使用SSI位）管理
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				 //数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;						     //CRC值计算的多项式

    //设置波特率预分频的值
    switch (speed)
    {
        case 2:
            tmp = SPI_BaudRatePrescaler_2;
            break;
        case 4:
            tmp = SPI_BaudRatePrescaler_4;
            break;
        case 8:
            tmp = SPI_BaudRatePrescaler_8;
            break;
        case 16:
            tmp = SPI_BaudRatePrescaler_16;
            break;
        case 32:
            tmp = SPI_BaudRatePrescaler_32;
            break;
        case 64:
            tmp = SPI_BaudRatePrescaler_64;
            break;
        case 128:
            tmp = SPI_BaudRatePrescaler_128;
            break;
        case 256:
            tmp = SPI_BaudRatePrescaler_256;
            break;
        default:
            return 0;
    }
    SPI_InitStructure.SPI_BaudRatePrescaler = tmp;

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE); 											 //使能SPI1外设
    return 1;
}
//--------------------------------------------------------------------------------------------
//读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 tmp = 0;

    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
    {	//等待发送结束
        tmp++;
        if (tmp > 200)
            return 0;
    }
    SPI_I2S_SendData(SPI1, TxData);

    tmp = 0;
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
    {	//等待接收完成
        tmp++;
        if (tmp > 200)
            return 0;
    }
    return SPI_I2S_ReceiveData(SPI1);
}

