// Port configuration register low
// Address offset:0x00
// reset value:0x4444 4444

// Port configuration register high
// Address offset:0x04
// reset value:0x4444 4444

// Port input data register
// Address offset:0x08
// reset value:0x0000 XXXX

// Port ouyput data register
// Address offset:0x0C
// reset value:0x0000 0000

// Port bit set/reset register
// Address offset:0x10
// reset value:0x0000 0000

// volatile static unsigned int* RCC_BA = (unsigned int*)0x40021000;
volatile static unsigned int* RCC_APB2_CER = (unsigned int*)(0x40021000 + 0x18);

// volatile static unsigned int* PORT_C_BA = (unsigned int*)0x40011000;
volatile static unsigned int* PORT_C_CRL = (unsigned int*)(0x40011000 + 0x00);
volatile static unsigned int* PORT_C_IDR = (unsigned int*)(0x40011000 + 0x08);

// volatile static unsigned int* PORT_D_BA = (unsigned int*)0x40011400;
volatile static unsigned int* PORT_D_CRL = (unsigned int*)(0x40011400 + 0x00);
volatile static unsigned int* PORT_D_SRR = (unsigned int*)(0x40011400 + 0x10);

int main(void)
{
  //initializing
  *RCC_APB2_CER = 0x00000000; // APB2 peripheral clock enable register reset
  *RCC_APB2_CER |= 0x00000030; // set PORT C & D clock enable
  
  *PORT_C_CRL = 0x44444444; // PORT_C config reset
  *PORT_D_CRL = 0x44444444; // PORT_D config reset
  
  *PORT_C_CRL = 0x44888844; // C포트 2~5번 핀 input mode with pull-up/ pull-down
  *PORT_D_CRL = 0x14411144; // D포트 2,3,4,7번 핀 general purpose output push-pull, max 10MHz
  
  while(1)
  {
    if(~(*PORT_C_IDR) & 0x04) // C포트 2번 핀 입력받을 경우 DOWN
    {
      *PORT_D_SRR = (0x04 | 0x08); // D포트 2, 3번 핀 ON
    }
    else if(~(*PORT_C_IDR) & 0x08) // C포트 3번 핀 입력받을 경우 LEFT
    {
      *PORT_D_SRR = (0x10 | 0x80)<<16; // D포트 4, 7번 핀 OFF
    }
    else if(~(*PORT_C_IDR) & 0x10) // C포트 4번 핀 입력받을 경우 RIGHT
    {
      *PORT_D_SRR = (0x04 | 0x08)<<16; // D포트 2, 3번 핀 OFF
    }
    else if(~(*PORT_C_IDR) & 0x20) // C포트 5번 핀 입력받을 경우 UP
    {
      *PORT_D_SRR = (0x10 | 0x80); // D포트 4, 7번 핀 ON
    }
  }
  
  return 0;
}