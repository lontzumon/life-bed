#include <SoftwareSerial.h>
#include <MPU6050.h>
#include <Wire.h>
#include <math.h>
#include <Timer.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>

#define REPORTING_PERIOD_MS 1000  // 數據顯示間隔 (毫秒)
#define REPORTING_Arrhythmia_MS 60000 // 心律不整持續60秒則report
#define Earthquake_LED 13  // 地震指示 LED 的腳位
#define Heart_LED 14  // 地震指示 LED 的腳位

SoftwareSerial BTSerial(10,11);
int ZeroOne = 0;
Timer Tg;
Timer Tzeroone;

//MAX30100
byte Arrhythmia[3];
byte HypoxiaHypoxia[3];
PulseOximeter pox;
uint32_t tsLastReport = 0;
uint32_t tsLastArrhythmia = 0;
boolean ArrhythmiaStart = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C 位址和長寬

//MPU6050
byte Earthquake[3];
MPU6050 accelgyro;
int16_t ax, ay, az,gx,gy,gz;
double dx,dy,dz,dt;
double thetax,thetay,thetaz;
double xb, yb, zb, xx,yy,zz;
int i = 0;
double a_net_g;
unsigned long int a_net;
bool debounce = false;
bool getg = true;
int times = 0;
int a_net_avg = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  BTSerial.begin(38400);
  accelgyro.initialize();
  pinMode(BEAT_LED, OUTPUT);
  digitalWrite(BEAT_LED, LOW);
  
  //變數ZeroOne
  Tzeroone.every(1000, BetweenZeroOne);
  
  //MPU6050
  Tg.every(100, GetG);
  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Pulse");
  delay(1000);
  // 初始化 MAX30100, 若失敗就用無窮迴圈卡住
  if (!pox.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("Failed to init!");
    while (true) {}
  }
  // 設定測到脈搏時要回呼的函式 (必須放在 setup 最後一行)
  pox.setOnBeatDetectedCallback(onBeatDetected);
}
void loop() {
  
  //變數0or1
  Tzeroone.update();
  
  //MAX30100
  pox.update();  // 更新 MAX30100 讀數
  // 每隔指定的時間讀一次讀數, 並顯示在 LCD 上
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) { //每秒量一次
    Arrhythmia[0] = 'A';
    Hypoxia[0] = 'H';
    float bpm = pox.getHeartRate();
    //float_to_byte(bpm, &Arrhythmia[1]);
    Serial.print("bpm : ");
    Serial.println(bpm);
    uint8_t spo2 = pox.getSpO2();
    //float_to_byte(spo2, &Arrhythmia[5]);
    Serial.print("spo2 : ");
    Serial.println(spo2);
    if(spo2 < 95.0){
      //Abnormal spo2
      Hypoxia[1] = '1';
      LED_Heart_happens(1);
     }
     else{
      Hypoxia[1] = '0';
     }
    
    if((bpm<60 || bpm>100) && !ArrhythmiaStart){
      tsLastArrhythmia = millis();
    }
    if((bpm<60 || bpm>100)){
      ArrhythmiaStart= true;
    }
    else{ ArrhythmiaStart= false; //Normal heartrate
    }
    if(millis() - tsLastArrhythmia > REPORTING_Arrhythmia_MS){ //心律不整持續超過60秒回報
      //report Arrhythmia
      Arrhythmia[1] = '1';
      LED_Heart_happens(1);
    }
    else{
      Arrhythmia[1] = '0';
    }
    
     
    lcd.clear();
    lcd.home();
    lcd.print("  Bpm : " + String(bpm));  // 顯示脈搏 bpm
    lcd.setCursor(0, 1);
    lcd.print("  SpO2: " + String(spo2) + "%");  // 顯示血氧濃度
    tsLastReport = millis();
  }

  //MPU6050
  Tg.update();
  
  
  if(Hypoxia[0] != 0){
    Hypoxia[2] = 'f';
    BTSerial.write(Hypoxia, sizeof(byte));
    Hypoxia[0] = 0;
  }
  if(Arrhythmia[0] != 0){
    Arrhythmia[2] = 'f';
    BTSerial.write(Arrhythmia, sizeof(byte));
    Arrhythmia[0] = 0;
  }
  if(Earthquake[0] != 0){
    Earthquake[2] = 'f';
    BTSerial.write(Earthquake, sizeof(byte));
    Earthquake[0] = 0;
  }
}

void GetG(){
  if(getg){
      accelgyro.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
      xb = ax;
      yb = ay;
      zb = az;
      getg = false;
  }
  accelgyro.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
  xx=(ax - xb)/16384;
  yy=(ay - yb)/16384;
  zz=(az - zb)/16384;
  a_net_g=sqrt(xx*xx+yy*yy+zz*zz);//運動加速度的值
  a_net = a_net_g*9.8*100;
  a_net_avg += a_net;
  times ++;
  //3級以下不影響生命安危，不警示
  if(times == 10){ //每秒判斷一秒內平均加速度，避免忽大忽小的數值
    a_net_avg /= 10;
    if(a_net_avg<25){
      Earthquake[0] = 'E';
      Earthquake[1] = '0';
      Serial.println("Small earthquake");
      Serial.print("Intensity : ");
      Serial.println(a_net);
    }
    else if(25<=a_net_avg && a_net_avg<80){ //4th level earthquake  ,4級警示不逃跑  8,25,80,250,400
      Earthquake[0] = 'E';
      Earthquake[1] = '1';
      Serial.println("Earthquake : Level 4");
      Serial.print("Intensity : ");
      Serial.println(a_net);
    }
    else if(80<=a_net_avg){ //>5th level earthquake  5級以上警示逃跑
      Earthquake[0] = 'E';
      Earthquake[1] = '2';
      Serial.println("Earthquake : Level 5 ~ 8");
      Serial.print("Intensity : ");
      Serial.println(a_net);
      LED_Earthquake_happens(1);
    }
    times = 0;
    a_net_avg = 0;
  }
}

void LED_Earthquake_happens(int e) {
  Earthquake_LED = (e & ZeroOne);
}

void LED_Heart_happens(int e) {
  Heart_LED = (e & ZeroOne);
}

void BetweenZeroOne(){
  ZeroOne = !ZeroOne;
}
/*void float_to_byte(float f, byte *b){
  memcpy(b, &f, 4);
}*/
