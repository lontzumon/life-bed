#include <SoftwareSerial.h>
#include <MPU6050.h>
#include <Wire.h>
#include <math.h>
#include <Timer.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
//#define led_pin 10  //壓感小夜燈腳位LED


#define REPORTING_PERIOD_MS 1000  // 數據顯示間隔 (毫秒)
#define REPORTING_Arrhythmia_MS 60000 // 心律不整持續60秒則report
#define Earthquake_LED 13  // 地震指示 LED 的腳位
#define Heart_LED 14  // 地震指示 LED 的腳位
#define fsr_pin 11    //壓感腳位

SoftwareSerial BTSerial(10,11);
Timer Tp;
//MAX30100
byte Arrhythmia[3];
byte Hypoxia[3];
byte pressure[2]={'p','f'};
//MPU6050
byte Earthquake[3];


//
bool state_lie = false;  //判別前一個狀態是坐或趴
bool state_inbed = false;
long press_value = 0;  //讀取壓感值
int sit_range[2]={500,900}; //坐下時壓感判定範圍
int lie_range[2]={10,300};  //趴下時壓感判定範圍
int times_press = 0;
int p_net_avg = 0;
//
void setup() {
  Wire.begin();
  Serial.begin(9600);
  BTSerial.begin(38400);
  //MPU6050
  Tp.every(20, Getp);
}
void loop() {
  
  //MPU6050
  Tp.update();
}


void Getp(){
	press_value = analogRead(fsr_pin);
	p_net_avg += press_value;
	times_press++;
	if(times_press == 50){
		p_net_avg/=50;
		Serial.println("pressure_value: ");
		Serial.println(p_net_avg);
		Serial.println("-------------");
		/*
		if(state_inbed){
			if(p_net_avg < 10){
			  state_inbed = false;
			  BTSerial.write(pressure, sizeof(byte)*2);
			}
		  }
		  else{
			if(p_net_avg > 10){
			  state_inbed = true;
			}
		  }
		  if(state_lie = true){   //若目前趴在床上且之後坐起來，則打開燈
			if(p_net_avg > sit_range[0] && p_net_avg < sit_range[1]){
			  digitalWrite(led_pin, 1);
			  state_lie = false;
			}
		 }
		 else{
		   if(state_lie = false){ //若目前坐在床上且之後趴在床，則關燈
			if(p_net_avg > lie_range[0] && p_net_avg < lie_range[1]){
			  digitalWrite(led_pin, 0);
			  state_lie = true ;
			}
		   }
		 }
		*/
	times_press = 0;
    p_net_avg = 0;
	
	}
}