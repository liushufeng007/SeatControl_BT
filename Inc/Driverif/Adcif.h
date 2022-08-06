#ifndef __ADCIF_H__
#define __ADCIF_H__
#include "public.h"
#include "user_init.h"


typedef enum
{
	ADCIF_CH_Rotate_IN_IO_HALL_AD = 0,  //ADC0
	ADCIF_CH_Back_IN_IO_HALL_AD = 1, //ADC1
	ADCIF_CH_LampAD_AD = 2, //ADC2
	ADCIF_CH_Leg_IN_IO_HALL_AD = 3, //ADC3
	ADCIF_CH_Motorleg_CURAD_AD = 4,//ADC4
	ADCIF_CH_Motorhead_CURAD_AD = 5,//ADC5
	ADCIF_CH_Motorrotate_CURAD_AD = 6,//ADC6
	ADCIF_CH_Head_IN_IO_HALL_AD = 7,//ADC7
	ADCIF_CH_Angle_IO_HALL_AD = 8,//ADC8
	ADCIF_CH_Rail_IO_HALL_AD = 9,//ADC9
	ADCIF_CH_Motorback_CURAD_AD = 10,//ADC11
	ADCIF_CH_MotorSeat_CURAD_AD = 11,//ADC12
	ADCIF_CH_Motorangle_CURAD_AD = 12,//ADC13
	ADCIF_CH_VBAT_SW_AD_AD = 13,//ADC14
	ADCIF_CH_Venting_AD_AD = 14,//ADC17
	ADCIF_CH_VERF_1_2_V = 15,//Verf
	ADCIF_CH_MAX_NUM = 16
}Adcif_Ch_e;


extern uint16_t Adcif_Get_AdcVal(uint8_t Adc_ch_u8);
extern void AdcIf_Polling(void);
extern void Adcif_DeInit(void);
extern void Adcif_Init(void);


#endif
