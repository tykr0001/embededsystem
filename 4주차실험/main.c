// Port configuration register low
// Address offset:0x00
// reset value:0x4444 4444

// Port configuration register high
// Address offset:0x04
// reset value:0x4444 4444

// Port input data register
// Address offset:0x08
// reset value:0x0000 XXXX

// Port output data register
// Address offset:0x0C
// reset value:0x0000 0000

// Port bit set/reset register
// Address offset:0x10
// reset value:0x0000 0000

void delay() {
  int i;
  for (i = 0; i < 10000000; i++) ;
}

// volatile static unsigned int* RCC_BA = (unsigned int*)0x40021000;
volatile static unsigned int* RCC_APB2_CER = (unsigned int*)(0x40021000 + 0x18);

// volatile static unsigned int* PORT_B_BA = (unsigned int*)0x40010C00;
volatile static unsigned int* PORT_B_CRH = (unsigned int*)(0x40010C00 + 0x04);
volatile static unsigned int* PORT_B_IDR = (unsigned int*)(0x40010C00 + 0x08);

// volatile static unsigned int* PORT_C_BA = (unsigned int*)0x40011000;
volatile static unsigned int* PORT_C_CRH = (unsigned int*)(0x40011000 + 0x04);
volatile static unsigned int* PORT_C_SRR = (unsigned int*)(0x40011000 + 0x10);

// volatile static unsigned int* PORT_D_BA = (unsigned int*)0x40011400;
volatile static unsigned int* PORT_D_CRH = (unsigned int*)(0x40011400 + 0x04);
volatile static unsigned int* PORT_D_IDR = (unsigned int*)(0x40011400 + 0x08);

int main(void)
{
  //initializing
  *RCC_APB2_CER = 0x00000000; // APB2 peripheral clock enable register reset
  *RCC_APB2_CER |= 0x00000038; // set PORT B & C & D clock enable
  
  *PORT_B_CRH = 0x44444444; // PORT_C configuration high reset
  *PORT_C_CRH = 0x44444444; // PORT_C configuration high reset
  *PORT_D_CRH = 0x44444444; // PORT_D configuration high reset
  
  *PORT_B_CRH = 0x44444448; // B포트 8번 핀 input mode with pull-up/ pull-down
  *PORT_C_CRH = 0x44444411; // C포트 8,9번 핀 general purpose output push-pull, max 10MHz
  *PORT_D_CRH = 0x44488444; // D포트 11, 12번 핀 input mode with pull-up/ pull-down
  
  *PORT_C_SRR = 0x300; // C포트 8,9번 핀 OFF로 초기화

  while(1)
  {
    if(~(*PORT_B_IDR) & 0x100) // B포트 8번 핀 입력받을 경우 SELECTION
    {
      *PORT_C_SRR = (0x300) << 16; // C포트 8,9번 핀 ON
      delay();
      *PORT_C_SRR = 0x300; // C포트 8,9번 핀 OFF
    }
    
    if(~(*PORT_D_IDR) & 0x800) // D포트 11번 핀 입력받을 경우 S1 USER
    {
      *PORT_C_SRR = (0x200) << 16; // C포트 9번 핀 ON
      delay();
      *PORT_C_SRR = 0x200; // C포트 9번 핀 OFF
    }
    
    if(~(*PORT_D_IDR) & 0x1000) // D포트 12번 핀 입력받을 경우 S2 USER
    {
      *PORT_C_SRR = (0x100) << 16; // C포트 8번 핀 ON
      delay();
      *PORT_C_SRR = 0x100; // C포트 8번 핀 OFF
    }
  }
  
  return 0;
}