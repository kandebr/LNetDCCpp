/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2015 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "CurrentMonitor.h"

///////////////////////////////////////////////////////////////////////////////

CurrentMonitor::CurrentMonitor(int pin, char *msg){
    this->pin=pin;
    this->msg=msg;
    current=0;
    setGlobalPower(OFF);
  } // CurrentMonitor::CurrentMonitor
  
boolean CurrentMonitor::checkTime(){
  if(millis()-sampleTime<CURRENT_SAMPLE_TIME)            // no need to check current yet
    return(false);
  sampleTime=millis();                                   // note millis() uses TIMER-0.  For UNO, we change the scale on Timer-0.  For MEGA we do not.  This means CURENT_SAMPLE_TIME is different for UNO then MEGA
  return(true);  
} // CurrentMonitor::checkTime
  
boolean CurrentMonitor::check(){
  current=analogRead(pin)*CURRENT_SAMPLE_SMOOTHING+current*(1.0-CURRENT_SAMPLE_SMOOTHING);        // compute new exponentially-smoothed current
  if(current>CURRENT_SAMPLE_MAX && digitalRead(SIGNAL_ENABLE_PIN_PROG)==HIGH){                    // current overload and Prog Signal is on (or could have checked Main Signal, since both are always on or off together)
    setGlobalPower(EMERGENCY);
    return(true);
  } 
  return(false);
} // CurrentMonitor::check  

void CurrentMonitor::setGlobalPower(uint8_t pPower)
{
  if (pPower==ON)
  {
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,HIGH);
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,HIGH);
    digitalWrite(PWON_LED_PIN, HIGH);
    digitalWrite(PWOFF_LED_PIN, LOW);
    digitalWrite(EMERGENCY_LED_PIN, LOW);
    this->globalPowerON=ON;
    INTERFACE.println("<p1>");
  }
  else if (pPower==OFF)
  {
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,LOW);
    digitalWrite(PWON_LED_PIN, LOW);
    digitalWrite(PWOFF_LED_PIN, HIGH);
    digitalWrite(EMERGENCY_LED_PIN, LOW);
    this->globalPowerON=OFF;
    INTERFACE.println("<p0>");
  }
  else if (pPower==EMERGENCY)
  {
    Serial.println(" EMERGENCY ");
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,LOW);
    digitalWrite(PWON_LED_PIN, LOW);
    digitalWrite(PWOFF_LED_PIN, LOW);
    digitalWrite(EMERGENCY_LED_PIN, HIGH);
    this->globalPowerON=EMERGENCY;
    INTERFACE.println("<p0>");
  }
}

long int CurrentMonitor::sampleTime=0;
byte CurrentMonitor::globalPowerON=OFF;

