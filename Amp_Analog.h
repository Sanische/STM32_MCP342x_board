#pragma once

#include "MCP342X.h"

enum TYPE_SENSOR
{
	VOLTAGE_0_5 = 0,
	VOLTAGE_0_10,
	CURR_0_20,
	CURR_4_20,
	CURR_0_40
};

typedef enum { PIN_LOW, PIN_HIGH, PIN_FLOAT } PinType;

const double MAX_CODE_FROM_RES[] = { 2047.0, 8191.0, 32767.0, 131071.0 };
const PGA PGA_TO_TYPE[] = { PGA_X2, PGA_X1, PGA_X2, PGA_X2, PGA_X1 };
const double PGA_TO_CALC[] = {2.0, 1.0, 2.0, 2.0, 1.0};
const double VREF = 2.048;

const uint8_t pin_addr[] = {
	0b1101000, 0b1101010, 0b1101001,
	0b1101100, 0b1101110, 0b1101101,
	0b1101011, 0b1101111, 0b1101000 };

class Amp_Analog
{
public:
	Amp_Analog(I2C_HandleTypeDef *hi2c, PinType A0, PinType A1);
	~Amp_Analog();

	void configCH(CHANNELS ch, uint8_t typeSensor, RESOLUTION resolution, double lowValue, double highValue);
	void calibration(CHANNELS ch , double r1, double r2, double rsh = 249);
	void zeroSet(int ch1, int ch2, int ch3, int ch4);
	void zeroGet(int mass[]);
	void zeroCurrentSet(double ch1, double ch2, double ch3, double ch4);
	double getValue(CHANNELS ch); //double physic value
	double getVoltage(CHANNELS ch); //double Volt
	double getCurrent(CHANNELS ch); //return current mA
	uint32_t getRAW(CHANNELS ch); //long RAW
private:
	MCP342X board;
	double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
		return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}
	double R1[4] = { 33000.0, 33000.0, 33000.0, 33000.0 }; //div oamp
	double R2[4] = { 180000.0, 180000.0, 180000.0, 180000.0 }; //div oamp
	double Rsh[4] = { 249.0, 249.0, 249.0, 249.0 };//shunt current

	double lowValue[4] = { 0,0,0,0 };
	double highValue[4] = { 0,0,0,0 };
	double k_zeroCurrent[4] = { 4.0, 4.0, 4.0, 4.0 };
	uint8_t pga;
	RESOLUTION resCh[4] = { RESOLUTION_12_BITS,RESOLUTION_12_BITS,RESOLUTION_12_BITS,RESOLUTION_12_BITS };
	uint8_t typeSens[4] = { 0,0,0,0 };
	int16_t zeroCal[4] = { 0,0,0,0 };
};

