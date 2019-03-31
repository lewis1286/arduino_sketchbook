#include <Arduino.h>
#line 1 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
#line 1 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

#include "Adafruit_ZeroFFT.h"
#include "signal.h"

//the signal in signal.h has 2048 samples. Set this to a value between 16 and 2048 inclusive.
//this must be a power of 2
#define DATA_SIZE 256

//the sample rate
#define FS 8000

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);


const int NS = 100; // Number of samples to take in for a fft batch (?global?)
const int DL = 5000; // Duratoin of delay within loop

#line 22 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void setup();
#line 40 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
float get_vector_len(float leng, float accel_values[]);
#line 51 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void get_direction(float direct[3], float accel_values[3], float leng);
#line 58 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void read_imu(float accel_values[3]);
#line 71 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void get_cos_sim(float sim, float i[3], float f[3], float len_i, float len_f);
#line 77 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void shape_array(float float_array[NS], int16_t int_array[NS]);
#line 85 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void loop();
#line 22 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
void setup()
{
  #ifndef ESP8266
    while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif
  Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }

}

float get_vector_len(float leng, float accel_values[]){
    leng = sqrt(
            sq(accel_values[0]) +
            sq(accel_values[1]) +
            sq(accel_values[2])
    );
    SerialUSB.print("leng: ");
    SerialUSB.println(leng);
    return leng;
}

void get_direction(float direct[3], float accel_values[3], float leng){

    direct[0] = accel_values[0] / leng;
    direct[1] = accel_values[1] / leng;
    direct[2] = accel_values[2] / leng;
}

void read_imu(float accel_values[3]){
    /* Read in Accelerometer metrics, return the length and direction */
    /* Get a new sensor event */
    sensors_event_t event;
    accel.getEvent(&event);

    /* 1. get the x,y,z coordinates of the accelerometer */

    accel_values[0] = event.acceleration.x;
    accel_values[1] = event.acceleration.y;
    accel_values[2] = event.acceleration.z;
}

void get_cos_sim(float sim, float i[3], float f[3], float len_i, float len_f){
    /* compute cosine similarity of two vectors */
    // int x = 1;
    sim = (f[0] * i[0] + f[1] * i[1] + f[2] * i[2]) / (len_i * len_f);
}

void shape_array(float float_array[NS], q15_t int_array[NS]){
    /*cos array is in [-1,1] but we need it in ints for FFT*/
    for(int i=0; i<NS; i++){
        float temp = float_array[i] * 1000;
        int_array[i] = (int) temp;
    }
}

void loop()
{

    float lens[NS];
    float cossim[NS];
    q15_t intcossim[NS];
    float accel_values[3];
    float sim;
    float old_direction[3];
    float new_direction[3];
    float old_length = 0;
    float new_length;
    float leng;

    for(int i=0; i<3; i++){old_direction[i] = 0.0;}
    for(int i=0; i<3; i++){new_direction[i] = 0.0;}
    SerialUSB.print("new direction: :) ");
    SerialUSB.println(new_direction[0]);
    for(int i=0; i<NS; i++){
        read_imu(accel_values);
        SerialUSB.print("new length");
        SerialUSB.println(leng);
        /* save length into array of lengths */
        /* lengths are not being used but for cossims, what to do w them? */
        leng = get_vector_len(leng, accel_values);

        SerialUSB.print("new direction");
        SerialUSB.println(new_direction[0]);
        get_direction(new_direction, accel_values, new_length);
        SerialUSB.print("new direction: ");
        SerialUSB.println(new_direction[0]);
        /* compute cosine similarity between new direction and old */
        if(i==0){
            cossim[i] = 0;
            // dirs[i] = 0;
        }
        else {
            get_cos_sim(
                sim,
                old_direction,
                new_direction,
                old_length,
                new_length
            );
            cossim[i] = sim;
        }
        /*sleep by some variables*/
        old_direction[0] = new_direction[0];
        old_direction[1] = new_direction[1];
        old_direction[2] = new_direction[2];
        old_length = new_length;

        delay(DL);
    }
    shape_array(cossim, intcossim);
    for(int i=0; i<10; i++){
        SerialUSB.print(cossim[i]);
    }
    SerialUSB.println(' ');

    for(int i=0; i<10; i++){
        SerialUSB.print(intcossim[i]);
    }
    SerialUSB.println(' ');
    /* Perform FFT on directions */
    ZeroFFT(intcossim, DATA_SIZE);
    /* Print some output to serial monitor */
    for(int i=0; i<10; i++){
        SerialUSB.print(intcossim[i]);
    }
    SerialUSB.println(' ');
}

