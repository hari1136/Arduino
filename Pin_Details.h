hw_timer_t *timer = NULL;
volatile bool timerFlag = false, Intf = false;
bool Light = 0, eprom = 0;
bool changeDate=0;
float var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12;
bool sqr, pkv, revwt, revvr, pkc, noa, nob, noc, sga, sgb, sgc;
#define LIGHT_SENSOR_PIN 33 // ESP32 pin GIOP33 (ADC0)
double res1 = 0.0002097747;
double res2 = 0.0002090907;
double res3 = 0.0002091807;
double res4 = 0.0000130684;
double res5 = 0.0000124965;
double res6 = 0.0000136217;
double res7 = 0.1347045814;
double res8 = 0.1283893120;
double res9 = 0.1407674285;
double res10 = 0.1350625431;
double res11 = 0.1307623942;
double res12 = 0.1402251196;
int32_t intrstat;
uint8_t currenthour = 10, currentminute;
uint16_t currentyear;
uint8_t ac = 0;
uint8_t StartHour = 0, StartMinute = 0, StopHour = 0, StopMinute = 0;
const char*  Err = "0";
const char*  Not = "EM";
const int interruptPin = 4;  // GPIO pin for the interrupt
int i = 0;
uint8_t er = 0;
float  Var_a,  Var_b,  Var_c;
float VAA, VAB, VAC;
float TotalUnit, EUnit, TotalPower, MaxPower;
float pf1, pf2, pf3;
const int METER_COUNT = 1;
float Voltage   [METER_COUNT][3];
float Current   [METER_COUNT][3];
float W         [METER_COUNT][3];
//float TotalUnit [METER_COUNT];
float Unit[METER_COUNT * 3];
float Voltage_r, Voltage_y, Voltage_b;
float Current_r, Current_y, Current_b;
float Frequency, freq;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

time_t epochTime;

bool RelayState = 0, Relay2State = 0;

#define LIGHT_SENSOR_PIN 33
#define OnOffRelayPin  25
#define LightPin  2 //26
#define SwitchOverRelayPin  27
#define EmergencyPin  21

#include "ADE7758.h"
ADE7758 meter;
char  CurrentTimetoDisplay[20];
char  CurrentTime[10];
char  CurrentDate[10];
void energy_setup()
{

  meter.Init();
  meter.setSPI();

  meter.write16(AWG, 0x0000);
  meter.write16(BWG, 0x0000);
  meter.write16(CWG, 0x0000);
  meter.write8(GAIN, 0x00);
  meter.write8(WDIV, 0x00);
  meter.write8(VADIV, 0x00);
  meter.write8(VARDIV, 0x00);
  meter.write16(AVARG, 0x0000);
  meter.write16(BVARG, 0x0000);
  meter.write16(CVARG, 0x0000);
  meter.write16(AVAG, 0x0000);
  meter.write16(BVAG, 0x0000);
  meter.write16(CVAG, 0x0000);
  meter.write16(AVRMSGAIN, 0x00);
  meter.write16(BVRMSGAIN, 0x0000);
  meter.write16(CVRMSGAIN, 0x0000);
  meter.write16(AIGAIN, 0x0000);
  meter.write16(BIGAIN, 0x0000);
  meter.write16(CIGAIN, 0x0000);
  meter.setAVoltageOffset(0x0000);
  meter.setBVoltageOffset(0x0000);
  meter.setCVoltageOffset(0x0000);
  meter.setACurrentOffset(0x0000);
  meter.setBCurrentOffset(0x0000);
  meter.setCCurrentOffset(0x0000);
  meter.gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_5V, GAIN_1, GAIN_1);
  meter.setOpMode(0x00);
  delay(5);
  meter.setMMode(0x00);
  delay(5);
  meter.setWavMode(0x20);//00
  delay(5);
  meter.setCompMode(0x9C);//5C
  delay(5);
  //meter.setLcycMode(0xBF);
  delay(5);
  meter.setVPINTLVL(0xBF);
  delay(5);
  meter.setIPINTLVL(0x6E);
  delay(5);
  meter.setSAGLVL(0x69);
  delay(5);
  //meter.write16(APCFDEN,0X0168);     //360 in decimal= 168 in hexa
  //delay(5);
  //meter.write16(APCFNUM,0x0001);    //set APCFNUM for value 1
  //delay(5);
  //meter.write16(VARCFDEN,0X017A);     //360 in decimal= 168 in hexa   //378
  //delay(5);
  //meter.write16(VARCFNUM,0x0001);    //set APCFNUM for value 1

  delay(5);
  meter.write16(LINECYC, 0x00);
  delay(5);
  meter.write24(MASK, 0xAC1F8);
  delay(5);
  //meter.resetStatus();
  //delay(5);
  //meter.write8(GAIN,(GAIN_1<<5)|(GAIN_1));
  //delay(5);
  //meter.write16(AWG,0x6CD);
  //delay(5);
  //meter.write16(BWG,0x71A);
  //delay(5);
  //meter.write16(CWG,0x726);
  //delay(5);
  meter.write16(AVAG, 0x9);
  delay(5);
  //meter.write16(BVAG,0x6DB);
  //delay(5);
  //meter.write16(CVAG,0x6EE);
  //delay(5);
  //meter.write8(APHCAL,0x592);
  //delay(5);
  //meter.write8(BPHCAL,0x596);
  //delay(5);
  //meter.write8(CPHCAL,0x5A8);
  //delay(5);
  //meter.write16(AVARG,0x00B9);
  //delay(5);
  //meter.write16(BVARG,0x063F);
  //delay(5);
  //meter.write16(CVARG,0x0630);
  //delay(5);
  Serial.println(meter.getCompMode());

  meter.closeSPI();


}
