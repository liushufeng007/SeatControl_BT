#ifndef __ADCIF_H__
#define __ADCIF_H__
#include "public.h"
#include "user_init.h"


typedef enum
{
	ADCIF_CH_Motor4_HALL_AD = 0,  //ADC0
	ADCIF_CH_RAIL_MOTOR_HALL_AD = 1, //ADC1
	ADCIF_CH_BATT_SW_AD = 2, //ADC2
	ADCIF_CH_Motor4_SNSR_AD = 3, //ADC4
	ADCIF_CH_Motor3_SNSR_AD = 4,//ADC5
	ADCIF_CH_BATT1_AD = 5,//ADC6
	ADCIF_CH_Motor3_HALL_AD = 6,//ADC7
	ADCIF_CH_MotorBack_HALL_AD = 7,//ADC8
	ADCIF_CH_Seat_RAIL_AD = 8,//ADC11
	ADCIF_CH_SEAT_BACK_MTR_SNSR_AD = 9,//ADC12
	ADCIF_CH_BATT2_AD = 10,//ADC13
	ADCIF_CH_VERF_1_2_V = 11,//Verf
	ADCIF_CH_MAX_NUM = 12
}Adcif_Ch_e;


extern uint16_t Adcif_Get_AdcVal(uint8_t Adc_ch_u8);
extern void AdcIf_Polling(void);
extern void Adcif_DeInit(void);
extern void Adcif_Init(void);


#endif
