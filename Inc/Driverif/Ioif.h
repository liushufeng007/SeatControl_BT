#ifndef __IOIF_H__
#define __IOIF_H__
#include "public.h"
#include "user_init.h"


typedef enum
{
/*PA0 - PA15*/
	GPIO_NUMBER_A0 = 0,
	GPIO_NUMBER_A1 = 1,
	GPIO_NUMBER_A2_NLMotorback_N = 2,
	GPIO_NUMBER_A3_NLMotorback_P = 3,
	GPIO_NUMBER_A4_Motorleg_P = 4,
	GPIO_NUMBER_A5_Motorleg_N = 5,
	GPIO_NUMBER_A6_Motorseat_N = 6,
	GPIO_NUMBER_A7_Motorsea_P = 7,
	GPIO_NUMBER_A8_Motorhead_P = 8,
	GPIO_NUMBER_A9_Motorhead_N = 9,
	GPIO_NUMBER_A10_Motorangle_N = 10,
	GPIO_NUMBER_A11_Motorangle_P = 11,
	GPIO_NUMBER_A12 = 12,
	GPIO_NUMBER_A13 = 13,
	GPIO_NUMBER_A14 = 14,
	GPIO_NUMBER_A15 = 15,
/*PB0~PB15*/
	GPIO_NUMBER_B0 = 16,
	GPIO_NUMBER_B1_AtLamp_EN,	
	GPIO_NUMBER_B2,
	GPIO_NUMBER_B3,
	GPIO_NUMBER_B4,	
	GPIO_NUMBER_B5,
	GPIO_NUMBER_B6,
	GPIO_NUMBER_B7	=	23	,
	GPIO_NUMBER_B8_Motorrotate_P,
	GPIO_NUMBER_B9_Motorrotate_N,
	GPIO_NUMBER_B10_VBAT_SW_EN = 26,
	GPIO_NUMBER_B11,	
	GPIO_NUMBER_B12	=	28	,
	GPIO_NUMBER_B13,	
	GPIO_NUMBER_B14,
	GPIO_NUMBER_B15,
	
/*PC0~PC15*/
	GPIO_NUMBER_C0 = 32,
	GPIO_NUMBER_C1	=	33	,
	GPIO_NUMBER_C2	=	34	,
	GPIO_NUMBER_C3	=	35	,
	GPIO_NUMBER_C4	=	36	,
	GPIO_NUMBER_C5	=	37	,
	GPIO_NUMBER_C6  = 38,
	GPIO_NUMBER_C7 =	39	,
	GPIO_NUMBER_C8	=	40	,
	GPIO_NUMBER_C9	=	41	,
	GPIO_NUMBER_C10,
	GPIO_NUMBER_C11=	43	,
	GPIO_NUMBER_C12	=	44	,
	GPIO_NUMBER_C13_BLE_Tran	=	45	,
	GPIO_NUMBER_C14_CAN_STB	=	46	,
	GPIO_NUMBER_C15	=	47	,
/*PD0~PD12*/

	GPIO_NUMBER_D0	=	48	,
	GPIO_NUMBER_D1	=	49	,
	GPIO_NUMBER_D2	=	50	,
	GPIO_NUMBER_D3	=	51	,
	GPIO_NUMBER_D4	=	52	,
	GPIO_NUMBER_D5_IGN_INT	=	53	,
	GPIO_NUMBER_D6	=	54	,
	GPIO_NUMBER_D7	=	55	,
	GPIO_NUMBER_D8	=	56	,
	GPIO_NUMBER_D9,
	GPIO_NUMBER_D10	=	58	,
	GPIO_NUMBER_D11	=	59	,
	GPIO_NUMBER_D12	=	60	,
/*PE0~PE9*/
	GPIO_NUMBER_E0	=	61	,
	GPIO_NUMBER_E1_BEL_STATUS	=	62	,
	GPIO_NUMBER_E2_BLE_Disc	=	63	,
	GPIO_NUMBER_E3_BLE_RESET	=	64	,
	GPIO_NUMBER_E5	=	65	,
	GPIO_NUMBER_E6	=	66	,
	GPIO_NUMBER_E7_VDD_EN	=	67	,
	GPIO_NUMBER_E8	=	68	,
	GPIO_NUMBER_E9	=	69	,
	GPIO_NUMBER_MAX = 70
}Ioif_Gpio_Num_e;

#if 0
FL_GPIO_EXTI_LINE_3,//PA12
FL_GPIO_EXTI_LINE_14,//PD9
FL_GPIO_EXTI_LINE_18,//PE8
FL_GPIO_EXTI_LINE_4,//PB0
FL_GPIO_EXTI_LINE_5,//PB7
FL_GPIO_EXTI_LINE_6,//PB11
FL_GPIO_EXTI_LINE_7,//PB12
FL_GPIO_EXTI_LINE_11,//PC13
FL_GPIO_EXTI_LINE_8,//PC0
FL_GPIO_EXTI_LINE_9,//PC6
FL_GPIO_EXTI_LINE_10,//PC11
FL_GPIO_EXTI_LINE_13,//PD5
FL_GPIO_EXTI_LINE_16,//PE0
#endif
	
typedef enum
{
	GPIO_EXTI_0 = 0,
	GPIO_EXTI_1,	
	GPIO_EXTI_2,
	GPIO_EXTI_3,
	GPIO_EXTI_4,
	GPIO_EXTI_5,
	GPIO_EXTI_6,
	GPIO_EXTI_7,
	GPIO_EXTI_8  = 8,
	GPIO_EXTI_9  ,
	GPIO_EXTI_10 ,
	GPIO_EXTI_11 ,
	GPIO_EXTI_12 ,
	GPIO_EXTI_13 ,
	GPIO_EXTI_14 ,
	GPIO_EXTI_15 ,
	GPIO_EXTI_16 = 16,
	GPIO_EXTI_17 ,
	GPIO_EXTI_18 ,
	GPIO_EXTI_NUMBER_MAX = 19
}Ioif_EXTI_Num_e;
	

extern void Ioif_GPIO_Init(void);
extern void Ioif_GPIO_DeInit(void);
extern uint8_t Ioif_GetPinLevel(Ioif_Gpio_Num_e GpioNum_u8);
extern uint8_t Ioif_SetPinLevel(Ioif_Gpio_Num_e GpioNum_u8,uint8_t GpioLevel_u8);

extern void Ioif_ClearAll_WakeupSrc(void);
extern void Ioif_Set_WakeupSrc(uint8_t fl_index_u8);
extern uint8_t Ioif_Get_WakeupSrc(uint8_t fl_index_u8);


#endif
