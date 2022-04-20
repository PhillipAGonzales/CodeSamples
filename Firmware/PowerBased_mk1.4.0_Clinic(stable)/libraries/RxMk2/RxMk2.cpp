/*
	RxMk2.cpp:	Reads in prescription file (.csv) and stores in array
	Written By: Phillip A. Gonzales, December, 12th, 2019
	Revised:	##/##/20##
*/

#include "Arduino.h"
#include "RxMk2.h"
#include "SD.h"
#include "SPI.h"

RxMk2::RxMk2(uint8_t randType, uint8_t appendType)
{
	_randType = randType;
	_appendType = appendType;
	
	if(_randType)
	{
		randomSeed(analogRead(3));
	}	
}

void RxMk2::genPowerStimParameters()
 {
	 _eosNDX = 0;
	 _psArrayNDX = 0;
	 _diffInPower = MAXSPEED - MINSPEED;
	 _speedRange = NUMOFTRIALS - 1;
	 
	 //Mainly for dev purposes, If the power level is the same then use that power level to test the dataAquasion file structure
	 if(_diffInPower <= 0)
		{
			_newPower = MAXSPEED;
			for(_singlePowerNDX = 0; _singlePowerNDX < NUMOFTRIALS; _singlePowerNDX ++) 
			{
				dataString = "";
				dataString += String(_newPower);
				ps_Array[_sPArrayNDX] = _newPower;
				
				//Open the file
				File dataFile = SD.open("PwrVal.txt", FILE_WRITE);
				
				//If the file is available then wright to it
				if (dataFile)
				{
					if(_sPeosNDX == _speedRange - 1)
					{
						dataString += "\n";
						dataString += String(_eos);			//Add the number to the string
					}
					dataFile.println(dataString);			//Write the appended number to the SD card
					dataFile.close();
				}
				_sPeosNDX++;
				_sPArrayNDX++;
			}
		}
		
	  else//Standard protocol: Resume your regular programming 
	  {
			for(psNDX = 0; psNDX <= _speedRange; psNDX++)//Generate array from 1 -> x
			{
				digitalWrite(LED_BUILTIN, HIGH);
				dataString = "";
				dataString += String((psNDX * SPD_STEP_SIZE) + MINSPEED);
				ps_Array[_psArrayNDX] = ((psNDX * SPD_STEP_SIZE) + MINSPEED);
			
				//Open the file		
				File dataFile = SD.open("PwrVal.txt", FILE_WRITE);
			
				//If the file is available then wright to it						 
				if (dataFile)
				{
					if(_eosNDX == _speedRange)
					{
						dataString += "\n";
						dataString += String(_eos);			//Add the number to the string
					}
					dataFile.println(dataString);			//Write the appended number to the SD card
					dataFile.close();
				}	
				_eosNDX++;
				_psArrayNDX++;
				digitalWrite(LED_BUILTIN,LOW);
			}												//Array is in order from 1 -> x

			//Array is still in order
			const size_t n = sizeof(ps_Array) / sizeof(ps_Array[1]);

			for(size_t i = 1; i < n - 1; i++)
			{
				//randomSeed(analogRead(3));
				size_t j = random(1, n - i);

				uint16_t t = ps_Array[i];
				ps_Array[i] = ps_Array[j];
				ps_Array[j] = t;
			}
				//Array is now jumbled while retaining all the original numbers from 1 -> x
		
		  _eosNDX = 1;
		  _psArrayNDX = 0;
			for(psNDX = 0; psNDX <= _speedRange ; psNDX++)
			{
				digitalWrite(LED_BUILTIN, HIGH);
				dataString = "";
				dataString += String(ps_Array[_psArrayNDX]);
			
				//Open the file
				File dataFile = SD.open("xPwrVal.txt", FILE_WRITE);
			
				//If the file is available then wright to it
				if (dataFile)
				{
					if(_eosNDX == _speedRange + 1)
					{
						dataString += "\n";
						dataString += String(_eos);			//Add the random number to the string
					}
					dataFile.println(dataString);			//Write the appended random number to the SD card
					dataFile.close();
				}
				_eosNDX++;
				_psArrayNDX++;
				digitalWrite(LED_BUILTIN, LOW);
			}
		}
}

void RxMk2::genDirectionStimParameters()
{
	//Generate a "random" set of direction parameters 0 -> 4
	for (i = 0; i < NUMOFTRIALS; i++)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		dataString = "";						//Make a string for assembling the data to log:
		randNumber = random(0,5);				//Pick a random number from 0 - 4
		dataString += String(randNumber);		//Add the random number to the string
		
		if(_appendType == 1)
		{
			dataString += ",";					//Append the string with a comma
		}
		
		dTableArray[i] = randNumber;			//Write randNumber to array
				
		//Open the file
		File dataFile = SD.open("DTVals.txt", FILE_WRITE);
		
		//If the file is available then wright to it
		if (dataFile)
		{
			if(i == (NUMOFTRIALS - 1))
			{
				dataString += "\n";
				dataString += String(_eos);		//Add the random number to the string
			}
			dataFile.println(dataString);		//Write the appended random number to the SD card
			dataFile.close();
		}
		digitalWrite(LED_BUILTIN, LOW);
	}
//////"Random" Number Set Generation Complete//////////////////////////////////////////////////////////////////////////////////////////////////////	
	for(_rptNDX = 0; _rptNDX < NUMOFREPEATS; _rptNDX++)
	{
		for(i = 0; i < NUMOFTRIALS; i++)
		{
			digitalWrite(LED_BUILTIN, HIGH);
			switch (dTableArray[i])
			{
				case 0:
					if(_rptNDX == 0)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 1)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 2)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 3)
						{
						dT_repeat[i][_rptNDX] = 1;
						}	
				break;
					
				case 1:
					if(_rptNDX == 0)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 1)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 2)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 3)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
				break;
				
				case 2:	
					if(_rptNDX == 0)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 1)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 2)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 3)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
				break;
				
				case 3:
					if(_rptNDX == 0)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 1)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 2)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 3)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
				break;
				
				case 4:
					if(_rptNDX == 0)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 1)
						{
						dT_repeat[i][_rptNDX] = 1;
						}
					if(_rptNDX == 2)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
					if(_rptNDX == 3)
						{
						dT_repeat[i][_rptNDX] = 0;
						}
				break;
			}
			
			dataString = "";
			_tempVal = dT_repeat[i][_rptNDX];
			dataString += String(_tempVal);
			
			if(_appendType == 1)
			{
				dataString += ",";
			}
			
			File dataFile = SD.open("DVals.txt",FILE_WRITE);
			
			if (dataFile)
			{			
				if(_rptNDX == (NUMOFREPEATS - 1) && i == (NUMOFTRIALS - 1 ))
				{
					dataString += "\n";
					dataString += String(_eos);
				}
				
				dataFile.println(dataString);
				dataFile.close();
				digitalWrite(LED_BUILTIN, LOW);	
			}	
		}
	}
}

void RxMk2::logUserResponse()
{
	for(p = 0; p <= totalSIZE; p++)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		dataString = "";
		_userResponse = userInput[p];
		dataString += _userResponse;
		
		File dataFile = SD.open("UserIn.txt", FILE_WRITE);
		
		if(dataFile)
		{
			if(p == totalSIZE)
			{
				dataString = String(_eos);	//Add the random number to the string
			}
			dataFile.println(dataString);
			dataFile.close();
		}
		digitalWrite(LED_BUILTIN, LOW);
	}	
}
