#ifndef __GPIOBAND_H
#define __GPIOBAND_H 			   

#define GPIOAOUT(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOA->ODR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOBOUT(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOB->ODR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOCOUT(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOC->ODR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIODOUT(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOD->ODR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOEOUT(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOE->ODR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOFOUT(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOF->ODR)& 0xFFFFF)<<5) + (n<<2)))

#define GPIOAIN(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOA->IDR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOBIN(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOB->IDR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOCIN(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOC->IDR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIODIN(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOD->IDR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOEIN(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOE->IDR)& 0xFFFFF)<<5) + (n<<2)))
#define GPIOFIN(n) *((vu32*)(PERIPH_BB_BASE+(((u32)(&GPIOF->IDR)& 0xFFFFF)<<5) + (n<<2)))

#endif
