#include "bsp.h"
#include "guser.h"
#include "..\mid\glcd.h"
/******************************************************************************/
//all app state machine
//App Work Sequence
	APL_STATE_MACHINE_e aplstate =  APL_STATE_PWON_LOW_2SECSOND;
/******************************************************************************/
//USART Buffer and fifo
static uint8_t LCDRxBuf[USART_LCD_RxBUFF_SIZE];
static uint8_t LCDTxBuf[USART_LCD_TxBUFF_SIZE];
FIFO_t LCDRxFifo;
FIFO_t LCDTxFifo;
/******************************************************************************/
//the string which need to compare with RxString
//static uint8_t cmpbuffa[sizeof(AT_READY)-1];
//FIFO_t cmpbuffaFifo;

//led toggle timer indicating GSM Module work 
static SOFT_TIMER_t sledtimer;
static void gsledtoggle(void *arg)
{
	STATUS_LED1_TOGGLE;
}
//led toggle timer indicating GSM Module work 
static SOFT_TIMER_t lcdisptimer;
static uint16_t disptime = 0;
static void glcdisptime(void *arg)
{
	gSendDisplay(VAR_TIME, ++disptime);
}
//footswitch detect timer
#define VOLID_SWITCH_PRESSED_CNT	(150 / FOOTSWITCH_DETECT_INTERVAL_TIME)
static SOFT_TIMER_t fswitchtimer;
typedef enum KEY_DETECT_STATE_MACHINE_E{
	KEY_PRESSED_FILT_DETECTING_STATE_E,
	KEY_UNPRESSED_FILT_DETECTING_STATE_E,
}KEY_PRESS_STATE_MACHINE_e;
static void gfswitchdetect(void *arg)
{
	static uint8_t pressedCnt = 0,unpressedCnt = 0;
	static KEY_PRESS_STATE_MACHINE_e keymachine = KEY_UNPRESSED_FILT_DETECTING_STATE_E;
	switch(keymachine)
	{
		case KEY_PRESSED_FILT_DETECTING_STATE_E:
			if(FOOTSWITCH_STATUS_PRESSED == GET_FOOTSWITCH_STATUS())
			{
				pressedCnt++;
				if(pressedCnt >= VOLID_SWITCH_PRESSED_CNT)
				{
					pressedCnt = 0;
					//start lcd display time
					disptime = 0;
					GTimerStart(&lcdisptimer);
					keymachine = KEY_UNPRESSED_FILT_DETECTING_STATE_E;
				}
			}
			else
			{
					pressedCnt = 0;
			}
		break;
		case KEY_UNPRESSED_FILT_DETECTING_STATE_E:
			if(FOOTSWITCH_STATUS_UNPRESSED == GET_FOOTSWITCH_STATUS())
			{
				unpressedCnt++;
				if(unpressedCnt >= VOLID_SWITCH_PRESSED_CNT)
				{
					unpressedCnt = 0;
					//stop lcd display timer
					GTimerStop(&lcdisptimer);
					keymachine = KEY_PRESSED_FILT_DETECTING_STATE_E;
				}
			}
			else
			{
					unpressedCnt = 0;
			}
		break;
		default:
				
		break;
	}
	
}
//node analog channel acquire
static SOFT_TIMER_t AnalogAcqTimer;
static void gAnalogAcqTimer(void *arg)
{
  uint8_t loop;
	if( !adconvertflag )
		return;
	adconvertflag = 0;
	#if(ADC_FILT_MODE == INTEGER_WINDOW_FILT)
		if(adcbufcnt >= ADC_BUFF_NUM)
		{
			adcbufcnt = 0;
			//Filt Arithmetic
			for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
				gLPfiltOneOrder(ADChannelCurVal+loop,ADChannellastVal+loop,ADChannelValBuf[loop],ADC_BUFF_NUM,ADC_FILT_ARG_VAL);
		}
	#elif(ADC_FILT_MODE == MOVING_WINDOW_FILT)
		//Filt Arithmetic
		for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
				gLPfiltOneOrder(ADChannelCurVal+loop,ADChannellastVal+loop,ADChannelValBuf[loop],ADC_BUFF_NUM,ADC_FILT_ARG_VAL);
		adcbufcnt%=ADC_BUFF_NUM;
	#endif
	ADC_Cmd(ADC1,ENABLE);
	ADC_SoftwareStartConvCmd(ADC_USED,ENABLE);
}

/**
	system event handle:
		HAL_EXIT_EVENT
		HAL_LCDTX_EVENT
		HAL_FAULT_EVENT
		HAL_TIMER_EVENT
		HAL_LCDRX_EVENT
**/
void HAL_EXIT_EVENT(void *arg)
{

}
void HAL_GSMTX_EVENT(void *arg)
{
	uint8_t txdata;	
	GFIFOPop(&LCDTxFifo,&txdata);
	USART_SendData(USART_LCD, (uint16_t)txdata );
}
void HAL_FAULT_EVENT(void *arg)
{
	switch(HalFaultCode)
	{
		case USART_GSM_FIFO_FULL:
		break;
		case USART_GSM_FIFO_EMPTY:
			USART_ITConfig(USART_LCD, USART_IT_TC , DISABLE);
		break;
		default:;
	}
}
void HAL_TIMER_EVENT(void *arg)
{
	GTimerLoop();
}

//USART RX Handle
void HAL_GSMRX_EVENT(void *arg)
{
	uint8_t rxdata;
	while(GFIFOPop(&LCDRxFifo,&rxdata) == EXE_FINISH)
	{
		
	}
}
/**************************************************************************************/
//system init
void GosInit(void)
{
	//timer init
	InitTimerlist();
	//Init Usart pipe
	GFIFOInit(&LCDRxFifo,LCDRxBuf,sizeof(LCDRxBuf));
	GFIFOInit(&LCDTxFifo,LCDTxBuf,sizeof(LCDTxBuf));
	//Init app Pipe
	//GFIFOInit(&cmpbuffaFifo,cmpbuffa,sizeof(cmpbuffa));
	//start app layer
	//led indicate that GSM Module is registering
	GTimerNodeInit(&sledtimer,gsledtoggle, LED_TOGGLE_INIT_GSM_INTERVAL_TIME , LED_TOGGLE_INIT_GSM_INTERVAL_TIME );
	GTimerStart(&sledtimer);
	GTimerNodeInit(&fswitchtimer,gfswitchdetect, FOOTSWITCH_DETECT_INTERVAL_TIME , FOOTSWITCH_DETECT_INTERVAL_TIME );
	GTimerStart(&fswitchtimer);
	GTimerNodeInit(&lcdisptimer,glcdisptime, LCD_DISPLAY_INTERVAL_TIME , LCD_DISPLAY_INTERVAL_TIME );
	gSendDisplay(VAR_TIME, 0);
	SET_EVENT( SYS_APL_EVENT_E );
	//Init Sensor Acquired Data
	
}
/**************************************************************************************/
//App Master state
void SYS_APL_EVENT(void *arg)
{
	
	switch(aplstate)
	{
		case APL_STATE_PWON_LOW_2SECSOND:

		break;
		case APL_STATE_SYSTEM_INIT:
		
		break;
		case APL_STATE_NORMAL_WORK:
		
		break;
	}
	SET_EVENT( SYS_APL_EVENT_E );
}
