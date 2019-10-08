/*
 * bininps
 *
 * Copyright (c) 2014 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 01/18/2012
 *
 * Device:
 * Binary input + counter module
 *
 * Description:
 * Device that reports the binary state of 12 digital inputs, 4 of them being
 * used as pulse counters as well.
 *
 * PANSTAMP AVR
 * Binary/counter inputs: pins 2, 3, 4, 5, 6, 8, 9, 10, 18, 20, 21 and 22
 *
 * PANSTAMP NRG
 * Binary/counter inputs: pins 23, 22, 11, 10, 9, 8, 6, 5, 21, 20, 19 and 18
 *
 * This sketch can be used to detect key/switch presses, binary alarms or
 * any other binary sensor. It can also be used to count pulses from a vaste
 * variety of devices such as energy meters, water meters, gas meters, etc.
 * You may want to add some delays in updateValues in order to better debounce
 * the binary states. We suggest to add external capacitors to the inputs as well.
 * Capacitor values and delays should depend on the nature and frequence of the
 * input signals.
 *
 * This device is low-power enabled so it will enter low-power mode just
 * after reading the binary states and transmitting them over the SWAP
 * network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * bininps.xml (Binary/Counter input module)
 */
 
#include "swap.h"
#include "regtable.h"
#include "PinChangeInterrupt.h"

/**
 * Macros
 */
#ifdef PANSTAMP_NRG

#else // PANSTAMP_AVR
  // PCINT Numbers, siehe https://github.com/NicoHood/PinChangeInterrupt
  #define Counter0 8   //0 //D8
  #define Counter1 9   //1 //D9
  #define Counter2 A0  //8 //A0
  #define Counter3 A1  //9 //A1
  #define Counter4 A2  //10 //A2
  #define Counter5 A3  //11 //A3
  #define Counter6 A4  //12 //A4
  #define Counter7 A5  //13 //A5
  #define Counter8 7   //23 //D7
  #define Counter9 6   //22 //D6
  #define Counter10 5  //21 //D5
  #define Counter11 3  //19 //D3

#endif

unsigned long counter[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                             // Initial counter values
unsigned int  transmissions = 0;  
/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  int i;

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(Counter0, INPUT_PULLUP);
  pinMode(Counter1, INPUT_PULLUP);
  pinMode(Counter2, INPUT_PULLUP);
  pinMode(Counter3, INPUT_PULLUP);
  pinMode(Counter4, INPUT_PULLUP);
  pinMode(Counter5, INPUT_PULLUP);
  pinMode(Counter6, INPUT_PULLUP);
  pinMode(Counter7, INPUT_PULLUP);
  pinMode(Counter8, INPUT_PULLUP);
  pinMode(Counter9, INPUT_PULLUP);
  pinMode(Counter10, INPUT_PULLUP);
  pinMode(Counter11, INPUT_PULLUP);
  
  // Attach the new PinChangeInterrupt and enable event function below 
  attachPCINT(digitalPinToPCINT(Counter0), Prg_Cnt0, RISING); 
  attachPCINT(digitalPinToPCINT(Counter1), Prg_Cnt1, RISING); 
  attachPCINT(digitalPinToPCINT(Counter2), Prg_Cnt2, RISING); 
  attachPCINT(digitalPinToPCINT(Counter3), Prg_Cnt3, RISING); 
  attachPCINT(digitalPinToPCINT(Counter4), Prg_Cnt4, RISING); 
  attachPCINT(digitalPinToPCINT(Counter5), Prg_Cnt5, RISING); 
  attachPCINT(digitalPinToPCINT(Counter6), Prg_Cnt6, RISING); 
  attachPCINT(digitalPinToPCINT(Counter7), Prg_Cnt7, RISING); 
  attachPCINT(digitalPinToPCINT(Counter8), Prg_Cnt8, RISING); 
  attachPCINT(digitalPinToPCINT(Counter9), Prg_Cnt9, RISING); 
  attachPCINT(digitalPinToPCINT(Counter10), Prg_Cnt10, RISING); 
  attachPCINT(digitalPinToPCINT(Counter11), Prg_Cnt11, RISING); 

  // Init SWAP stack
  swap.init();
  
  // Transmit product code
  swap.getRegister(REGI_PRODUCTCODE)->getData();

  // Enter SYNC state
  swap.enterSystemState(SYSTATE_SYNC);

  // During 3 seconds, listen the network for possible commands whilst the LED blinks
  for(i=0 ; i<6 ; i++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(400);
  }
  // Transmit periodic Tx interval
  swap.getRegister(REGI_TXINTERVAL)->getData();
  // Transmit power voltage
  swap.getRegister(REGI_VOLTSUPPLY)->getData();
  
  // Transmit initial binary states
  //swap.getRegister(REGI_BININPUTS)->getData();
  // Transmit initial counter values
  swap.getRegister(REGI_COUNTERS)->getData();
  
  // Switch to Rx OFF state
  swap.enterSystemState(SYSTATE_RXOFF);

}

void Prg_Cnt0(void) {counter[0]++;}
void Prg_Cnt1(void) {counter[1]++;}
void Prg_Cnt2(void) {counter[2]++;}
void Prg_Cnt3(void) {counter[3]++;}
void Prg_Cnt4(void) {counter[4]++;}
void Prg_Cnt5(void) {counter[5]++;}
void Prg_Cnt6(void) {counter[6]++;}
void Prg_Cnt7(void) {counter[7]++;}
void Prg_Cnt8(void) {counter[8]++;}
void Prg_Cnt9(void) {counter[9]++;}
void Prg_Cnt10(void) {counter[10]++;}
void Prg_Cnt11(void) {counter[11]++;}

void pcEnableInterrupt() {
  enablePCINT(digitalPinToPCINT(Counter0)); 
  enablePCINT(digitalPinToPCINT(Counter1)); 
  enablePCINT(digitalPinToPCINT(Counter2)); 
  enablePCINT(digitalPinToPCINT(Counter3)); 
  enablePCINT(digitalPinToPCINT(Counter4)); 
  enablePCINT(digitalPinToPCINT(Counter5)); 
  enablePCINT(digitalPinToPCINT(Counter6)); 
  enablePCINT(digitalPinToPCINT(Counter7)); 
  enablePCINT(digitalPinToPCINT(Counter8)); 
  enablePCINT(digitalPinToPCINT(Counter9)); 
  enablePCINT(digitalPinToPCINT(Counter10)); 
  enablePCINT(digitalPinToPCINT(Counter11)); 
}

void pcDisableInterrupt() {
  disablePCINT(digitalPinToPCINT(Counter0)); 
  disablePCINT(digitalPinToPCINT(Counter1)); 
  disablePCINT(digitalPinToPCINT(Counter2)); 
  disablePCINT(digitalPinToPCINT(Counter3)); 
  disablePCINT(digitalPinToPCINT(Counter4)); 
  disablePCINT(digitalPinToPCINT(Counter5)); 
  disablePCINT(digitalPinToPCINT(Counter6)); 
  disablePCINT(digitalPinToPCINT(Counter7)); 
  disablePCINT(digitalPinToPCINT(Counter8)); 
  disablePCINT(digitalPinToPCINT(Counter9)); 
  disablePCINT(digitalPinToPCINT(Counter10)); 
  disablePCINT(digitalPinToPCINT(Counter11)); 
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  // Sleep for panstamp.txInterval seconds (register 10)
  swap.goToSleep();
  //delay(2000);

  //pcDisableInterrupt();
  swap.getRegister(REGI_COUNTERS)->getData();
  transmissions++;

  //transmit Battery Voltage every 100 transmissions
  if((transmissions % 100)== 0) {
    swap.getRegister(REGI_VOLTSUPPLY)->getData();
    transmissions = 0;
  }
  //pcEnableInterrupt();
  
}

