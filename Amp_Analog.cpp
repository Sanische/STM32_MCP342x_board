#include "Amp_Analog.h"


Amp_Analog::Amp_Analog(I2C_HandleTypeDef *hi2c, PinType A0, PinType A1) : board(hi2c, (uint16_t)pin_addr[A0 * 3 + A1])
{
}

Amp_Analog::~Amp_Analog()
{
}

void Amp_Analog::configCH(CHANNELS ch, uint8_t typeSensor, RESOLUTION resolution, double lowValue, double highValue)
{
	this->lowValue[ch] = lowValue;
	this->highValue[ch] = highValue;
	this->typeSens[ch] = typeSensor;
	this->resCh[ch] = resolution;
}

void Amp_Analog::calibration(CHANNELS ch, double r1, double r2, double rsh)
{
	R1[ch] = r1;
	R2[ch] = r2;
	Rsh[ch] = rsh;
}

void Amp_Analog::zeroSet(int ch1, int ch2, int ch3, int ch4)
{
	zeroCal[0] = ch1;
	zeroCal[1] = ch2;
	zeroCal[2] = ch3;
	zeroCal[3] = ch4;
}

void Amp_Analog::zeroGet(int mass[])
{
	int temp;
	for (int i = 0; i < 4; i++)
	{
		board.setConfiguration((CHANNELS)i, resCh[i], CONTINUOUS_MODE, PGA_TO_TYPE[typeSens[i]]);
		for (int j = 0; j < 5; j++)
		{
			temp += board.measure();
		}
		mass[i] = temp / 5;
		temp = 0;
	}
}

void Amp_Analog::zeroCurrentSet(double ch1, double ch2, double ch3, double ch4)
{
	k_zeroCurrent[0] = ch1;
	k_zeroCurrent[1] = ch2;
	k_zeroCurrent[2] = ch3;
	k_zeroCurrent[3] = ch4;
}

double Amp_Analog::getValue(CHANNELS ch)
{
	double val = 0;
	switch (typeSens[ch])
	{
	case VOLTAGE_0_5:
	{ val = mapf(getVoltage(ch), 0.0, 5.0, lowValue[ch], highValue[ch]);
	return val; }
	case VOLTAGE_0_10:
	{ val = mapf(getVoltage(ch), 0.0, 10.0, lowValue[ch], highValue[ch]);
	return val; }
	case CURR_0_20:
	{ val = mapf(getCurrent(ch), 0.0, 20.0, lowValue[ch], highValue[ch]);
	return val; }
	case CURR_4_20:
	{
		double curr = getCurrent(ch);
		if (curr > 3.0)
		{
			val = mapf(curr, k_zeroCurrent[ch], 20.0, lowValue[ch], highValue[ch]);
			return val;
		}
		else
		{
			return -1.0;
		}
	}
	case CURR_0_40:
	{ val = mapf(getCurrent(ch), 0.0, 40.0, lowValue[ch], highValue[ch]);
	return val; }
	default:
	{ return 0; }
	};
}

double Amp_Analog::getVoltage(CHANNELS ch)
{
	double voltage = 0;
	voltage = ((double)getRAW(ch) / MAX_CODE_FROM_RES[resCh[ch]]) * (VREF / PGA_TO_CALC[typeSens[ch]]) * (1.0 / (R1[ch] / R2[ch]));
	return voltage;
}

double Amp_Analog::getCurrent(CHANNELS ch)
{
	double current = 0;
	current = (getVoltage(ch) / Rsh[ch])*1000.0;
	return current;
}

uint32_t Amp_Analog::getRAW(CHANNELS ch)
{
	int32_t temp;
	board.setConfiguration(ch, resCh[ch], CONTINUOUS_MODE, PGA_TO_TYPE[typeSens[ch]]);
	temp = board.measure() - zeroCal[ch];
	return temp > 0 ? temp : 0;
}


