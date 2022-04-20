/* 
	ButtonMk4.h for reading in any sensory input on the MetroM4
	Created by: Phillip A. Gonzales, June 17, 2019
	Revised: 8/26/2019
*/

#ifndef ButtonMk4_h
#define ButtonMk4_h

#include "Arduino.h"
#define PIN9 0x200
#define PIN10 0x400

class ButtonMk4
{
	public: 
		//Constructor: Init Button1's input pin
		ButtonMk4 (uint8_t fBtn = 10, uint8_t bBtn = 9, uint8_t type = 0);

		//----------------------------------------------USED IN ISR's----------------------------------------------//
		//Init bit-wise variables						
		static volatile uint8_t FLAG;
		static const uint8_t ZILCH = 0x00;				// 0000 0000
		static const uint8_t F_BUTTON_PRESSED = 0x01;	// 0000 0001
		static const uint8_t B_BUTTON_PRESSED = 0x02;	// 0000 0010
		static const uint8_t M_DETECTED = 0x04;			// 0000 0100
		static const uint8_t M_NOT_DETECTED = 0x08;		// 0000 1000
		
		//Init functions
		static void f_buttonPressed();
		static void b_buttonPressed();
		static void motor_detected();
		static void motor_not_detected();
		void reset();
		
		//----------------------------------------------General Use----------------------------------------------//
		//Init general use functions		
		void startTimer();
		uint8_t endTimer();
		
		//Init general use variables
		
		uint8_t t0 = 0;
		uint8_t t1 = 0;
		uint8_t totalTime = 0;
		
		private:
		uint8_t _type = 0;
};
#endif