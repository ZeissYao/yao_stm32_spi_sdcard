#include "stm32f10x.h"
#include "spi.h"

//--------------------------------------------------------------------------------------------
//SPI1�˿ڳ�ʼ��
void SPI1_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //ʹ��PAʱ��,PCʱ��,SPI1ʱ�Ӻ�APB2���ù���ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE);

    //�ر�SPI�����������:SD_CS(SD��),W25_CS(W25X16),T_CS(������),N_CS(RF24L01����ģ��)
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOC, GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12);

    // �ر�SPI1 CS��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    //ʹ��SPI1����(SCK��MOSI)�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //ʹ��SPI1����(MISO)��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//--------------------------------------------------------------------------------------------
//SPI1��ʼ��
//speed:��Ƶϵ��2,4,8,16,32,64,128,256�е�һ��
//�ɹ�����1,ʧ�ܷ���0
u8 SPI1_Init(u16 speed)
{
    u16 tmp;
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1����Ϊ����ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                     //����SPI1Ϊ��ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		             //����ʱ���ڲ�����ʱ��ʱ��Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		                 //�ڶ���ʱ���ؿ�ʼ��������
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			                 //NSS�ź��������ʹ��SSIλ������
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				 //���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;						     //CRCֵ����Ķ���ʽ

    //���ò�����Ԥ��Ƶ��ֵ
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
    SPI_Cmd(SPI1, ENABLE); 											 //ʹ��SPI1����
    return 1;
}
//--------------------------------------------------------------------------------------------
//��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 tmp = 0;

    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
    {	//�ȴ����ͽ���
        tmp++;
        if (tmp > 200)
            return 0;
    }
    SPI_I2S_SendData(SPI1, TxData);

    tmp = 0;
    while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
    {	//�ȴ��������
        tmp++;
        if (tmp > 200)
            return 0;
    }
    return SPI_I2S_ReceiveData(SPI1);
}

