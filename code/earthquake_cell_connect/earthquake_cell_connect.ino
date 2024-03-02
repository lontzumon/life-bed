#include <SoftwareSerial.h>
#include <MPU6050.h>
#include <Wire.h>
#include <math.h>

SoftwareSerial BTSerial(10, 11);
char Earthquake[3] = {'E', '1', '2'};
char End = '';
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
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  BTSerial.begin(38400);
  accelgyro.initialize();
}
void loop()
{
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
    if(times == 10){
      a_net_avg /= 10;
      if(a_net_avg<25){
        Serial.println("Small earthquake");
        Serial.print("Intensity : ");
        Serial.println(a_net);
     }
     else if(25<=a_net_avg && a_net_avg<80){ //4th level earthquake  ,4級警示不逃跑  8,25,80,250,400
       BTSerial.write(Earthquake[0]);
       BTSerial.write(Earthquake[1]);
       BTSerial.write(End);
       Serial.println("Earthquake : Level 4");
       Serial.print("Intensity : ");
        Serial.println(a_net);
     }
      else if(80<=a_net_avg){ //>5th level earthquake  5級以上警示逃跑
        BTSerial.write(Earthquake[0]);
        BTSerial.write(Earthquake[2]);
        BTSerial.write(End);
        Serial.println("Earthquake : Level 5 ~ 8");
        Serial.print("Intensity : ");
        Serial.println(a_net);
      }
      times = 0;
      a_net_avg = 0;
    }
    
    
    delay(100);
}
