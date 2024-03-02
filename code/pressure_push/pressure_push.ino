#include <SoftwareSerial.h>
#include <Wire.h>

//#define led_pin 11
#define fsr_pin A0
//壓感一邊接A0一邊接電阻和電壓源


SoftwareSerial BTSerial(10, 11);
char pressure = 'P';
char End = 'E';

int times = 0;  
int sample =50; //選取壓感測平均值之樣本數
long sample_total=0;
long fsr_value = 0;  //讀取壓電值
int sample_time =500; //每五秒將所抓取的所有壓電感測取平均做輸出，可修改但sample_time/sample 須為整數會比較好
int sit_range[2]={500,900}; //坐下時壓感判定範圍
int lie_range[2]={10,300};  //趴下時壓感判定範圍
bool state_lie = false;  //判別前一個狀態是坐或趴
bool state_inbed = false;
bool record = true;
unsigned long time;
long recorded_time = 0;

//啟動LED的效果是：若目前趴在床上且之後坐起來，則打開燈；若目前坐在床上且之後趴在床，則關燈

void setup()
{
  Wire.begin();
  BTSerial.begin(38400);
  
  Serial.begin(9600);
  //pinMode(led_pin, OUTPUT);  
}

void loop()
{
  time = millis();
  if(record){
    recorded_time = time;
    record = false;
  }
  if(time>=(recorded_time+sample_time*10/sample)){
    fsr_value = analogRead(fsr_pin); // 讀取FSR
    
    // int led_value = map(fsr_value, 0, 1023, 0, 255); // 從0~1023映射到0~255
    // analogWrite(led_pin, led_value); // 改變LED亮度
    times++;
    sample_total+=fsr_value;
    if(times>=sample){
      fsr_value = sample_total / sample;  //壓感平均值計算
      Serial.println(fsr_value);
      Serial.println("-------------");
  /*
      if(state_inbed){
        fsr_value
        if(fsr_value < 10){
          state_inbed = false;
          BTSerial.write(pressure);
          
          BTSerial.write(End);
          
        }
      }
      else{
        if(fsr_value > 10){
          state_inbed = true;
        }
      }
      if(state_lie = true){   //若目前趴在床上且之後坐起來，則打開燈
        if(fsr_value > sit_range[0] && fsr_value < sit_range[1]){
          digitalWrite(led_pin, 1);
          state_lie = false;
        }
     }
     else{
       if(state_lie = false){ //若目前坐在床上且之後趴在床，則關燈
        if(fsr_value > lie_range[0] && fsr_value < lie_range[1]){
          digitalWrite(led_pin, 0);
          state_lie = true ;
        }
       }
     }
     
     */
  
      
      times = 0;
      sample_total = 0;
    }
    record=true;
  }
  

  
  
  //delay(sample_time/sample);
}
