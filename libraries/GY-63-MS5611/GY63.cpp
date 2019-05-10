#include "GY63.h"
#include <Wire.h>

void GY63::MS561101BA_RESET()
{
	Wire.beginTransmission(MS561101BA_SlaveAddress);
	Wire.write(MS561101BA_RST);
	GY63::error = Wire.endTransmission();
}
void GY63::MS561101BA_PROM_READ()
{
	unsigned char d1, d2, i;
	for (i = 0; i <= 6; i++)
	{
		Wire.beginTransmission(MS561101BA_SlaveAddress);
		Wire.write((MS561101BA_PROM_RD + i * 2));
		Wire.endTransmission();

		Wire.requestFrom(MS561101BA_SlaveAddress, 2);
		d1 = Wire.read();
		d2 = Wire.read(); 
		delay(5);

		GY63::Cal_C[i] = ((unsigned char)d1 << 8) | d2;
	}
}

unsigned long GY63::MS561101BA_DO_CONVERSION(unsigned char command)
{
	unsigned long conversion = 0;
	unsigned long conv1, conv2, conv3;
	Wire.beginTransmission(MS561101BA_SlaveAddress);
	Wire.write(command);
	Wire.endTransmission();

	delay(100);

	Wire.beginTransmission(MS561101BA_SlaveAddress);
	Wire.write(0);
	Wire.endTransmission();

	Wire.requestFrom(MS561101BA_SlaveAddress, 3);
	conv1 = Wire.read();
	conv2 = Wire.read();
	conv3 = Wire.read();
	conversion = conv1 * 65535 + conv2 * 256 + conv3;

	return conversion;
}


float GY63::getTemperature(unsigned char OSR_Temp)
{

	GY63::D2_Temp = GY63::MS561101BA_DO_CONVERSION(OSR_Temp);
	delay(10);
	GY63::dT = GY63::D2_Temp - (((unsigned long)GY63::Cal_C[5]) << 8);
	GY63::TEMP = 2000 + GY63::dT*((unsigned long)GY63::Cal_C[6]) / 8388608;
	if(GY63::TEMP>6000.0||GY63::TEMP<-5000.0){
	    GY63::MS561101BA_RESET();
	   delay(1000);
	   GY63::MS561101BA_PROM_READ();
	   delay(1000);
	    return 0.0;
    }
	return GY63::TEMP / 100;
}

float GY63::getPressure(unsigned char OSR_Pres)
{

	GY63::D1_Pres = GY63::MS561101BA_DO_CONVERSION(OSR_Pres);
	delay(10);
	GY63::OFF_ = (unsigned long)GY63::Cal_C[2] * 65536 + ((unsigned long)GY63::Cal_C[4] * GY63::dT) / 128;
	GY63::SENS = (unsigned long)GY63::Cal_C[1] * 32768 + ((unsigned long)GY63::Cal_C[3] * GY63::dT) / 256;

	if (GY63::TEMP<2000)
	{
		// second order temperature compensation when under 20 degrees C
		GY63::T2 = (GY63::dT*GY63::dT) / 0x80000000;
		GY63::Aux = GY63::TEMP*GY63::TEMP;
		GY63::OFF2 = 2.5*GY63::Aux;
		GY63::SENS2 = 1.25*GY63::Aux;
		GY63::TEMP = GY63::TEMP - GY63::TEMP2;
		GY63::OFF_ = GY63::OFF_ - GY63::OFF2;
		GY63::SENS = GY63::SENS - GY63::SENS2;
	}

	GY63::Pressure = (GY63::D1_Pres*GY63::SENS / 2097152 - GY63::OFF_) / 32768;
	if(GY63::Pressure>130000.0||GY63::Pressure<80000.0){
	   GY63::MS561101BA_RESET();
	   delay(1000);
	   GY63::MS561101BA_PROM_READ();
	   delay(1000);
        return 0.0;
    }
	return GY63::Pressure / 100;
}

void GY63::init(int CSB)
{
	pinMode(CSB, OUTPUT);
	digitalWrite(CSB, LOW);
	Wire.begin();
	delay(20);

	GY63::MS561101BA_RESET();
	delay(1000);
	GY63::MS561101BA_PROM_READ();
	delay(1000);
}
