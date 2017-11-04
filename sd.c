#include "stm32f10x.h"
#include "gpioband.h"
#include "sd.h"
#include "spi.h"
#include "delay.h" 							   

u8 SD_Type = 0; //SD卡类型

//---------------------------------------------------------------------------------------------------------
//读SD卡应答并判断
//response:正确回应值
//成功返回0,失败返回1
u8 SD_GetResponse(u16 response)
{
    u16 ii;

    ii = 5000; //读应答最多5000次
    while ((SPI1_ReadWriteByte(0xff) != response) && (ii != 0))
        ii--;

    if (ii == 0)
        return 1; //返回失败
    else
        return 0; //返回成功
}
//---------------------------------------------------------------------------------------------------------
//写命令到SD卡
//cmd:命令
//arg:参数
//crc:校验码
//reset:(0/1)退出时(保持/失能)SD卡片选
//返回SD卡响应															  
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc, u8 reset)
{
    u8 tmp;
    u8 i;
    
    SD_CS_ENABLE();

    SPI1_ReadWriteByte(0xff); //增加8个时钟确保上次操作完成

    SD_CS_DISABLE();

    //依次写入命令,参数,校验码
    SPI1_ReadWriteByte(cmd | 0x40);
    SPI1_ReadWriteByte(arg >> 24);
    SPI1_ReadWriteByte(arg >> 16);
    SPI1_ReadWriteByte(arg >> 8);
    SPI1_ReadWriteByte(arg);
    SPI1_ReadWriteByte(crc);

    i = 0; //成功或超时退出
    do
    {
        tmp = SPI1_ReadWriteByte(0xff);
        i++;
        if (i > 200)
            break;
    } while (tmp == 0xff);

    if (reset)	//根据要求保持或失能SD卡
    {
        SD_CS_ENABLE();
        SPI1_ReadWriteByte(0xff); //增加8个时钟确保本次操作完成
    }

    return tmp;
}
//---------------------------------------------------------------------------------------------------------
//从指定扇区的起始处读len(<=512)字节
//buffer:数据存储区地址
//sector:扇区地址(物理扇区）
//len:字节数
//返回值0:成功,返回非0:失败
u8 SD_ReadBlock(u8 *buffer, u32 sector, u16 len)
{
    u8 tmp;
    u16 i;

    //除SDHC外,其他卡均需扇区地址转换成字节地址
    if (SD_Type != V2HC)
        sector = sector << 9;

    tmp = SD_SendCommand(17, sector, 0, 1); //读命令
    if (tmp != 0x00)
        return tmp;

    // 启动一次传输
    SD_CS_DISABLE();
    if (SD_GetResponse(0xfe)) //等待SD卡发回0xfe
    {
        SD_CS_ENABLE();  //错误退出
        return 1;
    }

    i = 512;
    while (i > 0)
    {
        *buffer = SPI1_ReadWriteByte(0xff);
        buffer++;
        i--;
    }

    SPI1_ReadWriteByte(0xff);  //丢弃2个CRC
    SPI1_ReadWriteByte(0xff);

    SD_CS_ENABLE();
    
    SPI1_ReadWriteByte(0xff); //再送8个时钟以保证操作完成

    return 0;
}
//---------------------------------------------------------------------------------------------------------
//获取逻辑0扇区的物理扇区号
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
//获取根目录的物理扇区号
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
//初始化SD卡
//初始化成功返回0,失败返回非0
u8 SD_Init(void)
{
    u8 tmp;
    u16 i;
    u8 buf[6];
    
    SPI1_GPIOInit(); //SPI1相关端口初始化
    
    SD_CS_ENABLE();

    //为适应MMC卡要求时钟<400KHz
    //暂用256分频(实际281.25KHz)
    SPI1_Init(256);

    delay_ms(SystemCoreClock, 100); //延时100mS等待卡上电

    for (i = 0; i < 10; i++)  //输出SD卡要求的至少74个初始化脉冲
        SPI1_ReadWriteByte(0xFF);

    i = 0;
    do
    {	//发送CMD0,进入SPI模式
        tmp = SD_SendCommand(0, 0, 0x95, 1);
        i++;
    } while ((tmp != 0x01) && (i < 200));	//等待回应0x01

    if (tmp == 200)
        return 1; //失败退出

    //获取卡的版本信息
    SD_CS_DISABLE();
    tmp = SD_SendCommand(8, 0x1aa, 0x87, 0);

    if (tmp == 0x05)
    {	//v1.0版和MMC
        SD_Type = V1;  //预设SDV1.0

        SD_CS_ENABLE();
        
        SPI1_ReadWriteByte(0xff); //增加8个时钟确保本次操作完成

        i = 0;
        do
        {
            tmp = SD_SendCommand(55, 0, 0, 1); //发送CMD55,应返回0x01
            if (tmp == 0xff)
                return tmp;	 //返回0xff表明无卡,退出

            tmp = SD_SendCommand(41, 0, 0, 1); //再发送CMD41,应返回0x00
            i++;
            //回应正确,则卡类型预设成立
        } while ((tmp != 0x00) && (i < 400));
        
        if (i == 400)
        {	//无回应,是MMC卡
            i = 0;

            do
            {	//MMC卡初始化
                tmp = SD_SendCommand(1, 0, 0, 1);
                i++;
            } while ((tmp != 0x00) && (i < 400));
            
            if (i == 400)
                return 1;   //MMC卡初始化失败

            SD_Type = MMC;
        }

        SPI1_Init(4); //SPI时钟改用4分频(18MHz)

        SPI1_ReadWriteByte(0xff); //输出8个时钟确保前次操作结束

        //禁用CRC校验
        tmp = SD_SendCommand(59, 0, 0x95, 1);
        if (tmp != 0x00)
            return tmp;  //错误返回
            
        //设置扇区宽度
        tmp = SD_SendCommand(16, 512, 0x95, 1);
        if (tmp != 0x00)
            return tmp;  //错误返回
    }
    else if (tmp == 0x01)
    {	//V2.0和V2.0HC版
        //忽略V2.0卡的后续4字节
        SPI1_ReadWriteByte(0xff);
        SPI1_ReadWriteByte(0xff);
        SPI1_ReadWriteByte(0xff);
        SPI1_ReadWriteByte(0xff);

        SD_CS_ENABLE();
        
        SPI1_ReadWriteByte(0xff); //增加8个时钟确保本次操作完成

        {
            i = 0;
            do
            {
                tmp = SD_SendCommand(55, 0, 0, 1);
                if (tmp != 0x01)
                    return tmp;	 //错误返回

                tmp = SD_SendCommand(41, 0x40000000, 0, 1);
                if (i > 200)
                    return tmp;  //超时返回
            } while (tmp != 0);

            tmp = SD_SendCommand(58, 0, 0, 0);
            if (tmp != 0x00)
            {
                SD_CS_ENABLE();  //失能SD
                return tmp;  //错误返回
            }

            //接收OCR信息
            buf[0] = SPI1_ReadWriteByte(0xff);
            buf[1] = SPI1_ReadWriteByte(0xff);
            buf[2] = SPI1_ReadWriteByte(0xff);
            buf[3] = SPI1_ReadWriteByte(0xff);

            SD_CS_ENABLE();

            SPI1_ReadWriteByte(0xff); //增加8个时钟确保本次操作完成

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
