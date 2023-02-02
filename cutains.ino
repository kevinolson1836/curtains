/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rtc
 */

#define DEBUG 1


//start of RTC vars 

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h> // Library for I2C communication
#include <SPI.h>  // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h"

// UNCHANGABLE PARAMATERS
#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

//timese to trigger blinds open or close
int times[] = {9, 20};     //9am 11am 8pm

RTC_DS3231 rtc;
//end of RTC vars 

//start of stepper vars
#include <Stepper.h>

const int stepsPerRevolution = 200; 

//pins for the stepper on the esp32866
#define IN1 14
#define IN2 12
#define IN3 13
#define IN4 15

//pins for the stepper on the arduino uno
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11


int revolutionsToOpen = 8;

  int delay_hour = -10;
  int delay_min = -10;
  int delay_sec = -10;


// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN2, IN3, IN4);

void setup () {
   // SETUP RTC MODULE
  if (! rtc.begin()) {
  
    while (1);
  }

  // sets the RTC to the date & time on PC this sketch was compiled
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  rtc.adjust(DateTime(F(__DATE__), F("08:59:55")));

  // set the speed at 60 rpm:
  myStepper.setSpeed(60);

}

void loop () {
  DateTime now = rtc.now();

  
    
    
  //corrrect time
  if (correctTime(now)){
          //delay moving up the blidns on a 3 min cooldown
              if(daytime(now)){
                //daytime open blinds
//                if(DEBUG){
//                  Serial.println("opening");
//                }
                 openBlinds();
              }else{
                //nighttime close blinds
//                if(DEBUG){
//                  Serial.println("closing");
//                  }
                  closeBlinds();
              }
      
        delay(61000);
        }
    



      
}


//checks if its a weekday
int isWeekDay(DateTime now){
  if(now.dayOfTheWeek() == 1 ||  //Monday
     now.dayOfTheWeek() == 2 ||  //Tuesday
     now.dayOfTheWeek() == 3 ||  //Wendsay
     now.dayOfTheWeek() == 4 ||  //Thursday
     now.dayOfTheWeek() == 5     //Friday
  ){
    //is a week day
    return 1; 
  }else{
    //not a weekday
    return 0;
  }
}


//checks if the current time is in list of accepted times
int correctTime(DateTime now){
  //loop over items in times array  
  for (int i=0; i < sizeof(times)/ sizeof(times[0]); i++) {
   if(now.hour() == times[i] ){
      //give 1 mins for buffer time
      if(now.minute() < 1){
        return 1; 
      }
    }
  }
  //current time not in list, return 0  
  return 0;
}


//checks if its day time
int daytime(DateTime now){
  if(now.hour() < 13 ){
      //its day time
    
      return 1;
  } else{
      //its night time
  
      return 0;
  }
}


//opens blinds
int openBlinds(){
 
  myStepper.step(stepsPerRevolution * revolutionsToOpen);
  delay(500);
  return 1;
}


//closes blinds
int closeBlinds(){

  myStepper.step(-stepsPerRevolution * revolutionsToOpen);
  delay(500);
  return 1;
}

int isDelayOver(DateTime now){
      //if its been more than 3 mins return 1
      if(now.minute() > 3 ||now.minute() > delay_min+1 ){
      
        return 1;
      } else{
        //return 0 within 3 mins of last call
        
        return 0;
      }
  }
