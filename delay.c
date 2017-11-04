#include "stm32f10x.h"

//--------------------------------------------------------------------------------------------
//ms级延时
//SYSCLK单位为Hz,ms单位为ms
//在72MHz时,1<=ms<=1864 
void delay_ms(u32 SYSCLK, u16 ms)
{
    u32 temp;

    SysTick->CTRL &= 0xfffffffb; //选择外部时钟HCLK/8
    SysTick->LOAD = (u32) (ms * (SYSCLK / 8000)); //时间加载(SysTick->LOAD为24bit)
    SysTick->VAL = 0x00;           //清空计数器
    SysTick->CTRL = 0x01;          //开始倒数

    do
    {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));          //等待时间到达

    SysTick->CTRL = 0x00;       //关闭计数器
    SysTick->VAL = 0X00;       //清空计数器
}




//--------------------------------------------------------------------------------------------
//us级延时
//SYSCLK单位为Hz,us单位为us,uS>=1
//本处理方法有一定误差,72MHz时实测误差<1uS
void delay_us(u32 SYSCLK, u32 us)
{
    u32 temp;

    SysTick->CTRL &= 0xfffffffb;       //选择外部时钟HCLK/8
    SysTick->LOAD = (u32) (us * (SYSCLK / 8000000)); //时间加载
    SysTick->VAL = 0x00;        //清空计数器
    SysTick->CTRL = 0x01;      //开始倒数

    do
    {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));      //等待时间到达

    SysTick->CTRL = 0x00;       //关闭计数器
    SysTick->VAL = 0X00;       //清空计数器
}
//--------------------------------------------------------------------------------------------

