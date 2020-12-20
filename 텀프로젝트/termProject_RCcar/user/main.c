// RCcar

#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"

uint32_t ADC_Value[1];

uint16_t flex_sensor;
uint16_t tmp_flex_sensor;
uint16_t accel_sensor;
uint16_t tmp_accel_sensor;
int BT_FLEX_FLAG;
int BT_ACCEL_FLAG;
int cnt, idx;

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void USART1_Init(void);
void USART2_Init(void);
void ADC_Configure(void);
void TIM_Configure(void);
void DMA_Configure(void);
void NVIC_Configure(void);
void sendDataUART1(uint16_t data);
void sendDataUART2(uint16_t data);

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
    // ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // TIM2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    /* UART1 TX/RX port clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    /* USART1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    /* USART2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
     /* Alternate Function IO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);


}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'

    // ADC12_IN0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // TIM2_CH1, CH2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // motor driver
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* UART1 pin setting */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* UART2 pin setting */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void USART1_Init(void) {
    USART_InitTypeDef USART1_InitStructure;

    // Enable the USART1 peripheral
    USART_Cmd(USART1, ENABLE);
   
    // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
    USART1_InitStructure.USART_BaudRate=9600;
    USART1_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART1_InitStructure.USART_StopBits=USART_StopBits_1;
    USART1_InitStructure.USART_Parity=USART_Parity_No;
    USART1_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
    USART1_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART1_InitStructure);
   
    // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USART2_Init(void) {
    USART_InitTypeDef USART2_InitStructure;

    // Enable the USART1 peripheral
    USART_Cmd(USART2, ENABLE);
   
    // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
    USART2_InitStructure.USART_BaudRate=9600;
    USART2_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART2_InitStructure.USART_StopBits=USART_StopBits_1;
    USART2_InitStructure.USART_Parity=USART_Parity_No;
    USART2_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
    USART2_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART2_InitStructure);
   
    // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void USART1_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
        // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);

        // TODO implement
        sendDataUART2(word);
        
        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

void USART2_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
        // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART2);
        if(word=='Z'){
          BT_FLEX_FLAG = 0;
          flex_sensor = tmp_flex_sensor;
          tmp_flex_sensor = 0;
          /*
          sendDataUART1(flex_sensor/1000+'0');
          sendDataUART1(flex_sensor/100%10+'0');
          sendDataUART1(flex_sensor/10%10+'0');
          sendDataUART1(flex_sensor%10+'0');
          */
        }
        else if(BT_FLEX_FLAG == 1){
          tmp_flex_sensor = tmp_flex_sensor * 10 + word - '0';
        }
        else if(word=='F') {
          BT_FLEX_FLAG = 1;
        }
        
        else if(word=='Z'){
          BT_ACCEL_FLAG = 0;
          accel_sensor = tmp_accel_sensor;
          tmp_flex_sensor = 0;
          /*
          sendDataUART1(flex_sensor/1000+'0');
          sendDataUART1(flex_sensor/100%10+'0');
          sendDataUART1(flex_sensor/10%10+'0');
          sendDataUART1(flex_sensor%10+'0');
          */
        }
        else if(BT_ACCEL_FLAG == 1){
          tmp_accel_sensor = tmp_accel_sensor * 10 + word - '0';
        }
        else if(word=='A'){
          BT_ACCEL_FLAG = 1;
        }
        
        // TODO implement
        sendDataUART1(word);
        
        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

void ADC_Configure(void) {

    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_71Cycles5);
    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)); //->while ?¡þi?¨Ï??i?¡þ Ou¢¥i?¡þ

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)); //->while ?¡þi?¨Ï??i?¡þ Ou¢¥i?¡þ

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void TIM_Configure(void){
    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    // 1kHz
    TIM_InitStructure.TIM_Prescaler = 72-1;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Period = 1000-1;
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM2, &TIM_InitStructure);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void TIM_PWM_Configure(){
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 999;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    //TIM_SetCompare2(TIM2, uint16_t Compare2);
}

void DMA_Configure(void) {

    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Channel1);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_BASE + 0x4C;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0x1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
    //DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
    
    // UART1
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // UART2
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}


void delay(){
    int i = 0;  
    for(;i<10000000;++i);
}


void sendDataUART1(uint16_t data) {
   USART_SendData(USART1, data);
   //while ((USART1->SR & USART_SR_TC) == 0);
}

void sendDataUART2(uint16_t data) {
   USART_SendData(USART2, data);
   //while ((USART1->SR & USART_SR_TC) == 0);
}

int main(void)
{
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    ADC_Configure();
    DMA_Configure();
    TIM_Configure();
    TIM_PWM_Configure();
    USART1_Init();
    USART2_Init();
    NVIC_Configure();
    
    //LCD_Init();
    //Touch_Configuration();
    //Touch_Adjust();
    //LCD_Clear(WHITE);

    int Prev_Value = ADC_Value[0];
    while (1) {
        int speed = 1000;
        /*
        // TODO: implement
        int illuminance = ADC_Value[0];
        if (illuminance > 300) {
            if (Prev_Value <= 300)
                LCD_Clear(GRAY);
            LCD_ShowNum(10, 50, illuminance, 4, WHITE, GRAY);
        }
        else {
            if (Prev_Value > 300)
                LCD_Clear(WHITE);
            LCD_ShowNum(10, 50, illuminance, 4, BLACK, WHITE);
        }
        Prev_Value = illuminance;
        */
        //GPIO_SetBits(GPIOA, GPIO_Pin_1);
        //GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        /*for(int i=10000000-1;i>0;--i){
            TIM_SetCompare1(TIM2, i/10000);
            TIM_SetCompare2(TIM2, i/10000);
        }*/
        if(flex_sensor>2700)
            ;
        else{
            if(accel_sensor < 2000){
                speed = flex_sensor/3;
            }
            else{
                speed = flex_sensor/2;
            }
        }
        
        
        TIM_SetCompare2(TIM2, speed);
        
        /*
        delay();
        TIM_SetCompare2(TIM2, 300);
        delay();
        TIM_SetCompare2(TIM2, 600);
        delay();
        TIM_SetCompare2(TIM2, 900);
        delay();
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        */
        //GPIO_SetBits(GPIOB, GPIO_Pin_1);
        //delay();
        //GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        //delay();
    }

    return 0;
}