#include "stm32f10x.h"
#include "gpioband.h"
#include "sd.h"
#include "spi.h"
#include "delay.h" 							   

u8 SD_Type = 0; //SD������

//---------------------------------------------------------------------------------------------------------
//��SD��Ӧ���ж�
//response:��ȷ��Ӧֵ
//�ɹ�����0,ʧ�ܷ���1
u8 SD_GetResponse(u16 response)
{
    u16 ii;

    ii = 5000; //��Ӧ�����5000��
    while ((SPI1_ReadWriteByte(0xff) != response) && (ii != 0))
        ii--;

    if (ii == 0)
        return 1; //����ʧ��
    else
        return 0; //���سɹ�
}
//---------------------------------------------------------------------------------------------------------
//д���SD��
//cmd:����
//arg:����
//crc:У����
//reset:(0/1)�˳�ʱ(����/ʧ��)SD��Ƭѡ
//����SD����Ӧ															  
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc, u8 reset)
{
    u8 tmp;
    u8 i;
    
    SD_CS_ENABLE();

    SPI1_ReadWriteByte(0xff); //����8��ʱ��ȷ���ϴβ������

    SD_CS_DISABLE();

    //����д������,����,У����
    SPI1_ReadWriteByte(cmd | 0x40);
    SPI1_ReadWriteByte(arg >> 24);
    SPI1_ReadWriteByte(arg >> 16);
    SPI1_ReadWriteByte(arg >> 8);
    SPI1_ReadWriteByte(arg);
    SPI1_ReadWriteByte(crc);

    i = 0; //�ɹ���ʱ�˳�
    do
    {
        tmp = SPI1_ReadWriteByte(0xff);
        i++;
        if (i > 200)
            break;
    } while (tmp == 0xff);

    if (reset)	//����Ҫ�󱣳ֻ�ʧ��SD��
    {
        SD_CS_ENABLE();
        SPI1_ReadWriteByte(0xff); //����8��ʱ��ȷ�����β������
    }

    return tmp;
}
//---------------------------------------------------------------------------------------------------------
//��ָ����������ʼ����len(<=512)�ֽ�
//buffer:���ݴ洢����ַ
//sector:������ַ(����������
//len:�ֽ���
//����ֵ0:�ɹ�,���ط�0:ʧ��
u8 SD_ReadBlock(u8 *buffer, u32 sector, u16 len)
{
    u8 tmp;
    u16 i;

    //��SDHC��,����������������ַת�����ֽڵ�ַ
    if (SD_Type != V2HC)
        sector = sector << 9;

    tmp = SD_SendCommand(17, sector, 0, 1); //������
    if (tmp != 0x00)
        return tmp;

    // ����һ�δ���
    SD_CS_DISABLE();
    if (SD_GetResponse(0xfe)) //�ȴ�SD������0xfe
    {
        SD_CS_ENABLE();  //�����˳�
        return 1;
    }

    i = 512;
    while (i > 0)
    {
        *buffer = SPI1_ReadWriteByte(0xff);
        buffer++;
        i--;
    }

    SPI1_ReadWriteByte(0xff);  //����2��CRC
    SPI1_ReadWriteByte(0xff);

    SD_CS_ENABLE();
    
    SPI1_ReadWriteByte(0xff); //����8��ʱ���Ա�֤�������

    return 0;
}
//---------------------------------------------------------------------------------------------------------
//��ȡ�߼�0����������������
u32 SD_GetLogic0(void)
{
    u32 tmp = 0;
    u8 buftmp[512];

    SD_ReadBlock(buftmp, 0, 512);
    tmp += buftmp[0x1c6];
    tmp += buftmp[0x1c6 + 1] << 8;
    tmp += buftmp[0x1c6 + 2] << 16;
    tmp += buftmp[0x1c6 + 3] << 24;

    return tmp;
}
//---------------------------------------------------------------------------------------------------------
//��ȡ��Ŀ¼������������
u32 SD_GetRoot(u32 nummber)
{
    u32 tmp = 0;
    u8 buftmp[512];

    SD_ReadBlock(buftmp, nummber, 512);

    if (buftmp[0x52] == 'F' && buftmp[0x53] == 'A' && buftmp[0x54] == 'T' && buftmp[0x55] == '3' && buftmp[0x56] == '2') //FAT32
        tmp += buftmp[14] + (buftmp[15] << 8) + buftmp[16] * (buftmp[36] + (buftmp[37] << 8));
    else if (buftmp[0x36] == 'F' && buftmp[0x37] == 'A' && buftmp[0x38] == 'T' && buftmp[0x39] == '1' && buftmp[0x3a] == '6') //FAT16
        tmp += buftmp[14] + (buftmp[15] << 8) + buftmp[16] * (buftmp[22] + (buftmp[23] << 8));

    return tmp;
}
//---------------------------------------------------------------------------------------------------------
//��ʼ��SD��
//��ʼ���ɹ�����0,ʧ�ܷ��ط�0
u8 SD_Init(void)
{
    u8 tmp;
    u16 i;
    u8 buf[6];
    
    SPI1_GPIOInit(); //SPI1��ض˿ڳ�ʼ��
    
    SD_CS_ENABLE();

    //Ϊ��ӦMMC��Ҫ��ʱ��<400KHz
    //����256��Ƶ(ʵ��281.25KHz)
    SPI1_Init(256);

    delay_ms(SystemCoreClock, 100); //��ʱ100mS�ȴ����ϵ�

    for (i = 0; i < 10; i++)  //���SD��Ҫ�������74����ʼ������
        SPI1_ReadWriteByte(0xFF);

    i = 0;
    do
    {	//����CMD0,����SPIģʽ
        tmp = SD_SendCommand(0, 0, 0x95, 1);
        i++;
    } while ((tmp != 0x01) && (i < 200));	//�ȴ���Ӧ0x01

    if (tmp == 200)
        return 1; //ʧ���˳�

    //��ȡ���İ汾��Ϣ
    SD_CS_DISABLE();
    tmp = SD_SendCommand(8, 0x1aa, 0x87, 0);

    if (tmp == 0x05)
    {	//v1.0���MMC
        SD_Type = V1;  //Ԥ��SDV1.0

        SD_CS_ENABLE();
        
        SPI1_ReadWriteByte(0xff); //����8��ʱ��ȷ�����β������

        i = 0;
        do
        {
            tmp = SD_SendCommand(55, 0, 0, 1); //����CMD55,Ӧ����0x01
            if (tmp == 0xff)
                return tmp;	 //����0xff�����޿�,�˳�

            tmp = SD_SendCommand(41, 0, 0, 1); //�ٷ���CMD41,Ӧ����0x00
            i++;
            //��Ӧ��ȷ,������Ԥ�����
        } while ((tmp != 0x00) && (i < 400));
        
        if (i == 400)
        {	//�޻�Ӧ,��MMC��
            i = 0;

            do
            {	//MMC����ʼ��
                tmp = SD_SendCommand(1, 0, 0, 1);
                i++;
            } while ((tmp != 0x00) && (i < 400));
            
            if (i == 400)
                return 1;   //MMC����ʼ��ʧ��

            SD_Type = MMC;
        }

        SPI1_Init(4); //SPIʱ�Ӹ���4��Ƶ(18MHz)

        SPI1_ReadWriteByte(0xff); //���8��ʱ��ȷ��ǰ�β�������

        //����CRCУ��
        tmp = SD_SendCommand(59, 0, 0x95, 1);
        if (tmp != 0x00)
            return tmp;  //���󷵻�
            
        //�����������
        tmp = SD_SendCommand(16, 512, 0x95, 1);
        if (tmp != 0x00)
            return tmp;  //���󷵻�
    }
    else if (tmp == 0x01)
    {	//V2.0��V2.0HC��
        //����V2.0���ĺ���4�ֽ�
        SPI1_ReadWriteByte(0xff);
        SPI1_ReadWriteByte(0xff);
        SPI1_ReadWriteByte(0xff);
        SPI1_ReadWriteByte(0xff);

        SD_CS_ENABLE();
        
        SPI1_ReadWriteByte(0xff); //����8��ʱ��ȷ�����β������

        {
            i = 0;
            do
            {
                tmp = SD_SendCommand(55, 0, 0, 1);
                if (tmp != 0x01)
                    return tmp;	 //���󷵻�

                tmp = SD_SendCommand(41, 0x40000000, 0, 1);
                if (i > 200)
                    return tmp;  //��ʱ����
            } while (tmp != 0);

            tmp = SD_SendCommand(58, 0, 0, 0);
            if (tmp != 0x00)
            {
                SD_CS_ENABLE();  //ʧ��SD
                return tmp;  //���󷵻�
            }

            //����OCR��Ϣ
            buf[0] = SPI1_ReadWriteByte(0xff);
            buf[1] = SPI1_ReadWriteByte(0xff);
            buf[2] = SPI1_ReadWriteByte(0xff);
            buf[3] = SPI1_ReadWriteByte(0xff);

            SD_CS_ENABLE();

            SPI1_ReadWriteByte(0xff); //����8��ʱ��ȷ�����β������

            if (buf[0] & 0x40)
                SD_Type = V2HC;
            else
                SD_Type = V2;

            SPI1_Init(4);
        }
    }
    
    return tmp;
}
//---------------------------------------------------------------------------------------------------------
