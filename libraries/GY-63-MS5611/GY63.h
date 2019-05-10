#ifndef GY63_H
#define GY63_H
#if ARDUINO < 100
	#include <WProgram.h>
#else
	#include <Arduino.h>
#endif

#define MS561101BA_SlaveAddress   (0xee>>1)   //定义器件在IIC总线中的从地址 

#define MS561101BA_D1      0x40
#define MS561101BA_D2      0x50
#define MS561101BA_RST     0x1E

#define MS561101BA_D1_OSR_256    0x40
#define MS561101BA_D1_OSR_512    0x42
#define MS561101BA_D1_OSR_1024   0x44
#define MS561101BA_D1_OSR_2048   0x46
#define MS561101BA_D1_OSR_4096   0x48

#define MS561101BA_D2_OSR_256    0x50
#define MS561101BA_D2_OSR_512    0x52
#define MS561101BA_D2_OSR_1024   0x54
#define MS561101BA_D2_OSR_2048   0x56
#define MS561101BA_D2_OSR_4096   0x58

#define MS561101BA_ADC_RD     0x00
#define MS561101BA_PROM_RD    0xA0
#define MS561101BA_PROM_CRC   0xAE

class GY63 {
	public:
		int error;
		float  getTemperature(unsigned char OSR_Temp);
		float  getPressure(unsigned char OSR_Pres);
		void  init(int CSB);
	private:
		void  MS561101BA_RESET();
		void  MS561101BA_PROM_READ();
		unsigned long MS561101BA_DO_CONVERSION(unsigned char command);
		
		unsigned int Cal_C[7];          //用于存放PROM中的8组数据
		unsigned long D1_Pres, D2_Temp;  // 存放压力和温度
		float dT, TEMP=0, T2;
		double OFF_, SENS;
		float Pressure=0;       //大气压
		float TEMP2, Aux, OFF2, SENS2; //温度校验值
};
#endif