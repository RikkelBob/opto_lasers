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

// DAC with serial comms
// Todo: Add channel to serial input
// Test

uint16_t ch = 0;
uint16_t stimAmp = 0x555;
int command;
double f = 20;
double period = 1 / f * 1000;
double stimDur = 750;
double taperDur = 250;

double stimPulseDur = 1;

double stimPulses = 10;
double taperPulses = taperDur * f / 1000;
double currentAmp;

void setup() {
  Serial.begin(115200);
  g_MAX5715_device.Init();
  g_MAX5715_device.REF(MAX5715::REF_AlwaysOn_4V096);
  Serial.setTimeout(1);
}

void loop() {
  while (Serial.available()) {
    command = Serial.readString().toInt();

    if (command != 0) {
      g_MAX5715_device.CODEallLOADall(command);
      delay(1000);

    }
    if (command == 0) {
      g_MAX5715_device.CODEallLOADall(0x000);
    }
  }
}
