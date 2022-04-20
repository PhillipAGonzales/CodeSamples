/* 
	InputsByPhillyG.h for reading in any sensory input on the MetroM4
	Created by: Phillip A. Gonzales, September 3rd, 2019
	Revised:	12/12/2019 @ 5:28 PM
*/

#ifndef InputsByPhillyG_h
#define InputsByPhillyG_h

#include "Arduino.h"
#define PIN8  0x100
#define PIN9  0x200
#define PIN10 0x400
#define SL_PIN 0x200000
#define ZERO 0
#define POSx 1
#define NEGx 2
#define POSy 3
#define NEGy 4

class InputsByPhillyG
{
	public:
	//Constructor: Init Button1's input pin
	InputsByPhillyG (uint8_t fBtn, uint8_t bBtn, uint8_t sBtn, uint8_t xout, uint8_t yout, uint8_t type);

	//----------------------------------------------USED IN ISR's----------------------------------------------//
	//Init bit-wise variables
	static volatile uint8_t FLAG;
	static const uint8_t ZILCH = 0x00;				// 0000 0000
	static const uint8_t F_BUTTON_PRESSED = 0x01;	// 0000 0001
	static const uint8_t B_BUTTON_PRESSED = 0x02;	// 0000 0010
	static const uint8_t M_DETECTED = 0x04;			// 0000 0100
	static const uint8_t M_NOT_DETECTED = 0x08;		// 0000 1000
	static const uint8_t SEL_BUTTON_PRESSED = 0x10;	// 0001 0000
	static const uint8_t X_POS = 0x20;				// 0010 0000
	static const uint8_t X_NEG = 0x40;				// 0100 0000
	static const uint8_t Y_POS = 0x80;				// 1000 0000
	static const uint8_t Y_NEG = 0x03;				// 0000 0011
	static const uint8_t X_NOT = 0x04;				// 0000 0100
	static const uint8_t Y_NOT = 0x05;				// 0000 0101
	
	//Init functions
	static void f_buttonPressed();
	static void b_buttonPressed();
	static void motor_detected();
	static void motor_not_detected();
	static void sel_buttonPressed();
	static void x_pos();
	static void x_neg();
	static void y_pos();
	static void y_neg();
	
	void reset();
	
	//----------------------------------------------General Use----------------------------------------------//
	//Init general use functions
	void startTimer();
	uint8_t endTimer();
	
	bool currentSelVal();
	uint16_t checkXval(uint16_t);
	uint16_t checkYval(uint16_t);
	uint16_t currentXval(uint8_t xout);
	uint16_t currentYval(uint8_t yout);
	uint16_t checkCurrentXval(uint8_t xout);
	uint16_t checkCurrentYval(uint8_t xout);
		
	//Init general use variables
	uint8_t FLAG2;	
	uint8_t t0 = 0;
	uint8_t t1 = 0;
	uint8_t totalTime = 0;
	uint8_t tStickUserInput = 0;
	uint16_t tStickVal[20];
	uint8_t tStickCheck1;
	uint8_t tStickCheck2;
	bool selButtonCheck1;
	bool selButtonCheck2;

	uint16_t xVal = 0;
	uint16_t yVal = 0;
	uint16_t newXval = 0;
	uint16_t newYval = 0;
	uint16_t newSval = 0;
	uint16_t selectButton;
	
	private:
	uint8_t _type = 0;
	uint8_t _direction = 0;

	uint16_t _i;
	uint16_t _element;
	
	uint8_t _xout;
	uint16_t _xVal = 0;
	uint16_t _xMaxHigh = 1023;
	uint16_t _xMinHigh = 525;
	uint16_t _xMaxLow = 500;
	uint16_t _xMinLow = 4;
	uint16_t _selButton; 
	
	uint8_t _yout;
	uint16_t _yVal = 0;
	uint16_t _yMaxHigh = 1023;
	uint16_t _yMinHigh = 530;
	uint16_t _yMaxLow =505; 
	uint16_t _yMinLow = 4;
};
#endif

