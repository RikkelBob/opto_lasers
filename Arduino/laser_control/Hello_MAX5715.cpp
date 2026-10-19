// /*******************************************************************************
// * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
// *
// * Permission is hereby granted, free of charge, to any person obtaining a
// * copy of this software and associated documentation files (the "Software"),
// * to deal in the Software without restriction, including without limitation
// * the rights to use, copy, modify, merge, publish, distribute, sublicense,
// * and/or sell copies of the Software, and to permit persons to whom the
// * Software is furnished to do so, subject to the following conditions:
// *
// * The above copyright notice and this permission notice shall be included
// * in all copies or substantial portions of the Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
// * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// * OTHER DEALINGS IN THE SOFTWARE.
// *
// * Except as contained in this notice, the name of Maxim Integrated
// * Products, Inc. shall not be used except as stated in the Maxim Integrated
// * Products, Inc. Branding Policy.
// *
// * The mere transfer of this software does not imply any licenses
// * of trade secrets, proprietary technology, copyrights, patents,
// * trademarks, maskwork rights, or any other form of intellectual
// * property whatsoever. Maxim Integrated Products, Inc. retains all
// * ownership rights.
// *******************************************************************************
// */
// example code includes
// standard include for target platform -- Platform_Include_Boilerplate
#include "Arduino.h"
// SPI interface
#include "SPI.h"
// end Platform_Include_Boilerplate
#include "MAX5715.h"

// example code declare SPI interface
// global SPI uses conventional Arduino 10-pin header D11 D12 D13
// chip select is on Arduino 10-pin header D10
uint8_t spi_cs = 10; // Generic: Arduino 10-pin header D10

// example code declare GPIO interface pins
uint8_t LDACb_pin = 9; // Digital Trigger Input to MAX5715 device
uint8_t CLRb_pin = 8; // Digital Trigger Input to MAX5715 device
// uint8_t REF_pin = Px_x_PortName_To_Be_Determined; // Reference Input to MAX5715 device
// uint8_t OUTA_pin = A0; // Analog Output from MAX5715 device
// uint8_t OUTB_pin = A1; // Analog Output from MAX5715 device
// uint8_t OUTC_pin = A2; // Analog Output from MAX5715 device
// uint8_t OUTD_pin = A3; // Analog Output from MAX5715 device
// uint8_t RDYb_pin = Px_x_PortName_To_Be_Determined; // Digital MAX5715 Output from MAX5715 device
// example code declare device instance
MAX5715 g_MAX5715_device(spi_cs, LDACb_pin, CLRb_pin, MAX5715::MAX5715_IC);

// Custom code starts here! - E. Dijkema, 2022
// Values sent over serial communication (CODEallLOADall) are used to set DAC values in mV. Reference is set to 4.096 volts, so do not go over command = 4095
// 4 available channels, to drive 4 lasers independently

//Stimulus variables
uint16_t ch = 0;
uint16_t stimAmp;
int command;
double f = 20;
double period = 1 / f * 1000;
double stimDur = 1000;
double taperDur = 100;
int taperSteps = 100;
int msPerStep = taperDur/taperSteps;
double ampPerStep;

double stimPulseDur = 10;

double stimPulses = stimDur * f / 1000;
double taperedStimPulses = (stimDur - taperDur) * f / 1000;
double taperPulses = taperDur * f / 1000;
int currentAmp;

//Serial communication variables
const byte numChars = 16;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

// variables to hold the parsed data
char stimulationType[numChars] = {0};
//char stimulusType;
boolean newData = false;
char type;

void pulse(int command) {
  stimAmp = command;
  Serial.println("Starting pulsed stimulation");
  Serial.println(command);
  Serial.println("Laser On");
  for (int i = 0; i <= stimPulses; i++) {              //Stimulus loop
    g_MAX5715_device.CODEallLOADall(stimAmp);
    delay(stimPulseDur);
    g_MAX5715_device.CODEallLOADall(0x000);
    delay(period - stimPulseDur);
  }
  Serial.println("Laser Off");
}

void pulseTapered(int command) {
  stimAmp = command;
  Serial.println("Starting tapered pulsed stimulation");
  Serial.println(command);
  Serial.println("Laser On");
  for (int i = 0; i <= taperedStimPulses; i++) {              //Stimulus loop
    g_MAX5715_device.CODEallLOADall(stimAmp);
    delay(stimPulseDur);
    g_MAX5715_device.CODEallLOADall(0x000);
    delay(period - stimPulseDur);
  }

  for (int i = 0; i <= taperPulses; i++) {              //Taper loop
    currentAmp = stimAmp - ((stimAmp / taperPulses) * i);
    Serial.println(currentAmp);
    g_MAX5715_device.CODEallLOADall(currentAmp);
    delay(stimPulseDur);
    g_MAX5715_device.CODEallLOADall(0x000);
    delay(period - stimPulseDur);
  }
  Serial.println("Laser Off");
}

void tapered(int command) {
  stimAmp = command;
  ampPerStep = (double)stimAmp / taperSteps;
  Serial.println("Starting tapered stimulation");
  Serial.println(command);
  Serial.println("Laser On");
  
  for (int i = 0; i <= taperSteps; i++) {              //Onset loop
    currentAmp = i * ampPerStep;
    g_MAX5715_device.CODEallLOADall(currentAmp);
    delay(msPerStep);
  }
  
  g_MAX5715_device.CODEallLOADall(stimAmp); //Stimulus
  delay(stimDur - 2*taperDur);
  
  for (int i = 0; i <= taperSteps; i++) {              //Offset loop
    currentAmp = stimAmp - (i * ampPerStep);
    g_MAX5715_device.CODEallLOADall(currentAmp);
    delay(msPerStep);
  }
  g_MAX5715_device.CODEallLOADall(0x000);
  Serial.println("Laser Off");
}


void cont(int command) {
  stimAmp = command;
  Serial.println("Starting continuous stimulation");
  g_MAX5715_device.CODEallLOADall(stimAmp);
  delay(stimDur);
  g_MAX5715_device.CODEallLOADall(0x000);
}

void hold(int command) {
  stimAmp = command;
  Serial.println("Laser On (hold), send <o> to switch off");
  g_MAX5715_device.CODEallLOADall(stimAmp);
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  strcpy(stimulationType, strtokIndx); // copy it to stimtype

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  stimAmp = strtokIndx ? atoi(strtokIndx) : 0;     // convert this part to an integer, 0 if missing
}

void setup() {
  Serial.begin(115200);
  g_MAX5715_device.Init();
  g_MAX5715_device.REF(MAX5715::REF_AlwaysOn_4V096);
  //Serial.setTimeout(1);
}

void loop() {
  recvWithStartEndMarkers();
  
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0

    parseData();
    char type = stimulationType[0];
    Serial.println(type);
    Serial.println(stimAmp);

    if (type == 'c') {
      cont(stimAmp);
    }
    else if (type == 'p') {
      pulse(stimAmp);
    }
    else if (type == 't') {
      tapered(stimAmp);
    }
    else if (type == 'h') {
      hold(stimAmp);
    }
    else if (type == 'o') {
      g_MAX5715_device.CODEallLOADall(0x000);
      Serial.println("Laser Off");
    }
    newData = false;
  }
}
