#include <stdint.h>
#include <stm32f10x.h>

void delay(volatile uint32_t ticks) {
    for (volatile uint32_t i = 0; i < ticks; i++) {
        __NOP();
    }
}

int __attribute__((noreturn)) main(void) {

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 


    GPIOC->CRH &= ~GPIO_CRH_CNF13;      
    GPIOC->CRH |= GPIO_CRH_MODE13_0;     


    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); 
    GPIOA->CRL |= GPIO_CRL_CNF0_1;                   
    GPIOA->ODR |= GPIO_ODR_ODR0;                   

    while (1) {
        uint32_t button_pressed = !(GPIOA->IDR & GPIO_IDR_IDR0);

        uint32_t delay_val = button_pressed ? 200000 : 1000000;

        GPIOC->ODR &= ~(1U << 13);  
        delay(delay_val);

        GPIOC->ODR |= (1U << 13);  
        delay(delay_val);
    }
}