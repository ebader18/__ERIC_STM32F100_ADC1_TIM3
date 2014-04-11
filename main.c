#include "main.h"
#include "stm32f10x.h"

#define __READ_BIT(var,pos) ((var) & (1<<(pos)))
#define __CLEAR_BIT(var,pos) ((var) &= ~(1<<(pos)))
#define __SET_BIT(var,pos) ((var) |= (1<<(pos)))
#define __TOGGLE_BIT(var,pos) ((var) ^= (1<<(pos)))

void Configure_RCC();
void Configure_LEDs();
void Configure_ADC1();
void Configure_TIM3();

int main(void)
{
  uint32_t adc_value = 0;
  
  Configure_RCC();
  Configure_LEDs();
  Configure_ADC1();
  Configure_TIM3();
  
  __CLEAR_BIT(GPIOC->ODR, 8);
  __SET_BIT(GPIOC->ODR, 9);
  
  TIM_Cmd(TIM3,ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  
  while(1)
  {
    while (__READ_BIT(ADC1->SR,1) == 0);
    adc_value = ADC1->DR;
    if (adc_value > 2047)
    {
      __TOGGLE_BIT(GPIOC->ODR, 8);
      __TOGGLE_BIT(GPIOC->ODR, 9);
    }
  }
}

void Configure_RCC()
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;                                            // Enable GPIOC Clock
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;                                            // Enable ADC1 Clock
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;                                            // Enable GPIOA Clock
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                                            // Enable Alternate Fuctions Clock
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;                                            // Enable TIM3
}

void Configure_LEDs()
{
  GPIOC->CRH = 0x44444422;                                                      // PC8 and PC9 as General Purpose Push Pull Output Max Speed = 2MHz
  GPIOA->CRL = 0x44444404;                                                      // PA1 as input analog mode
}

void Configure_ADC1()
{
  ADC_InitTypeDef ADC_InitStructure;
  
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
  
  ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}

void Configure_TIM3()
{
  TIM_TimeBaseInitTypeDef TimeBaseInitStructure; 
  
  TimeBaseInitStructure.TIM_Prescaler = 1000 - 1;
  TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TimeBaseInitStructure.TIM_Period = SystemCoreClock/1000000 - 1;
  TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3,&TimeBaseInitStructure);
  
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}