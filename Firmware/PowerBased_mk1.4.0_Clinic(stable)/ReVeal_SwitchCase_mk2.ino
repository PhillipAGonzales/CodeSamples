
/*
	Written By: Phillip A. Gonzales
    Name:       ReVeal_Power_Mk2.ino
    Created:	8/26/2019 9:51:42 AM
	Revised:	12/12/2019 3:08:26 PM

	Gathering data to check the power threshold of the motor vs tactile stimuli
*/

/********************************************Initializing Library's Variables********************************************/
#include "Arduino.h"
#include "RxMk2.h"
#include "Adafruit_NeoPixel.h"
#include "InputsByPhillyG.h"
#include "PhillyGsautoDriver.h"

#define ChipSelect SDCARD_SS_PIN
#define Rtype 0						//Type of randomization:  0 = Psudo Random, 1 = Truly Random
#define Atype 0						//Type of file appendage: 0 = None, 1 = Commas
RxMk2 rxFile(Rtype,Atype);

#define AD_CS_PIN	6				//Dev: 42, PCB:	6
#define AD_RST_PIN	19				//Dev: 45, PCB: 19
#define SW_PIN		13				//Dev: 9,  PCB: 12
PhillyGsAutoDriver autoDriver(0,AD_CS_PIN,AD_RST_PIN);

#define TYPE 3		// 1:Direction w/Buttons, 2:Power w/Buttons, 3:Directions/Power/Both w/Thumb-stick
#define RedBtn 10
#define BlueBtn 9
#define SelectBtn 21
#define XOUT A14
#define YOUT A15
InputsByPhillyG inputs(RedBtn,BlueBtn,SelectBtn,XOUT,YOUT,TYPE); //Init the use of buttons

#define NPIX 16						//Dev: 31, Use Case: 16	
#define NumOfNPIX	8				//Dev: 1
#define NPIX_Delay	500				//Dev: 500
#define BRIGHTNESS	255/16				//Dev: 128,	Use Case: 255
#define VIS_DELAY	600				//Dev: 250,	Use Case: 500
#define BLINK_DELAY	250				//Dev: 30,	Use Case: 250
#define BLIP		100				//Dev: 50,	Use Case: 100
#define WHITE		10				//Dev: 0,	Use Case: 10
Adafruit_NeoPixel strip(NumOfNPIX, NPIX, NEO_GRBW + NEO_KHZ800);
//Handful of colors
uint32_t red = 0;
uint32_t green = 0;
uint32_t blue = 0;
uint32_t orange = 0;
uint32_t purple = 0;
uint32_t yellow = 0;

/*******************************************Initializing Main Loop Variables********************************************/
#define BACKWARDS 0
#define FORWARDS 1
#define CORRECT 2
#define INCORRECT 3
#define NADA 4

#define NUMOFTRIALS 9		//Make sure to update the RxMk File
#define NUMOFREPEATS 1		//Make sure to update the RxMk File
#define RESPONSETIME 950	//Dev: 20,	Use Case: 800
#define TDELAY 700			//Dev: 30,	Use Case: 300
#define COMPLETETIME 500	//Dev: 50,	Use Case: 500
#define SPINTIME 850		//Dev: 1000,Use Case: 850 
//Pins that are unaccounted for
#define STEPCLK 17
#define ERR_FLAG 20

//uint16_t tStickVal = 0;
uint8_t var = 0;
uint8_t sdError = 150;
uint16_t sFlag = 1;
uint16_t _xValue;
uint16_t totalSIZEndx = 0;
uint8_t repeatNDX = 0;
uint8_t sizeNDX = 0;
uint16_t power;
uint8_t _dT_repeat;
uint8_t posCounter = 0;
uint8_t negCounter = 0;
uint8_t notCounter = 0;

/***********************************************************************************************************************/
/*********************************************Begining Main Loop********************************************************/
/***********************************************************************************************************************/
void setup()
{
	//Serial.begin(9600);	
	//Init NeoPixels
		strip.begin();
		strip.show();
		strip.setBrightness(BRIGHTNESS);
	
		//Init up all the colors 
		blue	= strip.Color(0	 ,  0,255,WHITE);
		yellow	= strip.Color(255,255,  0,WHITE);
		red		= strip.Color(255,	0,  0,WHITE);
		green	= strip.Color(0	 ,255,  0,WHITE);
		orange	= strip.Color(255,136,  0,WHITE);
		purple	= strip.Color(93 ,	0,255,WHITE);
	
	
	//Init SD COMMS	
		if(!SD.begin(ChipSelect))
		{
			//_sdFailed = true;
			while(1)
			{
				blinkYell();
			}
		}
	
	//Init SPI COMMs
		SPI.begin();
		SPI.setDataMode(SPI_MODE3);
	
	//Default configuration for autoDriver
		L6470Config();
		resetAD();
		pinMode(STEPCLK,OUTPUT);
		digitalWrite(STEPCLK,LOW);
		pinMode(ERR_FLAG,INPUT);
		//delay(VIS_DELAY);

	//Generate the Direction and Power(Speed) Parameters 
		rxFile.genDirectionStimParameters();
		//blinkYell();
		rxFile.genPowerStimParameters();
		//blinkPurp();
		
		//rainbow();
		//delay(VIS_DELAY);	
}

void loop()
{
	sFlag = inputs.currentSelVal();
	
	switch (sFlag)
	{
		case 0://Init count down sequence
				strip.clear();           //Clears any COLOR settings
				
				//Init 3 2 1 sequence
				solidRed(VIS_DELAY);
				
				solidYellow(VIS_DELAY);

				solidGreen(VIS_DELAY);
				
				sFlag = 2;//Push to main
			break;
		
		case 1://Wait for tStick press to init the session
				while(sFlag == 1)
				{
					sFlag = inputs.currentSelVal();
					solidPurp(BLIP);
				}
			break;
			
		case 2://Main Loop
				for(repeatNDX = 0; repeatNDX < NUMOFREPEATS; repeatNDX++)
				{
					for(sizeNDX = 0; sizeNDX < NUMOFTRIALS; sizeNDX++)
					{
						power = rxFile.ps_Array[sizeNDX];
						_dT_repeat = rxFile.dT_repeat[sizeNDX][repeatNDX];
						delay(TDELAY *2)
						
						//push the direction value from directionArray in element i to the motor
						if(_dT_repeat == BACKWARDS)		autoDriver.timeRun(REV,power,SPINTIME,LS);								
						if(_dT_repeat == FORWARDS)		autoDriver.timeRun(FWD,power,SPINTIME,LS);
						
						awaitUserResponse();
						analogDebouncing(totalSIZEndx);
						lockInUserResponse();
						
						inputs.tStickCheck1 = inputs.checkCurrentYval(YOUT);
						if(inputs.tStickCheck1 == inputs.Y_NEG)
						{
							saveToSD();
							sFlag = 1;//Quit
						}
					}
				}
			break;
				
	}
}
/*********************************************************************************************************************/
/*********************************************End of Main Loop********************************************************/
/*********************************************************************************************************************/

/****************************************Functions Used by Main Loop**************************************************/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Motor Config/Commands~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void L6470Config()
{														/*General Use Default Values*/
	autoDriver.SPIportConnect(&SPI);					
	autoDriver.configSyncPin(BUSY_PIN,0);				//BUSY_PIN,0
	autoDriver.configStepMode(STEP_FS_128);				//STEP_FS_128
	autoDriver.setMaxSpeed(100000);						//100000
	autoDriver.setFullSpeed(100000);					//100000
	autoDriver.setAccel(100000);						//100000
	autoDriver.setSlewRate(SR_530V_us);					//SR_180V_us
	autoDriver.setOverCurrentThreshold(OC_2250mA);		//OC_2250mA
	autoDriver.setOverCurrentShutdown(OC_SD_DISABLE);	//OC_SD_DISABLE
	autoDriver.setVoltageComp(VS_COMP_DISABLE);			//VS_COMP_DISABLE
	autoDriver.setPWMfreq(PWM_DIV_2,PWM_MUL_2);			//PWM_DIV_2,PWM_MUL_2
	autoDriver.setSwitchMode(SW_HARD_STOP);				//SW_USER
	autoDriver.setOsciMode(EXT_16MHZ_OSCOUT_INVERT);	//EXT_16MHZ_OSCOUt_INVERT
	autoDriver.setAccelKval(128);						//128
	autoDriver.setDecelKval(128);						//128
	autoDriver.setRunKval(128);							//128
	autoDriver.setHoldKval(128);						//128
}

void resetAD()
{
	digitalWrite(AD_RST_PIN,LOW);
	delay(100);
	digitalWrite(AD_RST_PIN,HIGH);
}

void awaitUserResponse()
{
	inputs.tStickCheck1 = inputs.checkCurrentXval(XOUT);
	if(inputs.tStickCheck1 == inputs.X_NOT)
	{
		delay(TDELAY);
		inputs.tStickCheck2 = inputs.checkCurrentXval(XOUT);
		while(inputs.tStickCheck1 == inputs.tStickCheck2)
		{
			solidBlue(BLIP);
			inputs.tStickCheck2 = inputs.checkCurrentXval(XOUT);
		}
	}
}

void analogDebouncing(uint16_t totalSIZEndx)
{
	uint16_t _totalSIZEndx = totalSIZEndx;//Protecting the original value
	
	for(uint16_t i = 0; i < 20; i++)
	{
		_xValue = inputs.currentXval(XOUT);
		inputs.tStickVal[i] = inputs.checkXval(_xValue);	//Polls the tStick 5 times and stores values in array
		delay(50);
		
		if (i == 19)									//Once array is full sort which value is the users input
		{											//Analog "De-bouncing"
			//run through the tStickVal array and save the mode response
			uint8_t posCounter = 0;
			uint8_t negCounter = 0;
			uint8_t notCounter = 0;
			for( uint8_t i = 0; i < 20; i++)			//Runs through the filled array and counts the number of times...
			{										//each value is stored
				if(inputs.tStickVal[i] == inputs.X_POS) posCounter++;
				if(inputs.tStickVal[i] == inputs.X_NEG) negCounter++;
				if(inputs.tStickVal[i] == inputs.X_NOT) notCounter++;
				
				if(i == 19)							//Finds the counter w/ the highest value
				{									//Once the highest value is found it writes it as the users input
					if (posCounter > negCounter)
					{
						if (posCounter > notCounter)
						{
							inputs.tStickUserInput = inputs.X_POS;
							rxFile.userInput[_totalSIZEndx] = FORWARDS;
						}
						else
						{
							inputs.tStickUserInput = inputs.X_NOT;
							rxFile.userInput[_totalSIZEndx] = NADA;
						}
					}
					else
					{
						if(negCounter > notCounter)
						{
							inputs.tStickUserInput = inputs.X_NEG;
							rxFile.userInput[_totalSIZEndx] = BACKWARDS;
						}
						else
						
						{
							inputs.tStickUserInput = inputs.X_NOT;
							rxFile.userInput[_totalSIZEndx] = NADA;
						}
					}
				}
			}
		}
	}
}

void lockInUserResponse()
{
	inputs.selButtonCheck1 = inputs.currentSelVal();
	while(inputs.selButtonCheck1)
	{
		inputs.selButtonCheck1 = inputs.currentSelVal();
		solidGreen(BLIP);
		inputs.selButtonCheck1 = inputs.currentSelVal();
	}
}

void saveToSD()
{
	rxFile.logUserResponse();
	delay(COMPLETETIME);
	autoDriver.hardHiZ();
	rainbow();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Color Codes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void solidBlue(uint16_t _onTime)
{
	strip.clear();
	strip.setBrightness(BRIGHTNESS);
	strip.fill(blue,0,8);
	strip.show();
	delay(_onTime);
	nPIXoff();
	inputs.reset();
}

void solidYellow(uint16_t _onTime)
{
	strip.clear();
	strip.setBrightness(BRIGHTNESS);
	strip.fill(yellow,0,8);
	strip.show();
	delay(_onTime);
	nPIXoff();
	inputs.reset();
}

void solidRed(uint16_t _onTime)
{
	strip.clear();
	strip.setBrightness(BRIGHTNESS);
	strip.fill(red,0,8);
	strip.show();
	delay(_onTime);
	nPIXoff();
	inputs.reset();
}

void solidGreen(uint16_t _onTime)
{
	strip.clear();
	strip.setBrightness(BRIGHTNESS);
	strip.fill(green,0,8);
	strip.show();
	delay(_onTime);
	nPIXoff();
	inputs.reset();
}

void solidOrange(uint16_t _onTime)
{
	strip.clear();
	strip.setBrightness(BRIGHTNESS);
	strip.fill(orange,0,8);
	strip.show();
	delay(_onTime);
	nPIXoff();
	inputs.reset();
}

void solidPurp(uint16_t _onTime)
{
	strip.clear();
	strip.setBrightness(BRIGHTNESS);
	strip.fill(purple,0,8);
	strip.show();
	delay(_onTime);
	nPIXoff();
	inputs.reset();
}

void rainbow()
{
	strip.setBrightness(BRIGHTNESS);
	//Display Rainbow to show the device is awake
	for (uint8_t i = 0; i <= 3; i++)
	{
		strip.clear();
		strip.fill(red,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.fill(orange,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.fill(yellow,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		strip.fill(blue,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		strip.fill(purple,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
	}
	inputs.reset();
}

void blinkBlue()
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		strip.fill(blue,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		delay(BLINK_DELAY);
	}
	inputs.reset();
}

void blinkYell()
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		strip.fill(yellow,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		delay(BLINK_DELAY);
	}
	inputs.reset();
}

void blinkRed()
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		strip.fill(red,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		delay(BLINK_DELAY);
	}
	inputs.reset();
}

void blinkGreen()
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		strip.fill(green,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		delay(BLINK_DELAY);
	}
	inputs.reset();
}

void blinkOrg()
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		strip.fill(orange,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		delay(BLINK_DELAY);
	}
	inputs.reset();
}

void blinkPurp()
{
	for (uint8_t i = 0; i <= 2; i++)
	{
		strip.fill(purple,0,8);
		strip.show();
		delay(BLINK_DELAY);
		strip.clear();
		strip.show();
		delay(BLINK_DELAY);
	}
}

void nPIXoff()
{
	strip.clear();
	strip.show();
}