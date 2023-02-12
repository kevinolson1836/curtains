/*
 * Created by Kevin Olson
 * starter code from ArduinoGetStarted.com
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rtc
 */

// sets debug mode
#define DEBUG 1


//*****************************start of RTC vars*****************************

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h> // Library for I2C communication
#include <SPI.h>  // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h" //for the real time clock

// UNCHANGABLE PARAMATERS
#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

//times to trigger blinds open or close
int times[] = {8, 20};     //9am  8pm

RTC_DS3231 rtc;
//*****************************end of RTC vars*****************************



//*****************************START OF STEPPER MOTER CONFIGS *****************************

//pins for the stepper on the esp32866
//#define IN1 14
//#define IN2 12
//#define IN3 13
//#define IN4 15



//pins for the stepper on the arduino uno
const int stepPin = 5; 
const int dirPin = 2; 
const int enPin = 8;

// how many times to spin the shaft
int revolutionsToOpen = 10;

// 1600 rotations for 1 full turn of the shaft
int stepsToOpen = 1600;


// config for the delay, init at a negitvie number
int delay_hour = -10;
int delay_min = -10;
int delay_sec = -10;



void setup () {
 
    // set up for serial data
    Serial.begin(9600);
    Serial.println("starting up!");

    // SETUP RTC MODULE
    if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    // sets the RTC to the date & time on PC this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    //rtc.adjust(DateTime(F(__DATE__), F("07:59:55"))); //for debuging dont uncomment, will mess up the correct time

    // setup stepper pins
    pinMode(stepPin,OUTPUT); 
    pinMode(dirPin,OUTPUT);
    pinMode(enPin,OUTPUT);
    digitalWrite(enPin,LOW);
}



void loop () {

    DateTime now = rtc.now();

    if(DEBUG){
        // start of the debug lines.
        Serial.println("******start of debug loop************");

        Serial.print("hour: ");
        Serial.print(now.hour());
        Serial.print("      min: ");
        Serial.print(now.minute());
        Serial.print("      sec: ");
        Serial.println(now.second());

        Serial.print("correcttime: ");
        Serial.println(correctTime(now));
    }
    
    // if current time is in the times array
    if (correctTime(now)){
        //delay moving up the blidns on a 10 min cooldown

        if(daytime(now)){ //its day time
            
            //debug info
            if(DEBUG){
                Serial.println("opening");
            }

            // open the blinds
            openBlinds();
        }else{ //night time close blinds
                
            //debug info
            if(DEBUG){
                Serial.println("closing");
            }

            // close the blinds
            closeBlinds();
        }

        // delay and allow the delay time to pass
        Serial.println("Delaying for 1 min and 1 second");
        delay(61000);
    }

    //end of the debug info line here 
    if(DEBUG){
        Serial.println("delay for 500 to read debug");
        Serial.println("******end of debug loop************");
        Serial.println();Serial.println();Serial.println();Serial.println();
        delay(500);
    }
}


//checks if its a weekday, returns 1 if its a weekday
int isWeekDay(DateTime now){
    if( now.dayOfTheWeek() == 1 ||  //Monday
        now.dayOfTheWeek() == 2 ||  //Tuesday
        now.dayOfTheWeek() == 3 ||  //Wendsay
        now.dayOfTheWeek() == 4 ||  //Thursday
        now.dayOfTheWeek() == 5     //Friday
      ){
            //is a week day, return a 1
            return 1; 
        }else{
            //not a weekday, return 0
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
                // time is in the accepted array
                Serial.println("it is the correct time");
                return 1; 
            }
        }
    }

    //current time not in list, return 0  
    return 0;
}


//checks if its day time
int daytime(DateTime now){

    // if hour is above 12 it is no longer day time
    if(now.hour() < 13 ){
        //its day time, return 1
        Serial.println("day time");
        return 1;
    }else{
        //its night time, return 0
        Serial.println("night time");
        return 0;
    }
}


//opens blinds
int openBlinds(){
    Serial.println("opening");

    digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
    
    // the main loop that moves the moter. stepsToOpen times how many times we want it to spin
    for(int x = 0; x < stepsToOpen * revolutionsToOpen; x++) {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(500); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(500); 
    }
    return 1;
}



//closes blinds
int closeBlinds(){
    Serial.println("closing");

    digitalWrite(dirPin,LOW); //Changes the direction of rotation
  
    // the main loop that moves the moter. stepsToOpen times how many times we want it to spin
    for(int x = 0; x < stepsToOpen * revolutionsToOpen; x++) {
        digitalWrite(stepPin,HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin,LOW);
        delayMicroseconds(500);
    }
    return 1;
}



// checks if the delay time has past. returns 1 if delay is over 
int isDelayOver(DateTime now){
    //if its been more than 3 mins or delay+1 min. returns 1
    if(now.minute() > 3 || now.minute() > delay_min+1 ){
        Serial.println("delay over");
        return 1;
    }else{
        //return 0 within 3 mins of last call
        Serial.println("delay NOT over!!!!!!");
        return 0;
    }
}