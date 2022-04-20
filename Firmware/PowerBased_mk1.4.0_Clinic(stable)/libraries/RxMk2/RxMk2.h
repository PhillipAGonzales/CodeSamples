/*
	RxMk2.h:	Reads in prescription file (.csv) and stores in array
	Written By: Phillip A. Gonzales, December, 12th, 2019
	Revised:	##/##/20##
*/

#ifndef RxMk2_h
#define RxMk2_h
#include "Arduino.h"
#include "SPI.h"
#include "SD.h"

#define MAXSPEED	  400
#define MINSPEED	  0
#define SPD_STEP_SIZE 50
#define NUMOFTRIALS	  9		//Make sure to update the .ino file, Should be = [(MAXPOWER - MINPOWER) / SPD_STEP_SIZE] + 1
#define NUMOFREPEATS  5		//Make sure to update the .ino file, Use Case: 2
#define TOTALTRIALS   45		//Should be = NUMBEROFTRIALS * NUMOFREPEATS, Use Case: > 150 

class RxMk2
{
	public:
		RxMk2(uint8_t randType = 0, uint8_t appendType = 0);
		uint16_t totalSIZE = TOTALTRIALS;
		uint16_t dTableArray[NUMOFTRIALS];	
		uint16_t	dT_repeat[NUMOFTRIALS][NUMOFREPEATS];
		uint16_t	userInput[TOTALTRIALS];

		uint16_t ps_Array[NUMOFTRIALS];
			
		long randNumber;
		String dataString;
		
		void genPowerStimParameters();
		void genDirectionStimParameters();
		void logUserResponse();
		
	private:		
		//Direction Table
/////////////////////////////////////////	
	uint8_t dTArray0[4] = {0,0,1,1};
	uint8_t dTArray1[4] = {0,1,0,1};
	uint8_t dTArray2[4] = {1,0,0,1};
	uint8_t dTArray3[4] = {1,0,1,0};
	uint8_t dTArray4[4] = {1,1,0,0};
/////////////////////////////////////////
	
		uint16_t _userResponse = 0;
		uint8_t _randType;
		uint8_t _appendType;
		uint16_t i;
		uint16_t p;
		uint16_t _responseNDX = 0;
		uint8_t _responsetSR;
		uint8_t _singlePowerNDX;
		int16_t _speedRange;

		uint16_t psNDX;
		uint16_t _rptNDX;
		uint16_t _tempVal;
		uint16_t _eosNDX;
		uint16_t _sPeosNDX;
		uint16_t _psArrayNDX;
		uint16_t _sPArrayNDX;
		uint16_t _diffInPower;
		uint16_t _newPower;
		
			
		bool _sdFailed = 0;
		bool _sdRead = 0;
		bool _fileError = 0;
		String _eos = "End of Session";
		String _bos = "Beginning of session\n";
};

#endif