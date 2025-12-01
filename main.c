#include <stdint.h>
#include "ssd1306.h"

#define TIM4_BASE       0x40000800U
#define TIM4_CR1        (*(volatile uint32_t*)(TIM4_BASE + 0x00))
#define TIM4_PSC        (*(volatile uint32_t*)(TIM4_BASE + 0x28))
#define TIM4_ARR        (*(volatile uint32_t*)(TIM4_BASE + 0x2C))
#define TIM4_EGR        (*(volatile uint32_t*)(TIM4_BASE + 0x14))
#define TIM4_SR         (*(volatile uint32_t*)(TIM4_BASE + 0x10))

#define GPIOC_BASE      0x40011000U
#define GPIOC_CRH       (*(volatile uint32_t*)(GPIOC_BASE + 0x04))
#define GPIOC_ODR       (*(volatile uint32_t*)(GPIOC_BASE + 0x0C))

#define RCC_BASE        0x40021000U
#define RCC_APB2ENR     (*(volatile uint32_t*)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x1C))

void SPI1_Init(void);
void SSD1306_Init(void);
void SSD1306_DrawChessBoard(void);
void SSD1306_Update(void);

void delay(uint32_t ticks) {
    for (volatile uint32_t i = 0; i < ticks; i++);
}

void Timer4_Init(void) {
    RCC_APB1ENR |= (1 << 2);
    
    TIM4_PSC = 36000 - 1;
    TIM4_ARR = 500 - 1;
    
    TIM4_EGR = (1 << 0);
    
    TIM4_SR &= ~(1 << 0);
    

    TIM4_CR1 |= (1 << 0);
}

int main(void) {

    RCC_APB2ENR |= (1 << 4); 
    

    GPIOC_CRH &= ~(0xF << 20); 
    GPIOC_CRH |= (0x1 << 20); 
    

    for(int i = 0; i < 3; i++) {
        GPIOC_ODR &= ~(1 << 13); 
        delay(100000);
        GPIOC_ODR |= (1 << 13);  
        delay(100000);
    }
    
    SPI1_Init();
    SSD1306_Init();
    SSD1306_DrawChessBoard();
    SSD1306_Update();
    
    Timer4_Init();
    
    while(1) {
        if (TIM4_SR & (1 << 0)) {
            TIM4_SR &= ~(1 << 0);
            
            GPIOC_ODR ^= (1 << 13);
        }
    }
}