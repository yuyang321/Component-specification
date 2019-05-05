#ifndef _GUSER_H_
#define _GUSER_H_

	#include "..\gos\glib.h"
	
	//#define _DEBUG_
/**
	USART FIFO  MACRO/Variable
**/
	#define USART_LCD_RxBUFF_SIZE		200
	#define USART_LCD_TxBUFF_SIZE		200
	extern FIFO_t LCDRxFifo;
	extern FIFO_t LCDTxFifo;
/*
	Timer Task Parm
*/
#define LED_TOGGLE_INIT_GSM_INTERVAL_TIME					500
#define FOOTSWITCH_DETECT_INTERVAL_TIME						25
#define LCD_DISPLAY_INTERVAL_TIME									10

#define LED_TOGGLE_MQTT_CONNECT_OK_INTERVAL_TIME	500
#define ACQ_CHANNEL_SCAN_INTERVAL_TIME						12000
#define ANALOG_CHANNEL_ACQ_INTERVAL_TIME					80
#define GSM_AT_RESPONSE_MAX_DELAY_TIME						3000
#define GSM_INIT_MAX_INTERVAL_TIME								120000
#define GSM_PUBLISH_MAX_INTERVAL_TIME							5000
#define GSM_SUBSCRIBE_MAX_INTERVAL_TIME						5000
/*
	App Struct
*/
// Master StateMachine
		typedef enum _APL_STATE_MACHINE_E_{
		APL_STATE_PWON_LOW_2SECSOND =1,
		APL_STATE_SYSTEM_INIT,
		APL_STATE_NORMAL_WORK,
	}APL_STATE_MACHINE_e;
/*****************************************************************************************************/
/**
	Sensor Cycle data
**/

/**
	Analog Val Filt Arg
**/
	#define MOVING_WINDOW_FILT		0
	#define INTEGER_WINDOW_FILT		1
	#define ADC_FILT_MODE					MOVING_WINDOW_FILT
/**
	External  Function
**/
	extern int main(void);
	extern void GosInit(void);
	extern void HAL_GSMRX_EVENT(void *arg);
	extern void HAL_EXIT_EVENT(void *arg);
	extern void HAL_GSMTX_EVENT(void *arg);
	extern void HAL_TIMER_EVENT(void *arg);
	extern void SYS_APL_EVENT(void *arg);
	extern void HAL_FAULT_EVENT(void *arg);
#endif
