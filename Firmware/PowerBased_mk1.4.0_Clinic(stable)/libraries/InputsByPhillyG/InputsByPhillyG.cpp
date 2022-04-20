/* 
	InputsByPhillyG.cpp for reading in any sensory input on the MetroM4
	Created by: Phillip A. Gonzales, September 3rd, 2019
	Revised:	1/24/2020 
*/

#include "InputsByPhillyG.h"
#include "Arduino.h"

volatile uint8_t InputsByPhillyG::FLAG;

InputsByPhillyG::InputsByPhillyG(uint8_t redBtn = 10, uint8_t blueBtn = 9, uint8_t selBtn = 8, uint8_t xout = A0, uint8_t yout = A1,  uint8_t type = 3)	//Setup for the uController
{
	REG_EIC_CTRLA =  EIC_CTRLA_CKSEL;
	
	FLAG = ZILCH;
	_type = type;
	_xout = xout;
	_yout = yout;
	selectButton = selBtn;
	
	switch (_type)
	{
		case 1: //Direction w/Buttons
			pinMode(redBtn, INPUT_PULLDOWN);
			pinMode(blueBtn, INPUT_PULLDOWN);
			
			REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(PIN10);
			REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(PIN10);
			
			REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(PIN9);
			REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(PIN9);
			
			
			attachInterrupt(redBtn, f_buttonPressed, RISING);
			attachInterrupt(blueBtn, b_buttonPressed, RISING);
		break;
			
		case 2:	//Power w/Buttons
			pinMode(redBtn, INPUT_PULLDOWN);
			pinMode(blueBtn, INPUT_PULLDOWN);
			
			REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(PIN10);
			REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(PIN10);
			
			REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(PIN9);
			REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(PIN9);
			
			attachInterrupt(redBtn, motor_detected, RISING);
			attachInterrupt(blueBtn, motor_not_detected, RISING);
		break;
		
		case 3:	//Direction/Power/Both w/Thumb-stick
			xVal = currentXval(_xout);
			yVal = currentYval(_yout);
			pinMode(selectButton,INPUT_PULLUP);
			
			REG_EIC_ASYNCH = EIC_ASYNCH_ASYNCH(SL_PIN);
			REG_EIC_DEBOUNCEN = EIC_DEBOUNCEN_DEBOUNCEN(SL_PIN);				
		break;
	}
}

//----------------------------------------------USED IN ISR's----------------------------------------------//
void InputsByPhillyG::f_buttonPressed()
{
	FLAG = F_BUTTON_PRESSED;
}

void InputsByPhillyG::b_buttonPressed()
{
	FLAG = B_BUTTON_PRESSED;
}

void InputsByPhillyG::motor_detected()
{
	FLAG = M_DETECTED;
}

void InputsByPhillyG::motor_not_detected()
{
	FLAG = M_NOT_DETECTED;
}

void InputsByPhillyG::sel_buttonPressed()
{
	FLAG = SEL_BUTTON_PRESSED;
}

void InputsByPhillyG::x_pos()
{
	FLAG = X_POS;
}

void InputsByPhillyG::y_pos()
{
	FLAG = Y_POS;
}

void InputsByPhillyG::y_neg()
{
	FLAG = Y_NEG;
}
void InputsByPhillyG::reset()
{
	FLAG = ZILCH;
	FLAG2 = ZILCH;
}
//----------------------------------------------General Use----------------------------------------------//
uint16_t InputsByPhillyG::currentXval(uint8_t xout = A0)
{	
	newXval = analogRead(xout);
	return newXval;
}

uint16_t InputsByPhillyG::checkXval(uint16_t newXval)
{
		if(newXval > _xMinHigh)
		{
			FLAG2 = X_POS;
			return FLAG2;
		}
		
		if(newXval < _xMaxLow)
		{
			FLAG2 = X_NEG;
			return FLAG2;
		}
		
		if(newXval < _xMinHigh && newXval >= _xMaxLow)
		{
			FLAG2 = X_NOT;
			return FLAG2;
		}
}

uint16_t InputsByPhillyG::checkCurrentXval(uint8_t xout = A0)
{
	uint16_t newXval = analogRead(xout);
	if(newXval > _xMinHigh)
	{
		FLAG2 = X_POS;
		return FLAG2;
	}
	
	if(newXval < _xMaxLow)
	{
		FLAG2 = X_NEG;
		return FLAG2;
	}
	
	if(newXval < _xMinHigh && newXval >= _xMaxLow)
	{
		FLAG2 = X_NOT;
		return FLAG2;
	}
}

										
uint16_t InputsByPhillyG::currentYval(uint8_t yout = A1)
{
	newYval = analogRead(yout);
	return newYval;
}

uint16_t InputsByPhillyG::checkYval(uint16_t newYval)
{
	if(newYval > _yMinHigh)
	{
		FLAG2 = Y_POS;
		return FLAG2;
	}
	
	if(newYval < _yMaxLow)
	{
		FLAG2 = Y_NEG;
		return FLAG2;
	}
	
	if(newYval < _yMinHigh && newYval >= _yMaxLow)
	{
		FLAG2 = Y_NOT;
		return FLAG2;
	}
}

uint16_t InputsByPhillyG::checkCurrentYval(uint8_t yout = A1)
{
	uint16_t newYval = analogRead(yout);
	if(newYval > _yMinHigh)
	{
		FLAG2 = Y_POS;
		return FLAG2;
	}
	
	if(newYval < _yMaxLow)
	{
		FLAG2 = Y_NEG;
		return FLAG2;
	}
	
	if(newYval < _yMinHigh && newYval >= _yMaxLow)
	{
		FLAG2 = Y_NOT;
		return FLAG2;
	}
}

bool InputsByPhillyG::currentSelVal()
{
	_selButton = selectButton;
	newSval = digitalRead(_selButton);
	return newSval;
}

void InputsByPhillyG::startTimer()
{
	t0 = millis();
}

uint8_t InputsByPhillyG::endTimer()
{
	t1 = millis();
	totalTime = t1 - t0;
	return totalTime;
}
