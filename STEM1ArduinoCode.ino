/*
 File/Sketch Name: AudioFrequencyDetector

 Version No.: v1.0 Created 12 December, 2019
 
 Original Author: Clyde A. Lettsome, PhD, PE, MEM
 
 Description:  This code/sketch makes displays the approximate frequency of the loudest sound detected by a sound detection module. For this project, the analog output from the 
 sound module detector sends the analog audio signal detected to A0 of the Arduino Uno. The analog signal is sampled and quantized (digitized). A Fast Fourier Transform (FFT) is
 then performed on the digitized data. The FFT converts the digital data from the approximate discrete-time domain result. The maximum frequency of the approximate discrete-time
 domain result is then determined and displayed via the Arduino IDE Serial Monitor.

 Note: The arduinoFFT.h library needs to be added to the Arduino IDE before compiling and uploading this script/sketch to an Arduino.

 License: This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License (GPL) version 3, or any later
 version of your choice, as published by the Free Software Foundation.

 Notes: Copyright (c) 2019 by C. A. Lettsome Services, LLC
 For more information visit https://clydelettsome.com/blog/2019/12/18/my-weekend-project-audio-frequency-detector-using-an-arduino/

*/

#include "arduinoFFT.h"
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2640 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values
double peak[20];
const int buttonPin1 = 5;     // the number of the pushbutton pin
const int buttonPin2 = 6;     // the number of the pushbutton pin
const int buttonPin3 = 7;     // the number of the pushbutton pin
const int buttonPin4 = 8;     // the number of the pushbutton pin
const int buttonPin5 = 9;     // the number of the pushbutton pin

const int dirPin = 3;
const int stepPin = 4;
const int stepsPerRevolution = 400;

const double stringG = 196.0; 
const double stringD = 293.6;
const double stringA = 440.0;
const double stringE = 659.3;

int TrueOrFalse; 
int buttonState1 = 0;         // variable for reading the pushbutton status
int buttonState2 = 0;         // variable for reading the pushbutton status
int buttonState3 = 0;         // variable for reading the pushbutton status
int buttonState4 = 0;         // variable for reading the pushbutton status
int buttonState5 = 0;         // variable for reading the pushbutton status

int stringFrequency; 
void setup() 
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
     pinMode(buttonPin1, INPUT);  
    pinMode(buttonPin2, INPUT);  
    pinMode(buttonPin3, INPUT);  
    pinMode(buttonPin4, INPUT);  
    Serial.begin(115200); //Baud rate for the Serial Monitor
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
    }


void loop() 
{
   // read the state of the pushbutton value:
  buttonState1 = digitalRead(buttonPin1);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState1 == HIGH) {
        Serial.println("G String Mode"); 
        TrueOrFalse = 1; 
        stringFrequency = stringG; 
  }
  
  // read the state of the pushbutton value:
  buttonState2 = digitalRead(buttonPin2);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState2 == HIGH) {
    Serial.print("D String Mode");
    TrueOrFalse = 1; 
    stringFrequency = stringD; 
  }
  buttonState3 = digitalRead(buttonPin3);

  // check if the pushbutton is  pressed. If it is, the buttonState is HIGH:
  if (buttonState3 == HIGH) {
        Serial.println("A String Mode");
       TrueOrFalse = 1; 
       stringFrequency = stringA; 

  }
  // read the state of the pushbutton value:
  buttonState4 = digitalRead(buttonPin4);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState4 == HIGH) {
        Serial.println("E String Mode");

       TrueOrFalse = 1; 
       stringFrequency = stringE; 
  }
 
  while(TrueOrFalse == 1) {
  for(int j = 0;j<7;j++) {
    frequencydetect(j); 
  }
    frequencymotor(stringFrequency); 
  

}
}
void frequencydetect(int j) 
{  
  
    for(int i=0; i<SAMPLES; i++)
    {
       //offbutton
  buttonState5 = digitalRead(buttonPin5); 
  if(buttonState5 == HIGH) { 
    TrueOrFalse = 0; 
    Serial.println("End");
   
  }
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
      
        vReal[i] = analogRead(0); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always

        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //do nothing
        }
    }
 
    /*Perform FFT on samples*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency and print peak*/
    double peak1 = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
        peak1 = -0.973485 + 0.96289*peak1 + 9.18623*pow(10,-6) * pow(peak1,2); 
    if(peak1 > 5 && peak1<1000) {
         Serial.println(peak1);     //Print out the most dominant frequency.
           peak[j] = peak1; 
    }
}

void frequencymotor(int stringFrequency) {
  
  int counter = 1; 
  int currentnum = 0;
  int avgtotal = 0;
  double currenttotal = 0;
  for(int i = 0;i<7; i++) {
     //offbutton
  buttonState5 = digitalRead(buttonPin5); 
  if(buttonState5 == HIGH) { 
    TrueOrFalse = 0; 
    Serial.println("End");
   
  }
  if(peak[i] - peak[i+1]<10 && peak[i]-peak[i+1]>-10) { 
    counter ++;
    currenttotal += peak[i]; 
    if(counter>3) {
      currenttotal += peak[i+1]; 
      int avgtotal = currenttotal/counter; 

      String string3 = "Target Frequency: ";
      String stringTargetFrequency = string3 + stringFrequency;
      Serial.println(stringTargetFrequency); 
      String string2 = "Average Total: ";
      String stringAvgTotal = string2 + avgtotal;
      Serial.println(stringAvgTotal); 
      String string4 = "Total: ";
      String stringTotal = string4 + currenttotal; 
      Serial.println(stringTotal);
       

      
      if(avgtotal - stringFrequency < 2 && avgtotal - stringFrequency > -2)
        {
          Serial.println("String is tuned"); 
          TrueOrFalse = 0; 
          counter = 1; 
          currenttotal = 0; 

        }
      else if(avgtotal < stringFrequency) {
       // Set motor direction clockwise

         if(stringFrequency<300) { // Spin motor quickly
              digitalWrite(dirPin, LOW);

          for(int x = 0; x < stepsPerRevolution; x++)
              { 
            digitalWrite(stepPin, HIGH);
           delayMicroseconds(2000);
           digitalWrite(stepPin, LOW);
             delayMicroseconds(2000);
               } 
         }
         else {
              digitalWrite(dirPin, HIGH);

                for(int x = 0; x < stepsPerRevolution; x++)
              { 
            digitalWrite(stepPin, HIGH);
           delayMicroseconds(2000);
           digitalWrite(stepPin, LOW);
             delayMicroseconds(2000);
               } 
         }
        Serial.println("Tighten String"); 
        counter = 1; 
        currenttotal = 0; 
      
      }
        
      else if(avgtotal> stringFrequency) {
         if(stringFrequency<300) {
        digitalWrite(dirPin, HIGH);

          // Spin motor quickly
          for(int x = 0; x < stepsPerRevolution; x++)
              { 
            digitalWrite(stepPin, HIGH);
           delayMicroseconds(2000);
           digitalWrite(stepPin, LOW);
             delayMicroseconds(2000);
               }    
         }
         else {
           digitalWrite(dirPin, LOW);

          // Spin motor quickly
          for(int x = 0; x < stepsPerRevolution; x++)
              { 
            digitalWrite(stepPin, HIGH);
           delayMicroseconds(2000);
           digitalWrite(stepPin, LOW);
             delayMicroseconds(2000);
               }     
         }
        Serial.println("Loosen String"); 
        counter = 1; 
        currenttotal = 0; 

      }
      
  
   else {
    counter = 1;
    currenttotal = 0; 
   }
  }
  }
  }
  
}  
