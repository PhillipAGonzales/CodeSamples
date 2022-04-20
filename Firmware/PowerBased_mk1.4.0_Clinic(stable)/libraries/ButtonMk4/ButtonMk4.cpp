/* 
	ButtonMk4.cpp for reading in any sensory input on the MetroM4
	Created by: Phillip A. Gonzales, June 17, 2019
	Revised: 8/26/2019
*/

#include "ButtonMk4.h"
#include "Arduino.h"

volatile uint8_t ButtonMk4::FLAG;

ButtonMk4::ButtonMk4(uint8_t RedBtn, uint8_t BlueBtn, uint8_t type)	//Setup for the uController
{
	FLAG = ZILCH;
	
	pinMode(RedBtn, INPUT_PULLDOWN);
	pinMode(BlueBtn, INPUT_PULLDOWN);
	_type = type;

	REG_EIC_CTRLA =  EIC_CTRLA_CKSEL;
	REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(PIN10);
	REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(PIN10);
	
	REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(PIN9);
	REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(PIN9);
	
	if (_type == 2)
	{
		attachInterrupt(RedBtn, f_buttonPressed, RISING);
		attachInterrupt(BlueBtn, b_buttonPressed, RISING);		
	}

	if (_type == 3)
	{
		attachInterrupt(RedBtn, motor_detected, RISING);
		attachInterrupt(BlueBtn, motor_not_detected, RISING);
	}
}

//----------------------------------------------USED IN ISR's----------------------------------------------//
void ButtonMk4::f_buttonPressed()
{
	FLAG = F_BUTTON_PRESSED;
}

void ButtonMk4::b_buttonPressed()
{
	FLAG = B_BUTTON_PRESSED;
}

void ButtonMk4::motor_detected()
{
	FLAG = M_DETECTED;
}

void ButtonMk4::motor_not_detected()
{
	FLAG = M_NOT_DETECTED;
}

void ButtonMk4::reset()
{
	FLAG = ZILCH;
}
//----------------------------------------------General Use----------------------------------------------//
void ButtonMk4::startTimer()
{
	t0 = millis();
}

uint8_t ButtonMk4::endTimer()
{
	t1 = millis();
	totalTime = t1 - t0;
	return totalTime;
}

