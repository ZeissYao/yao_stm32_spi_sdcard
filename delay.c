#include "stm32f10x.h"

//--------------------------------------------------------------------------------------------
//ms����ʱ
//SYSCLK��λΪHz,ms��λΪms
//��72MHzʱ,1<=ms<=1864 
void delay_ms(u32 SYSCLK, u16 ms)
{
    u32 temp;

    SysTick->CTRL &= 0xfffffffb; //ѡ���ⲿʱ��HCLK/8
    SysTick->LOAD = (u32) (ms * (SYSCLK / 8000)); //ʱ�����(SysTick->LOADΪ24bit)
    SysTick->VAL = 0x00;           //��ռ�����
    SysTick->CTRL = 0x01;          //��ʼ����

    do
    {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));          //�ȴ�ʱ�䵽��

    SysTick->CTRL = 0x00;       //�رռ�����
    SysTick->VAL = 0X00;       //��ռ�����
}




//--------------------------------------------------------------------------------------------
//us����ʱ
//SYSCLK��λΪHz,us��λΪus,uS>=1
//����������һ�����,72MHzʱʵ�����<1uS
void delay_us(u32 SYSCLK, u32 us)
{
    u32 temp;

    SysTick->CTRL &= 0xfffffffb;       //ѡ���ⲿʱ��HCLK/8
    SysTick->LOAD = (u32) (us * (SYSCLK / 8000000)); //ʱ�����
    SysTick->VAL = 0x00;        //��ռ�����
    SysTick->CTRL = 0x01;      //��ʼ����

    do
    {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));      //�ȴ�ʱ�䵽��

    SysTick->CTRL = 0x00;       //�رռ�����
    SysTick->VAL = 0X00;       //��ռ�����
}
//--------------------------------------------------------------------------------------------

