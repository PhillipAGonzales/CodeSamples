/* 
	PhillyGsAutoDriver.cpp for initializing the SparkFunAutoDriver stepper motor driver
	Created by: Phillip A. Gonzales, Dec 5, 2019
	Revised: 12/11/2019 @ 3:39 pm
*/

#include "PhillyGsAutoDriver.h"
#include "SPI.h"
#include "Arduino.h"

int PhillyGsAutoDriver::_numBoards;

PhillyGsAutoDriver::PhillyGsAutoDriver(int boardPosition, int CSpin, int resetPin, int busyPin)
{
	_boardPosition = boardPosition;
	_CSpin = CSpin;
	_resetPin = resetPin;
	_busyPin = busyPin;
	_numBoards++;
	_SPI = &SPI;
	
	//Init SPI port & pins
	pinMode(_busyPin, OUTPUT);
	digitalWrite(_CSpin, HIGH);
	
	//Reset the board to put all pins at a known state
	pinMode(_resetPin, OUTPUT);
	digitalWrite(_resetPin,LOW);
	digitalWrite(_resetPin,HIGH);
}
PhillyGsAutoDriver::PhillyGsAutoDriver(int boardPosition, int CSpin, int resetPin)
{
	_boardPosition = boardPosition;
	_CSpin = CSpin;
	_resetPin = resetPin;
	_busyPin = -1;
	_numBoards++;
	_SPI = &SPI;
	
	//Init SPI port & pins
	pinMode(_CSpin, OUTPUT);
	digitalWrite(_CSpin, HIGH);
	
	//Reset the board to put all pins at a known state
	pinMode(_resetPin, OUTPUT);
	digitalWrite(_resetPin,LOW);
	digitalWrite(_resetPin,HIGH);	
}

void PhillyGsAutoDriver::SPIportConnect(SPIClass *SPIPort)
{
	_SPI = SPIPort;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Registry Read/Write Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
long PhillyGsAutoDriver::parameterHandler(byte parameter, unsigned long value)
{
	long returnTemp = 0;
	
	switch (parameter)
	{
		/*	ABS_POS:	 Current distance from HOME
		 				 22 bit #, expressed in 2's complement
		 				 = 0, @ boot
						 Only written when motor is NOT moving	*/		
		case ABS_POS:
				returnTemp = xferParameters(value, 22);
			break;	
		/*	EL_POS:		 Electrical position in the step generation cycle
		 				 22 bit #, expressed in 2's complement
		 				 = 0, @ boot
						 Only written when motor is NOT moving	*/	
		case EL_POS:
				returnTemp = xferParameters(value, 9);
			break;				
		/*	MARK:        2nd position != 0 that the motor can be driven to
		 		         22 bit #, expressed in 2's complement
		 		         = 0, @ boot	*/	
		case MARK:
			returnTemp = xferParameters(value,22);
			break;
		/*	SPEED:       Read-ONLY	*/
		case SPEED:
			returnTemp = xferParameters(0,20);
			break;
		/*	ACCEL/DECEL: Set the acceleration/deceleration rates
						 Both = 0x08A, @ boot up
						 ***NOTE***
						 AccelCalc()/DecelCalc() converts steps/sec^2 -> 12-bit values for L6470	*/
		case ACCEL:
			returnTemp = xferParameters(value,12);
			break;
		case DECEL:
			returnTemp = xferParameters(value,12);	
		/*	MAX_SPEED:	 Set the max speed allowed by the motor
						 = 0x041, @ boot
						 ***NOTE***
						 MaxSpeedCalc() converts steps/sec -> 10-bit values for L6470	*/
		case MAX_SPEED:
			returnTemp = xferParameters(value,10);
			break;
		/*	MIN_SPEED:	 Activates the Low-Speed Optimization feature
						 Sets the lowest speed allowed by the motor
						 LSPD_OPT = 13th bit, once set the min allowed speed is = 0
						 = 0, @ boot
						 ***NOTE***
						 MaxSpeedCalc() converts steps/sec -> 10-bit value for L6470	*/
		case MIN_SPEED:
			returnTemp = xferParameters(value,13);
			break;
		/*	FS_SPD:	     Sets the threshold which forces uStepping -> full-stepping
						 = 0x027, @ boot
						 ***NOTE***
						 FSCalc() converts steps/sec -> 10-bit values for L6470	*/
		case FS_SPD:
			returnTemp = xferParameters(value,10);
			break;
		/*	KVAL's:		 Sets the maximum voltage of the PWM outputs
						 8-bit ratiometric values, 100%:255, 50%:128
						 = 0x29, @ boot	*/
		case KVAL_HOLD:
			returnTemp = xferParameters(value,8);
			break;
		case KVAL_RUN:
			returnTemp = xferParameters(value,8);
			break;
		case KVAL_ACCEL:
			returnTemp = xferParameters(value,8);
			break;
		case KVAL_DECEL:
			returnTemp = xferParameters(value,8);
			break;
		/*	INT_SPD, ST_SLP, FN_SLP & FN_SLP_DEC: Set EMF compensation functionality (Leave @ defaults)	*/
		case INT_SPD:
			returnTemp = xferParameters(value,14);
			break;
		case ST_SLP:
			returnTemp = xferParameters(value,8);
			break;
		case FN_SLP_ACC:
			returnTemp = xferParameters(value,8);
			break;
		case FN_SLP_DEC:
			returnTemp = xferParameters(value,8);			
			break;
		/*	K_THERM:	 Sets the motor winding thermal drift compensation (Leave @ defaults)	*/
		case K_THERM:
			value &= 0x0F;
			returnTemp = xferParameters(value,8);
			break;
		/*	ADC_OUT:	 Read-ONLY	*/
		case ADC_OUT:
			returnTemp = xferParameters(value,8);
			break;
		/*	OCD_TH:		 Sets the OverCurrent threshold
						 375mA > OCD_TH > 6A, in steps of 375mA
						 4-bit value
						 = 3.375mA/0x08, @ boot	*/
		case OCD_TH:
			value &= 0x0F;
			returnTemp = xferParameters(value,8);
			break;
		/*	STALL_TH:	 Sets the stall current threshold
						 31.25mA > STALL_TH > 4A, in steps of 31.25mA
						 7-bit value
						 = 2.03A/0x40, @ boot	*/
		case STALL_TH: 
			value &= 0x7F;
			returnTemp = xferParameters(value,8);
			break;
		/*	STEP_MODE:	 Sets the uStepping mode, generation of L6470 output signal
						 Bits 2 -> 0: controls # of uSteps/Step
						 Bit	   7: controls BUSY/SYNC pin outputs BUSY or step sync signal 
						 Bits 6 -> 4: controls freq of output signal relative to full-step freq	*/
		case STEP_MODE:
			returnTemp = xferParameters(value,8);
			break;
		/*	ALARM_EN:	 Sets which alarms will cause the FLAG pin to fall to LOW
						 ALL alarms will trigger FLAG pin @ boot	*/
		case ALARM_EN:
			returnTemp = xferParameters(value,8);
			break;
		/*	CONFIG:		 = 0x2E88, @ boot
						 Is set by user using previous functions	*/
		case CONFIG:
			returnTemp = xferParameters(value,16);
			break;
		/* STATUS:		 Returns the current status of all registries in the L6470
						 Read-ONLY	*/
		case STATUS:
			returnTemp = xferParameters(0,16);
			break;
		default:
			SPIXfer((byte)value);
			break;
	}
	return returnTemp;
}

long PhillyGsAutoDriver::xferParameters(unsigned long value, byte bitLength)
{
	byte byteLength = bitLength/8;
	if (bitLength % 8 > 0) byteLength++;
	
	byte tempVal;
	unsigned long returnTemp = 0;
	
	for (int i = 0; i < byteLength; i++)
	{
		returnTemp = returnTemp << 8;
		tempVal = SPIXfer((byte)(value >> ((byteLength-i-1)*8)));
		returnTemp |= tempVal;
	}
	
	unsigned long mask = 0xFFFFFFFF >> (32-bitLength);
	return returnTemp & mask;
}	

byte PhillyGsAutoDriver::SPIXfer(byte data)
{
	byte dataNugget[_numBoards];
	for(int i = 0; i < _numBoards; i++)
	{
		dataNugget[i] = 0;
	}
	
	dataNugget[_boardPosition] = data;
	digitalWrite(_CSpin, LOW);
	_SPI ->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
	_SPI->transfer(dataNugget, _numBoards);
	_SPI->endTransaction();
	digitalWrite(_CSpin, HIGH);
	return dataNugget[_boardPosition];
}				

//Writes to various registers
void PhillyGsAutoDriver::setParam(byte param, unsigned long value)
{
	param |= SET_PARAM;
	SPIXfer((byte)param);
	parameterHandler(param, value);
}

//Retrieves various register values
long PhillyGsAutoDriver::getParam(byte param)
{
	SPIXfer(param | GET_PARAM);
	return parameterHandler(param,0);
}

int PhillyGsAutoDriver::busyCheck(void)
{
	if(_busyPin == -1)
	{
		if(getParam(STATUS)&0x0002) return 0;
		else return 1;
	}
	else
	{
		if(digitalRead(_busyPin)==HIGH) return 0;
		else return 1;
	}
}

////////////////////////////////////////////////////////Config the AutoDriver////////////////////////////////////////////////////////

//************************************Setting the values************************************//

//Init the SYNC/BUSY pin to be either SYNC/BUSY, and set the ticks per step 
void PhillyGsAutoDriver::configSyncPin(byte pinFunction, byte syncSteps)
{
	//Clearing register
	byte syncPinConfig = (byte)getParam(STEP_MODE);
	syncPinConfig &= 0x0f;
	
	//Masking data
	syncPinConfig |= ((pinFunction & 0x80) | (syncSteps & 0x70));
	
	//Write to dSPIN
	setParam(STEP_MODE, (unsigned long)syncPinConfig);
}

//Changing the uStepping mode
void PhillyGsAutoDriver::configStepMode(byte stepMode)
{
	//Clear & set register
	byte stepModeConfig = (byte)getParam(STEP_MODE);
	stepModeConfig &= 0xF8;
	
	//Making data
	stepModeConfig |= (stepMode &= 0x07);
	
	//Write to dSPIN
	setParam(STEP_MODE, (unsigned long)stepModeConfig);
}

//Set the maximum speed the motor will produce 
void PhillyGsAutoDriver::setMaxSpeed(float stepsPerSecond)
{
	//Convert the human number -> L6470
	unsigned long integerSpeed = maxSpeedCalc(stepsPerSecond);
	
	//Set the register to calculated speed
	setParam(MAX_SPEED, integerSpeed);
}

//Set the minimum speed the motor will produce
void PhillyGsAutoDriver::setMinSpeed(float stepsPerSecond)
{
	//Convert the human number -> L6470
	unsigned long integerSpeed = minSpeedCalc(stepsPerSecond);
	
	//Set the register to calculated speed
	setParam(MIN_SPEED, integerSpeed);
}

//Set the threshold to init uStepping -> full-Stepping
void PhillyGsAutoDriver::setFullSpeed(float stepsPerSecond)
{
	unsigned long integerSpeed = FSCalc(stepsPerSecond);
	setParam(FS_SPD, integerSpeed);
}

//Set rate of acceleration(steps/sec2), from human value -> L6470 value
//***NOTE*** ANY value > 29802 will disable acceleration -> "Infinite" acceleration mode
void PhillyGsAutoDriver::setAccel(float stepsPerSecond2)
{
	unsigned long integerAccel = accelCalc(stepsPerSecond2);
	setParam(ACCEL, integerAccel);
}

//Set rate of deceleration(step/sec2), from human value -> L6470 value
//See setAccel() ***NOTE***
void PhillyGsAutoDriver::setDecel(float stepsPerSec2)
{
	unsigned long integerDecel = decelCalc(stepsPerSec2);
	setParam(DECEL, integerDecel);
}

//Set the over current threshold
void PhillyGsAutoDriver::setOverCurrentThreshold(byte threshold)
{
	setParam(OCD_TH, 0x0F & threshold);
}

//Setting the PWM freq when uStepping
//	Divisors: 1 - 7
//	Multipliers: 0.625 - 2
void PhillyGsAutoDriver::setPWMfreq(int divisor, int multiplier)
{
	unsigned long configVal = getParam(CONFIG);
	//Masking variables
	configVal &= ~(0xE000);	//15:13, local of divisor in reg
	configVal &= ~(0x1C00);	//12:10, local of multiplier in reg
	configVal |= ((0xE000 & divisor)|(0x1C00 & multiplier));
	
	//Set the register to the masked value
	setParam(CONFIG, configVal);
}

//Setting the slew rate -> 180, 290, or 530
void PhillyGsAutoDriver::setSlewRate(int slewRate)
{
	unsigned long configVal = getParam(CONFIG);
	//Masking variables
	configVal &= ~(0x0300);	//9:8, local of the slewRate in reg
	configVal |= (0x0300 & slewRate);
	//Set the register to the masked value
	setParam(CONFIG, configVal);
}

//Enable board shutdown if Over Current is detected
void PhillyGsAutoDriver::setOverCurrentShutdown(byte OCshutdown)
{
	unsigned long configVal = getParam(CONFIG);
	//Masking variables
	configVal &= ~(0x0080);	//7, local of the OCshutdown in reg
	configVal |= (0x0080 & OCshutdown);
	//Set the register to the masked value
	setParam(CONFIG, configVal);
}

//Enabling motor voltage compensation
void PhillyGsAutoDriver::setVoltageComp(int vCompMode)
{
	unsigned long configVal = getParam(CONFIG);
	configVal &= ~(0x0020);	//5, local of the vComp in reg
	configVal |= (0x0020 & vCompMode);
	//Set the register to the masked value
	setParam(CONFIG, configVal);
}

//Setting the Switch Input
void PhillyGsAutoDriver::setSwitchMode(int switchMode)
{
	unsigned long configVal = getParam(CONFIG);
	//Masking variables
	configVal &= ~(0x0010);	//4, local of the SW in reg
	configVal |= (0x0010 & switchMode);
	//Set the register to the masked value
	setParam(CONFIG, configVal);
}

//Setting the clock source, this will be used for calculations as well
void PhillyGsAutoDriver::setOsciMode(int ocsillatorMode)
{
	unsigned long configVal = getParam(CONFIG);
	//Masking variables
	configVal &= ~(0x00F);	//3:0, local of the OSC mode in reg
	configVal |= (0x00F & ocsillatorMode);
	//Set the register to the masked value
	setParam(CONFIG, configVal);
}

//Enable/Disable Low-Speed Optimization 
//	Enable: 0 -> MaxSpeed, decreases to MinSpeed smoother
//	Disable: MinSpeed -> MaxSpeed
void PhillyGsAutoDriver::setLowSpeedOpt(bool enable)
{
	unsigned long temp = getParam(MIN_SPEED);
	if (enable) temp |= 0x00001000;	//Set LSPD_OPT
	else		temp &= 0xffffefff;	//Clear LSPD_OPT
	setParam(MIN_SPEED, temp);
}

//Setting all K Values
void PhillyGsAutoDriver::setAccelKval(byte kvalInput)
{
	setParam(KVAL_ACCEL, kvalInput);
}
void PhillyGsAutoDriver::setDecelKval(byte kvalInput)
{
	setParam(KVAL_DECEL, kvalInput);
}
void PhillyGsAutoDriver::setRunKval(byte kvalInput)
{
	setParam(KVAL_RUN, kvalInput);
}
void PhillyGsAutoDriver::setHoldKval(byte kvalInput)
{
	setParam(KVAL_HOLD, kvalInput);
}

//************************************Checking the values************************************//
//Returns the number of steps the motor has traveled from the HOME position, 
//	HOME = zeroing this register(init as zero @ startup)
long PhillyGsAutoDriver::getPos()
{
	long temp = getParam(ABS_POS);
	//Account for 2's complement to maintain proper sign of value
	if(temp & 0x00200000) temp |= 0xffc00000;
	return temp;
}

//Returns the value in the STATUS register
//	Resets all warnings, if you do NOT want to reset warnings us GetPara(STATUS)
int PhillyGsAutoDriver::getStatus()
{
	int temp = 0;
	byte* bytePointer = (byte*) & temp;
	SPIXfer(GET_STATUS);
	bytePointer[1] = SPIXfer(0);
	bytePointer[0] = SPIXfer(0);
	return temp;
}

//See getPos() notes
long PhillyGsAutoDriver::getMark()
{
	long temp = getParam(MARK);
	//Account for 2's complement to maintain proper sign of value
	if (temp & 0x00200000) temp |= 0xffC00000;
	return temp;
}

//Ping for current uStepping mode
byte PhillyGsAutoDriver::getStepMode()
{
	return (byte)(getParam(STEP_MODE) & 0x07);
}

//Ping for current max speed setting
float PhillyGsAutoDriver::getMaxSpeed()
{
	return maxSpeedParse(getParam(MAX_SPEED));
}

//Ping for current min speed setting
float PhillyGsAutoDriver::getMinSpeed()
{
	return minSpeedParse(getParam(MIN_SPEED));
}

//Ping for current full speed setting
float PhillyGsAutoDriver::getFullSpeed()
{
	return FSParse(getParam(FS_SPD));
}

//Ping for current Acceleration Setting
float PhillyGsAutoDriver::getAccel()
{
	return accelParse(getParam(ACCEL));
}

//Ping for current Deceleration Setting
float PhillyGsAutoDriver::getDecel()
{
	return decelParse(getParam(DECEL));
}

//Ping for the current Over Current threshold
byte PhillyGsAutoDriver::getOverCurrentThreshold()
{
	return (byte)(getParam(OCD_TH) & 0xF);
}

//Ping for the current PWM Divisor
int PhillyGsAutoDriver::getPWMfreqDivisor()
{
	return (int)(getParam(CONFIG) & 0xE000);
}

//Ping for the current PWM Multiplier
int PhillyGsAutoDriver::getPWMfreqMulitplier()
{
	return (int)(getParam(CONFIG) & 0x1C00);
}

//PIng for the current SlewRate
int PhillyGsAutoDriver::getSlewRate()
{
	return (int)(getParam(CONFIG) & 0x0300);
}

//Ping for the current state of the OCshutdown flag
int PhillyGsAutoDriver::getOverCurrentShutdown()
{
	return (int)(getParam(CONFIG) & 0x0080);
}

//Ping for the current state of the VolatgeComp reg
int PhillyGsAutoDriver::getVoltageComp()
{
	return (int)(getParam(CONFIG) & 0x0020);
}

//Ping for the current state of the Switch Mode
int PhillyGsAutoDriver::getSwitchMode()
{
	return (int)(getParam(CONFIG) &0x0010);
}

//Ping for the current clock option
int PhillyGsAutoDriver::getOsciMode()
{
	return (int)(getParam(CONFIG) & 0x000F);
}

//Ping for the state of the Low-Speed Optimization flag
bool PhillyGsAutoDriver::getLowSpeedOpt()
{
	return (bool) ((getParam(MIN_SPEED) & 0x00001000) != 0);
}

//Ping for the K values
byte PhillyGsAutoDriver::getAccelKval()
{
	return (byte)getParam(KVAL_ACCEL);
}
byte PhillyGsAutoDriver::getDecelKval()
{
	return (byte)getParam(KVAL_DECEL);
}
byte PhillyGsAutoDriver::getRunKval()
{
	return (byte)getParam(KVAL_RUN);
}
byte PhillyGsAutoDriver::getHoldKval()
{
	return (byte)getParam(KVAL_HOLD);
}

////////////////////////////////////////////////////////AutoDriver Command Functions////////////////////////////////////////////////////////
//Sets motor in motion based on user defined values
//	MAX_SPEED & MIN_SPEED registers
//	if FS_SPD is exceeded motor will go from uStepping to Full-Stepping
void PhillyGsAutoDriver::run(byte direction, float stepsPerSec)
{
	SPIXfer(RUN | direction);
	unsigned long integerSpeed = speedCalc(stepsPerSec);
	if (integerSpeed > 0xFFFFF) integerSpeed = 0xFFFFF;
	
	//Organize the byte-order for the L6470
	byte* bytePointer = (byte*) & integerSpeed;
	for (int8_t i = 2; i >= 0; i--)
	{
		SPIXfer(bytePointer[i]);
	}
}

//Sets the clock source to be external, STCK is now clock source
//	Will be used for RUN, MOVE, etc
void PhillyGsAutoDriver::stepClock(byte dir)
{
	SPIXfer(STEP_CLOCK | dir);
}

//Set the motor in motion(relative) by numStep in Full-Stepping mode based on user definitions
void PhillyGsAutoDriver::move(byte direction, unsigned long numSteps)
{
	SPIXfer(MOVE | direction);
	if (numSteps > 0x3FFFFF) numSteps = 0x3FFFFF;
	
	//Organize the byte-order for the L6470
	byte* bytePointer = (byte*) & numSteps;
	for (int8_t i = 2; i >= 0; i--)
	{
		SPIXfer(bytePointer[i]);
	}
}

//Absolute version of move()
void PhillyGsAutoDriver::goTo(long pos)
{
	SPIXfer(GOTO);
	if (pos > 0x3FFFFF) pos = 0x3FFFFF;
	
	//Organize the byte-order for the L6470
	byte* bytePointer = (byte*) & pos;
	for (int8_t i = 2; i >= 0; i--)
	{
		SPIXfer(bytePointer[i]);
	}
}

//Constrained version of goTo()
void PhillyGsAutoDriver::goToDir(byte direction, long pos)
{
	SPIXfer(GOTO_DIR | direction);
	if (pos > 0x3FFFFF) pos = 0x3FFFFF;
	
	//Organize the byte-order for the L6470
	byte* bytePointer = (byte*)&pos;
	for (int8_t i = 2; i >= 0; i--)
	{
		SPIXfer(bytePointer[i]);
	}
}

//Sets the motor in motion based on user definitions until the SW pin detects a falling edge
//	The type of stop (soft/hard) is set in setSwitchMode()
void PhillyGsAutoDriver::goUntil(byte action, byte dir, float stepsPerSec)
{
	SPIXfer(GO_UNTIL | action | dir);
	unsigned long integerSpeed = speedCalc(stepsPerSec);
	if (integerSpeed > 0x3FFFFF) integerSpeed = 0x3FFFFF;
	
	//Organize the byte-order for the L6470
	byte* bytePointer = (byte*)&integerSpeed;
	for (int8_t i = 2; i >= 0; i--)
	{
		SPIXfer(bytePointer[i]);
	}
}

//Sets the motor in motion at MIN_SPEED or 5 steps/sec (whichever is higher) until the SW pin detects a rising edge
//	The type of stop (soft/hard) is set in setSwitchMode()
void PhillyGsAutoDriver::releaseSw(byte action, byte direction)
{
	SPIXfer(RELEASE_SW | action | direction);
}

//Allow user to define MARK and ABS_POS
void PhillyGsAutoDriver::setMark(long newMark)
{
	setParam(MARK, newMark);
}

void PhillyGsAutoDriver::setPos(long newPos)
{
	setParam(ABS_POS, newPos);
}

//Home the motor, via the shortest path,
//	If direction is needed use GoTo_Dir() instead
void PhillyGsAutoDriver::goHome()
{
	SPIXfer(GO_HOME);
}

//Sets the motor in motion to this location via the shortest path
//	If direction is needed use GoTo_Dir() instead
void PhillyGsAutoDriver::goMark()
{
	SPIXfer(GO_MARK);
}

//Sets the ABS_POS to register 0
void PhillyGsAutoDriver::resetPos()
{
	SPIXfer(RESET_POS);
}

//Resets the device to power up conditions
void PhillyGsAutoDriver::resetDevice()
{
	SPIXfer(RESET_DEVICE);
}

//Bring the motor to a soft stop
void PhillyGsAutoDriver::softStop()
{
	SPIXfer(SOFT_STOP);
}

//Bring the motor to a hard stop
void PhillyGsAutoDriver::hardStop()
{
	SPIXfer(HARD_STOP);
}

//Bring the motor to a soft stop and hold
void PhillyGsAutoDriver::softHiZ()
{
	SPIXfer(SOFT_HIZ);
}

//Bring the motor to an immediate stop and hold
void PhillyGsAutoDriver::hardHiZ()
{
	SPIXfer(HARD_HIZ);
}

//Run the motor at a set direction speed and duration and stop with a SoftStop/HardStop
void PhillyGsAutoDriver::timeRun(byte direction, float speed, float duration, int stopType)
{
	run(direction,speed);
	delay(duration);
	if (stopType == LS) softStop();
	else				softHiZ();
}

void PhillyGsAutoDriver::stepRun(byte direction, float speed, float numOfSteps, int stopType)
{
	move(direction,numOfSteps);
	if (stopType == SS) softHiZ();
	else				hardHiZ();
}

////////////////////////////////////////////////////////AutoDriver Calculation Functions////////////////////////////////////////////////////////

//The value in the ACC register is [(steps/s2)*(tick^2)]/(2^-40), tick = 250ns = 0x08A on boot
unsigned long PhillyGsAutoDriver::accelCalc(float stepsPerSec2)
{
	float temp = stepsPerSec2 * 0.137438;
	if( (unsigned long) long(temp) > 0x00000FFF) return 0x00000FFF;
	else return (unsigned long) long(temp);
}
float PhillyGsAutoDriver::accelParse(unsigned long stepsPerSec2)
{
	return (float)(stepsPerSec2 & 0x00000FFF) / 0.137438;
}
unsigned long PhillyGsAutoDriver::decelCalc(float stepsPerSec2)
{
	float temp = stepsPerSec2 * 0.137438;
	if( (unsigned long) long(temp) > 0x00000FFF) return 0x00000FFF;
	else return (unsigned long) long(temp);
}float PhillyGsAutoDriver::decelParse(unsigned long stepsPerSec2)
{
	return (float)(stepsPerSec2 & 0x00000FFF) / 0.137438;
}

//The value in the MAX_SPD register is  [(steps/s)*(tick)]/(2^-18), tick = 250ns = 0x041 on boot
unsigned long PhillyGsAutoDriver::maxSpeedCalc(float stepsPerSec)
{
	unsigned long temp = ceil(stepsPerSec * 0.065536);
	if( temp > 0x000003FF) return 0x000003FF;
	else return temp;
}
float PhillyGsAutoDriver::maxSpeedParse(unsigned long stepsPerSec)
{
	return (float) (stepsPerSec & 0x000003FF) / 0.065536;
}

//The value in the MIN_SPD register is [(steps/s)*(tick)]/(2^-24), tick = 250ns = 0x000 on boot
unsigned long PhillyGsAutoDriver::minSpeedCalc(float stepsPerSec)
{
	float temp = stepsPerSec / 0.238;
	if( (unsigned long) long(temp) > 0x00000FFF) return 0x00000FFF;
	else return (unsigned long) long(temp);
}
float PhillyGsAutoDriver::minSpeedParse(unsigned long stepsPerSec)
{
	return (float) ((stepsPerSec & 0x00000FFF) * 0.238);
}

//The value in the FS_SPD register is ([(steps/s)*(tick)]/(2^-18))-0.5, tick = 250ns = 0x027 on boot
unsigned long PhillyGsAutoDriver::FSCalc(float stepsPerSec)
{
	float temp = (stepsPerSec * .065536)-.5;
	if( (unsigned long) long(temp) > 0x000003FF) return 0x000003FF;
	else return (unsigned long) long(temp);
}
float PhillyGsAutoDriver::FSParse(unsigned long stepsPerSec)
{
	return (((float) (stepsPerSec & 0x000003FF)) + 0.5) / 0.065536;
}

//The value in the INT_SPD register is [(steps/s)*(tick)]/(2^-24), tick = 250ns = 0x408 on boot
unsigned long PhillyGsAutoDriver::intSpeedCalc(float stepsPerSec)
{
	float temp = stepsPerSec * 4.1943;
	if( (unsigned long) long(temp) > 0x00003FFF) return 0x00003FFF;
	else return (unsigned long) long(temp);
}
float PhillyGsAutoDriver::intSpeedParse(unsigned long stepsPerSec)
{
	return (float) (stepsPerSec & 0x00003FFF) / 4.1943;
}

//The value in the INT_SPD register is [(steps/s)*(tick)]/(2^-28), tick = 250ns = 0x408 on boot
unsigned long PhillyGsAutoDriver::speedCalc(float stepsPerSec)
{
	unsigned long temp = stepsPerSec * 67.106;
	if( temp > 0x000FFFFF) return 0x000FFFFF;
	else return temp;
}
float PhillyGsAutoDriver::speedParse(unsigned long stepsPerSec)
{
	return (float) (stepsPerSec & 0x000FFFFF) / 67.106;
}