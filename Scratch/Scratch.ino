/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

// digital pin 2 has a pushbutton attached to it. Give it a name:

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  SerialUSB.begin(9600);
  // make the pushbutton's pin an input:
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  float a = 1.04;
  SerialUSB.println(a);
  float b = 3.02;
  SerialUSB.println(b);
  float c = a * b;
  SerialUSB.println(c);
  delay(5000);        // delay in between reads for stability
}
