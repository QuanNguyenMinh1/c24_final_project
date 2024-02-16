#ifndef DHT_H_
#define DHT_H_

typedef struct
{
	float Temperature;
	float Humidity;
	float Temp1;
	float Temp2;
	float Hum1;
	float Hum2;
}DHT_DataTypedef;


void DHT_GetData (DHT_DataTypedef *DHT_Data);
void DHT_Start();
#endif 
