
#include "bsp.h"
#include "guser.h"

void ghwrccset(void)
{
	RCC_DeInit();
	RCC_LSICmd(ENABLE);
	RCC_HSICmd(DISABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_2);
	#if defined(STM32F103RC) || defined(STM32F103RB)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
	#elif	defined(STM32F105RB)
		RCC_PLLConfig(RCC_PLLSource_PREDIV1,RCC_PLLMul_9);
	#endif
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	RCC_ClocksTypeDef RCC_ClocksVar;
	RCC_GetClocksFreq(&RCC_ClocksVar);
	SystemCoreClock = RCC_ClocksVar.HCLK_Frequency;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(SystemCoreClock/1000);
	//NVIC_SetPriority (SysTick_IRQn, 0);
}

void gnvicset(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_GSM_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_GSM_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXIT_CH0_7_PREPRIORITY;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = EXIT_CH0_7_SUBPRIORITY;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ADCDMA_PRIORITY_PREEMPTION;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ADCDMA_PRIORITY_SUBPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void GenericGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//Board Status LED Init
	RCC_APB2PeriphClockCmd( STATUS_LED_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = STATUS_LED1_GPIO_PIN | STATUS_LED2_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(STATUS_LED_GPIO, &GPIO_InitStructure);
	//Board V1¡¢V2¡¢V3¡¢FAM GPIO Init
	RCC_APB2PeriphClockCmd( FREEZE_FAN_VALVE_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = FREEZE_FAN_GPIO_PIN | FREEZE_CTRL1_GPIO_PIN | FREEZE_CTRL2_GPIO_PIN \
																| FREEZE_CTRL3_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(FREEZE_FAN_VALVE_GPIO, &GPIO_InitStructure);
	//Board footswitch GPIO Init
	RCC_APB2PeriphClockCmd( FOOTSWITCH_INPUT_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = FOOTSWITCH_INPUT_CH_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(FOOTSWITCH_INPUT_GPIO, &GPIO_InitStructure);	
}

void BspInit(void)
{
	GenericGpioInit();
	ginitusart();
	gInitAnalogADC();
}
