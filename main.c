/**
 ******************************************************************************
 * @file    USART/Printf/main.c
 * @author  MCD Application Team
 * @version V3.3.0
 * @date    04/16/2010
 * @brief   Main program body
 ******************************************************************************
 * @copy
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"

#include <stdio.h>
#include <stdlib.h>

#include "sd.h"
#include "usart.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
u8 SD_Buffer[512]; //SD卡数据缓存区
int main(void)
{
    u8 tmp;
    u16 mm, nn;
    u32 nummber, nummber_bak;

    //设置优先级分组:抢占优先级和响应优先级各2位
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //设置向量表的位置和偏移:在FLASH中偏移为0
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);

    //USART1初始化
    USART1_Init();

    //检测当前系统时钟SystemCoreClock
    SystemCoreClockUpdate();

    printf("\r\n准备读卡");

    //检测SD卡,失败则2秒后继续检测
    while (SD_Init() != 0)
    {
        printf("\r\n未检测到SD卡！");
        delay_ms(SystemCoreClock, 1000);
        delay_ms(SystemCoreClock, 1000);
        printf("\r\n读卡失败！！！！");
    }

    delay_ms(SystemCoreClock, 1000);

    //逻辑0扇区的物理扇区号
    nummber_bak = SD_GetLogic0();

    printf("\r\nbank number:%d hex:%x", nummber_bak, nummber_bak);
    nummber_bak = 0;

    printf("\r\n初始化SD卡成功！\r\n");

    while (1)
    {
        RxCounter1 = 0;
        printf("\r\n请发送要读取的逻辑扇区号\r\n");

        do
        {  //等待接收停止100mS
            tmp = RxCounter1;
            delay_ms(SystemCoreClock, 100);
        } while (RxCounter1 == 0 || tmp != RxCounter1);

        //将接收到的字符串转换成数值
        RxBuffer1[RxCounter1] = '\0';
        nummber = atol((const char *) RxBuffer1);

        if (SD_ReadBlock(SD_Buffer, nummber_bak + nummber, 512) == 0)  //读指定扇区

            printf("\r\n第%d逻辑扇区数据:", nummber);

        for (mm = 0; mm < 32; mm++)
        {
            printf("\r\n%03xH  ", mm << 4);

            for (nn = 0; nn < 16; nn++)
                printf("%c", SD_Buffer[(mm << 3) + nn]);
        }
        printf("\r\n");
    }

//  while (1)
//  {
//  	delay_ms(SystemCoreClock,1000);
//	printf("\r\n串口1测试程序");	   
//  }
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (uint8_t) ch); /*发送一个字符函数*/

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)/*等待发送完成*/
    {

    }
    return ch;
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
