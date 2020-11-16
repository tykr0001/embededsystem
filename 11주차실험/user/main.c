#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"

uint16_t value;
int color[12] = {WHITE, CYAN, BLUE, RED, MAGENTA, LGRAY, GREEN, YELLOW, BROWN, BRRED, GRAY};
char team[] = "MON_Team07";
char ON[] = "ON  ";
char OFF[] = "OFF";
char BUT[] = "BUT";
u16 PosX;
u16 PosY;
int LED1, LED2;
int LEDs_TOGGLE;
int cnt;

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void TIM_Configure(void);
void NVIC_Configure(void);

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
        
    // LCD_CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // LCD_RS, LCD_WR, LCD_RD, 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // DB00 ~ DB15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // LED 1, 2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void TIM_Configure(void){
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    
    TIM_InitStructure.TIM_Prescaler = 7200;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 10000;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
    
    // ADC1
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int num; 

void TIM2_IRQHandler(void){
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        cnt++;
        
        if(cnt % 1 == 0) LED1 = !LED1;
        if(cnt % 5 == 0) LED2 = !LED2;
        
        
        if(LEDs_TOGGLE && LED1){
          GPIO_SetBits(GPIOD, GPIO_Pin_2);
        }
        else if(LEDs_TOGGLE && !LED1){
          GPIO_ResetBits(GPIOD, GPIO_Pin_2);
        }
         if(LEDs_TOGGLE && LED2){
          GPIO_SetBits(GPIOD, GPIO_Pin_3);
        }
         else if(LEDs_TOGGLE && !LED2){
          GPIO_ResetBits(GPIOD, GPIO_Pin_3);
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

int main(void)
{
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    TIM_Configure();
    NVIC_Configure();
    
    LCD_Init();
    Touch_Configuration();
    Touch_Adjust();
    LCD_Clear(WHITE);
    
    uint16_t x, y;
    LCD_ShowString(10, 30, OFF, BLACK, WHITE);
    LCD_ShowString(10, 10, team, BLACK, WHITE);
    LCD_ShowString(60, 125, BUT, BLACK, WHITE);
    LCD_DrawRectangle(50, 100, 100, 150);
                
      
    while (1) {
        // TODO: implement
        PosX = 0;
        PosY = 0;
        Touch_GetXY(&x, &y, 1);
        
        Convert_Pos(x, y, &PosX, &PosY);
        
        if(50<=PosX && PosX <=100){
          if(100 <= PosY && PosY <= 150){
            LEDs_TOGGLE = !LEDs_TOGGLE;
            if(LEDs_TOGGLE)
              LCD_ShowString(10, 30, ON, BLACK, WHITE);
            else{
              LCD_ShowString(10, 30, OFF, BLACK, WHITE);
              GPIO_ResetBits(GPIOD, GPIO_Pin_2);
              GPIO_ResetBits(GPIOD, GPIO_Pin_3);
            }
           }
         }
        
        
           
    }
    
    return 0;
}