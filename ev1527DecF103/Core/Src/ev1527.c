/*
 * ev1527.c
 *
 *  Created on: Oct 11, 2023
 *      Author: m.reza
 */

#include <main.h>
#include <stdint.h>
#include <stdbool.h>
#include <ev1527.h>

typedef void (*RfCallbackFunc_t)(uint32_t );
typedef struct _ev1527{
	IRQn_Type ExternalInterrupt;
	uint16_t myGpioPin;
	GPIO_TypeDef* myGpioPort;
	RfCallbackFunc_t myCallBack;
}ev1527_t;

ev1527_t Ev1527_Utillities;
static float data[25];
static bool NewDataRecieved;
static uint32_t remoteCodeAndData;

uint8_t RefineBit(float in);
bool validateData(void);
uint16_t ev1527_GetTick(void);
void ev1527_ResetTick(void);

void ev1527_Init(IRQn_Type irq, uint16_t pin , GPIO_TypeDef* gpio ,RfCallbackFunc_t fun){
	Ev1527_Utillities.ExternalInterrupt =irq;
	Ev1527_Utillities.myGpioPin = pin;
	Ev1527_Utillities.myGpioPort = gpio;
	Ev1527_Utillities.myCallBack = fun;
}


void ev1527_IsrRoutine(uint16_t GPIO_Pin){
	static uint64_t t1 = 0;
	static uint64_t t2 = 0;
	static uint64_t t3 = 0;
	static uint8_t phase = 1;
	static bool preambleDetected = false;
	static uint8_t index =0;
	static float r;

	if (NewDataRecieved) return;
    if(GPIO_Pin == RfInput_Pin) // If The INT Source Is EXTI Line9 (A9 Pin)
    {
    	if (HAL_GPIO_ReadPin(Ev1527_Utillities.myGpioPort, Ev1527_Utillities.myGpioPin)== GPIO_PIN_SET){
    		// rising
    		if (phase ==1){
    			ev1527_ResetTick();
    			t1 = ev1527_GetTick();
    			phase = 2;
    		} else if (phase ==3) {
    			t3 = ev1527_GetTick();
    			phase =2;
    			if (t1 !=t2){
    				 r= ((float)t3 - (float)t2)/((float)t2-(float)t1);
    			}

    			if (r >28&& r<32 && !preambleDetected){
    				r=0;
    				preambleDetected =true;

    			}
    			if (preambleDetected ){
    				data[index] = r;
    				index++;
    				if (index == 25 ){
    					ev1527_Disable();
    					preambleDetected=false;
    					index =0;
    					NewDataRecieved =true;
    				}
    			}
    			//t1 = t3;
    			ev1527_ResetTick();
    			t1 = ev1527_GetTick();
    		}
    		} else{
    		//falling
    		if (phase ==2){
    			phase =3;
    			t2= ev1527_GetTick();
    		}
    	}
    }
}

void ev1527_Process(void){
	if(NewDataRecieved){
		for (uint8_t i=1; i<25; i++){
			data[i] = RefineBit(data[i]);
		}
		if (validateData()){
			remoteCodeAndData =0;
			for (uint8_t i=1; i<25;i++){
				remoteCodeAndData += ((uint32_t)data[i]) << (24-i) ;
			}
			Ev1527_Utillities.myCallBack(remoteCodeAndData);
		}

		ev1527_Enable();
		NewDataRecieved = false;
	}

}
uint8_t RefineBit(float in){
	if (in<0.35f && in > 0.25f ){
		return 1;
	} else if (in<3.1f && in > 2.8f ){
		return 0;
	}	else {
		return 2;
	}
}

bool validateData(void){

	for (uint8_t i=1; i< 25; i++){
		if (data[i]==2){
			return false;
		}
	}
	return true;
}

void ev1527_Disable(void){
	 HAL_NVIC_DisableIRQ(Ev1527_Utillities.ExternalInterrupt);
}
void ev1527_Enable(void){
	HAL_NVIC_EnableIRQ(Ev1527_Utillities.ExternalInterrupt);
}
uint16_t ev1527_GetTick(void){
	return TIM2->CNT;
}
void ev1527_ResetTick(void){
	 TIM2->CNT = 0;
}
