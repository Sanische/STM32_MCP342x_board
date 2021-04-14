#pragma once

#include "MCP342X.h"
#include "stm32f4xx_hal.h"

enum MEASURE_MODE
{
	ONE_SHOT_MODE=0,
	CONTINUOUS_MODE
};

enum RESOLUTION
{
	RESOLUTION_12_BITS=0,
	RESOLUTION_14_BITS,
	RESOLUTION_16_BITS,
	RESOLUTION_18_BITS
};

enum CHANNELS
{
	CH1=0,
	CH2,
	CH3,
	CH4
};

enum PGA
{
	PGA_X1=0,
	PGA_X2,
	PGA_X4,
	PGA_X8
};

const uint8_t resolutionConvert[] = {12,14,16,18};
const uint8_t PGAConvert[] = {1,2,4,8};

class MCP342X
{
public:

MCP342X(I2C_HandleTypeDef *hi2c, uint16_t address);
~MCP342X();
void setConfiguration(CHANNELS channel,RESOLUTION resolution,MEASURE_MODE mode,PGA pga);
void newConversion();
bool isConversionFinished();
int32_t measure();
void getRawDatas(uint8_t buffer[4]);
uint8_t getConfiguration();

private:
I2C_HandleTypeDef * _hi2c;  // I2C handle
uint16_t _address;
RESOLUTION _resolution;
MEASURE_MODE _mode;
PGA _PGA;
int32_t _LSB;
uint8_t _buffer[4];
};
