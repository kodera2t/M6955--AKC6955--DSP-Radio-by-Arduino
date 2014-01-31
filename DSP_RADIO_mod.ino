/*
 * DSP Radio control program on Arduino IDE
 *
 *  Created on: Dec. 24 , 2013
 *      Author: kodera2t
 */

#include <I2CLiquidCrystal.h>
#include <Wire.h>
I2CLiquidCrystal lcd(20, true);
#define RADIO 0x10
int read_byte,raw_upper,upper,lower,mode;
float freq;
 int channel_num,s_upper,s_lower,hoge,initial_num;
 char s_upper2, s_lower2;
int terminal_1  = 2;
int terminal_2  = 4;
volatile char old_state = 0;
volatile int encorder_val  = 0;
int mode_set;
int ct,pt,event;
volatile int band_mode = LOW;
float listen_freq;

void i2c_write(int device_address, int memory_address, int value)
{
  Wire.beginTransmission(device_address);
  Wire.write(memory_address);
  Wire.write(value);
  Wire.endTransmission();
  delay(3);
}

void i2c_read(int device_address, int memory_address)
{
Wire.beginTransmission(device_address);
Wire.write(memory_address);
Wire.endTransmission(false);
Wire.requestFrom(device_address, 1, false);
read_byte = Wire.read();
Wire.endTransmission(true);
delay(2);
}

void setup()
{
    unsigned int upper,lower,raw_upper;
    unsigned int mask,mode,mode_set,hoge;
    Wire.begin() ;
  attachInterrupt(0, Rotary_encorder, CHANGE);
  attachInterrupt(1,mode_setting, CHANGE);
    lcd.begin(16,2);
  pinMode(terminal_1, INPUT);
  pinMode(terminal_2, INPUT);
  pinMode(3, INPUT);
  digitalWrite(terminal_1, HIGH);
  digitalWrite(terminal_2, HIGH);
//  digitalWrite(3, HIGH);
///
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00        
    i2c_write(RADIO,0x01,0b00010011); ///AM-band
    i2c_write(RADIO,0x04,0x00);
    i2c_write(RADIO,0x05,0xff);
    listen_freq=1008.0; ////frequency MHz for FM, kHz for AM
    channel_num=listen_freq/3; 
    s_upper2=channel_num/256 | 0b01100000;
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
    
    mode_set=0;
//      i2c_read(RADIO,0x14);
//  raw_upper = read_byte;
//  upper = (raw_upper&0b00011111)<<8;
//  delay(200);
//  i2c_read(RADIO,0x15);  
//  lower = read_byte;
//  i2c_read(RADIO,0x00);
//  mode= (read_byte&0b01000000)>>6;
lcd.setCursor(0,0);
    hoge=channel_num;

//mode==1:FM, mode==0:AM
if(mode==1){
  lcd.print("FM");
  freq=int(hoge)*0.025+30.0;
}else{
  if(mode_set==2){
lcd.print("SW");
  freq=int(hoge)*5;  
  }else{
lcd.print("AM");
  freq=int(hoge)*3;
}}
lcd.setCursor(0,1);
lcd.print(freq,1);
lcd.setCursor(8,1);
if(mode==1){
lcd.print("MHz");
}else{
lcd.print("kHz");
}
}///end of setup

void loop()
{


if(event==1){
      switch(mode_set){
        case 0://AM
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00        
    i2c_write(RADIO,0x01,0b00010011); ///AM-band
    listen_freq=1008.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/3; 
    channel_num=initial_num+encorder_val*3; 
    s_upper2=channel_num/256 | 0b01100000;
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
        mode=0; 
        break;
        case 1://FM
    i2c_write(RADIO,0x01,0b00010011); ///FM-band Japan       
    i2c_write(RADIO,0x00,0b11000000); ///power_on,FM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=83.3; ////frequency MHz for FM, kHz for AM
//    channel_num=(listen_freq-30)*40+encorder_val;
    initial_num=(listen_freq-30)*40; 
    channel_num=initial_num+encorder_val*4; 
    s_upper2=channel_num/256 | 0b01100000;
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b11100000);
    i2c_write(RADIO,0x00,0b11000000);
    mode=1; 
        break;        
        case 2://SW4
    i2c_write(RADIO,0x01,0b00111011); ///SW4       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=6900.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val; 
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);   
          mode=0;   
        break; 
               case 3://SW5
    i2c_write(RADIO,0x01,0b01000011); ///SW5       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=8000.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);
        mode=0;     
        break;  
                      case 4://SW6
    i2c_write(RADIO,0x01,0b01001011); ///SW6       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=9500.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/5; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01000000; // AM, 5kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000);  
         mode=0;    
        break;  
                      case 5://LW
    i2c_write(RADIO,0x01,0b00000011); ///LW       
    i2c_write(RADIO,0x00,0b10000000); ///power_on,AM, tune0,seek0,seek_down,non_mute,00 
    listen_freq=234.0; ////frequency MHz for FM, kHz for AM
    initial_num=listen_freq/3; 
    channel_num=initial_num+encorder_val;
    s_upper2=channel_num/256 | 0b01100000; // AM, 3kHz step
    s_lower2= channel_num&0b0000011111111;
    i2c_write(RADIO,0x03,s_lower2);
    i2c_write(RADIO,0x02,s_upper2);
    i2c_write(RADIO,0x00,0b10100000);
    i2c_write(RADIO,0x00,0b10000000); 
        mode=0;     
        break;            
   
}

//  i2c_read(RADIO,0x14);
//  raw_upper = read_byte;
//  upper = (raw_upper&0b00011111)<<8;
//  delay(200);
//  i2c_read(RADIO,0x15);  
//  lower = read_byte;
//  i2c_read(RADIO,0x00);
//  mode= (read_byte&0b01000000)>>6;
lcd.setCursor(0,0);
    hoge=channel_num;
if(mode==1){
  lcd.print("FM");
  freq=int(hoge)*0.025+30.0;
}else{
////AM no sekai
if(mode_set==5){
  lcd.print("LW");
    freq=int(hoge)*3; 
}
  else{ if(mode_set>=2){
lcd.print("SW");
  freq=int(hoge)*5;  
  }else{
lcd.print("AM");
  freq=int(hoge)*3;
}}}
lcd.setCursor(0,1);
lcd.print(freq);
lcd.setCursor(5,1);
if(mode==1){
lcd.print("MHz");
}else{
lcd.print("kHz");
}
event=0;
}  
}

void mode_setting(){
int sw;
ct=millis();
delay(10);
sw=digitalRead(3);
if(sw==LOW && (ct-pt)>50){
band_mode=HIGH;
mode_set=mode_set+1;
}
pt=ct;
if(mode_set>6){
  mode_set=0;
}
event=1;
encorder_val=0;
}
    
void Rotary_encorder(void)
{
  if(!digitalRead(terminal_1)){
    if(digitalRead(terminal_2)){
      old_state = 'R';
    } else {
      old_state = 'L';
    }
  } else {
    if(digitalRead(terminal_2)){
      if(old_state == 'L'){ 
        encorder_val--;
      }
    } else {
      if(old_state == 'R'){
        encorder_val++;
      }
    }
    old_state = 0;
    event=1;
  }

}
