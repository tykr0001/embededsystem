// Controller2 stm32f107 board

#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"

// [0:1] => flex sensor, [2:4] => 3-axis accel sensor
volatile uint32_t ADC_Value[5]; 

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void ADC_Configure(void);
void TIM_Configure(void);
void DMA_Configure(void);
void NVIC_Configure(void);
void USART1_Init(void);
void USART2_Init(void);
void Delay(void);
void sendDataUART1(uint16_t data);
void sendDataUART2(uint16_t data);
void sendU32DataUART2(uint32_t data);


//---------------------------------------------------------------------------------------------------

void RCC_Configure(void) {
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
    // ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
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

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'

    // ADC12_IN4, IN5, IN6 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ADC12_IN10, IN11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // TIM2_CH2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
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

void ADC_Configure(void) {

    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 5;
    ADC_Init(ADC1, &ADC_InitStructure);

    // flex Sensor
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_71Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_71Cycles5);
    
    // 3-axis Acceleration Sensor
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_71Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 4, ADC_SampleTime_71Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_71Cycles5);
    
    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)); //->while ?¡þi?¨Ï??i?¡þ Ou¢¥i?¡þ

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)); //->while ?¡þi?¨Ï??i?¡þ Ou¢¥i?¡þ

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void TIM_Configure(void) {
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

void TIM_PWM_Configure(void ) {
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
    
    //TIM_SetCompare2(TIM2, uint16_t Compare2);
}

void DMA_Configure(void) {

    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Channel1);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_BASE + 0x4C;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 5;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
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

        // TODO implement
        sendDataUART1(word);
        
        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

void Delay(){
   int i;
   for (i = 0; i < 200000; i++) {}
}

void sendDataUART1(uint16_t data) {
   USART_SendData(USART1, data);
   while ((USART1->SR & USART_SR_TC) == 0);
}

void sendDataUART2(uint16_t data) {
   USART_SendData(USART2, data);
   while ((USART2->SR & USART_SR_TC) == 0);
}

void sendU32DataUART2(uint32_t data) {
    //sendDataUART2(((data&0xf000)>>12) + '0');
    //sendDataUART2(((data&0xf00)>>8) + '0');
    sendDataUART2(data / 1000 + '0');
    data %= 1000;
    sendDataUART2(data / 100 + '0');
    data %= 100;
    sendDataUART2(data / 10 + '0');
    data %= 10;
    sendDataUART2(data + '0');
}

const char msg[] = "Hello Team07\r\n";


int main(void)
{
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    ADC_Configure();
    DMA_Configure();
    USART1_Init();
    USART2_Init();
    NVIC_Configure();
    
    
   int i=0;
   while (1) {
        sendDataUART2('-');
        sendDataUART2('\n');
        sendU32DataUART2(ADC_Value[0]);
        sendDataUART2(' ');
        sendDataUART2('F');
        sendU32DataUART2(ADC_Value[1]);
        sendDataUART2('Z');
        sendDataUART2('A');
        sendU32DataUART2(ADC_Value[2]);
        sendDataUART2('Y');
        sendU32DataUART2(ADC_Value[3]);
        sendU32DataUART2(ADC_Value[4]);
        //sendU32DataUART2(flex_Sensor[1]);
        //sendDataUART2(value);
        Delay();
    }
    
    return 0;
}