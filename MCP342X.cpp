#include "MCP342X.h"

MCP342X::MCP342X(I2C_HandleTypeDef *hi2c, uint16_t address)
{
	_hi2c=hi2c;
	_address = 0b1101<<3;
	_address |= address&0b111;
	_address = (uint8_t)(_address<<1);
}

MCP342X::~MCP342X()
{

}



uint8_t MCP342X::getConfiguration()
{

	HAL_I2C_Master_Receive(_hi2c, _address, (uint8_t*)_buffer, _resolution==RESOLUTION_18_BITS?4:3, (uint32_t)100);

	//Wire.requestFrom(_address, _resolution==RESOLUTION_18_BITS?4:3);

	//uint8_t i=0;

	//while(Wire.available()) _buffer[i++] = Wire.read();

	return (_buffer[(_resolution==RESOLUTION_18_BITS?3:2)]);

}

void MCP342X::setConfiguration(CHANNELS channel,RESOLUTION resolution,MEASURE_MODE mode,PGA pga)
{

uint8_t cfgbyte=0;
_resolution=resolution;
_PGA=pga;
uint32_t divisor = 1;
_LSB = 2048000000/(divisor<<(resolutionConvert[_resolution]-1));

cfgbyte |= (channel & 0x3) << 5;
cfgbyte |= (mode & 0x1) << 4;
cfgbyte |= (resolution & 0x3) << 2;
cfgbyte |= pga & 0x3;

HAL_I2C_Master_Transmit(_hi2c, _address, &cfgbyte, 1, 100);

//Wire.beginTransmission(_address);
//Wire.write(cfgbyte);
//Wire.endTransmission();

}

void MCP342X::newConversion()
{

	uint8_t cfgbyte=getConfiguration();
	cfgbyte|=128;
	HAL_I2C_Master_Transmit(_hi2c, _address, &cfgbyte, 1, 100);
	//Wire.beginTransmission(_address);
	//Wire.write(cfgbyte|=128);
	//Wire.endTransmission();

}

bool MCP342X::isConversionFinished()
{

	uint8_t result= !(getConfiguration() & 0b10000000);

	return result;

}

void MCP342X::getRawDatas(uint8_t buffer[4])
{
	for(int i=0;i<4;i++) buffer[i]=_buffer[i];
}

int32_t MCP342X::measure()
{

	union resultUnion
	{
		uint8_t asBytes[4];
		int32_t asLong;
	} result;

	while(isConversionFinished()==0);

	if(_resolution==RESOLUTION_18_BITS)
	{
		result.asBytes[3] = _buffer[0]&0x80?0xFF:0x00;
		result.asBytes[2] = _buffer[0];
		result.asBytes[1] = _buffer[1];
		result.asBytes[0] = _buffer[2];

	} else
	{
		result.asBytes[3] = _buffer[0]&0x80?0xFF:0x00;
		result.asBytes[2] = _buffer[0]&0x80?0xFF:0x00;
		result.asBytes[1] = _buffer[0];
		result.asBytes[0] = _buffer[1];

	}

	//	return (result.asLong * _LSB / PGAConvert[_PGA]);
		return (result.asLong);

}

