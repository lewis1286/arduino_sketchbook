#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

#include "Adafruit_ZeroFFT.h"

//the signal in signal.h has 2048 samples. Set this to a value between 16 and 2048 inclusive.
//this must be a power of 2
#define DATA_SIZE 256

#define DL 1 // Duration of delay within loop
//sample rate - with DL = 1, takes 2ms per loop, so FS = 1/.002 = 500 Hz
#define FS 500

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
#define NS 10 // Number of samples to take in for a fft batch


void setup()
{

  #ifndef ESP8266
    while (!SerialUSB);     // will pause until serial opens
  #endif
  SerialUSB.begin(9600);
  SerialUSB.println("Accelerometer Test"); SerialUSB.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    SerialUSB.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }

}

float get_vector_len(float vector[]){
    float leng;
    leng = sqrt(
            sq(vector[0]) +
            sq(vector[1]) +
            sq(vector[2])
    );
    return leng;
}

void get_direction(float direct[3], float accel_values[3], float leng){
    for(int i=0; i<3; i++){
        direct[i] = accel_values[i] / leng;
    }
}

void read_imu(float &x, float &y, float &z){
    /* Read in Accelerometer metrics, return the length and direction */
    /* Get a new sensor event */
    sensors_event_t event;
    accel.getEvent(&event);

    /* get the x,y,z coordinates of the accelerometer */
    x = event.acceleration.x;
    y = event.acceleration.y;
    z = event.acceleration.z;
}

float get_cos_sim(float initial[3], float finall[3]){
    /* compute cosine similarity of two vectors */

    float similarity;
    float len_i = get_vector_len(initial);
    float len_f = get_vector_len(finall);
    similarity = (
        finall[0] * initial[0] +
        finall[1] * initial[1] +
        finall[2] * initial[2]
    ) / (len_i * len_f);
    return similarity;
}

void shape_array(float float_array[NS], q15_t int_array[NS]){
    /*cos array is in [-1,1] but we need it in ints for FFT*/
    for(int i=0; i<NS; i++){
        float temp = ((-1 * float_array[i]) + 1)* 1000;
//        float temp = float_array[i] * 1000;
        int_array[i] = (int) temp;
    }
}

void loop()
{

    float lens[NS];
    float cossim[NS];
    q15_t intcossim[NS];
    float old_vector[3] = {0.57735, 0.57735, 0.57735}; // unit vector
    float new_vector[3] = {0.0, 0.0, 0.0};
    float leng;
    float start_time = micros();

    float time_delta;
    // build array of sensor readings within loop, then manipulate outside
    float imu_readings[NS][3];


    for(int i=0; i<NS; i++){
        read_imu(
            imu_readings[i][0],
            imu_readings[i][1],
            imu_readings[i][2]
        );
        delay(4);

    }
        /*compute cosine similarity between new direction and old */
        /*if(i==0){cossim[i] = 1;}*/
        /*else {*/
            /*cossim[i] = get_cos_sim(*/
                /*old_vector,*/
                /*new_vector*/
            /*);*/
        /*}*/
        /*// copy save new_direction*/
        /*for(int j = 0; j < 3; ++j) {*/
            /*old_vector[j] = new_vector[j];*/
        /*}*/

        /*sleep*/
        /*delay(DL);*/
    float end_time = micros();
    time_delta = end_time - start_time;
    SerialUSB.print("Run-time: ");
    SerialUSB.println(time_delta / NS);
    for(int i=0; i<NS;i++){
      for(int j=0; j<3;j++){
        SerialUSB.print(imu_readings[i][j]);
        SerialUSB.print(", ");
      }
      SerialUSB.println("");
    }
    SerialUSB.println("");
    /*shape_array(cossim, intcossim);*/
    /*[> Perform FFT on directions <]*/
    /*ZeroFFT(intcossim, DATA_SIZE);*/
//    plot_fft(intcossim);

    delay(5000);
}

void print_fft(q15_t data[]){
  for(int i=2; i<DATA_SIZE/2; i++){
    
    //print the frequency
    SerialUSB.print(FFT_BIN(i, FS, DATA_SIZE));
    SerialUSB.print(" Hz: ");

    //print the corresponding FFT output
    SerialUSB.println(data[i]);
  }
}
void plot_fft(q15_t data[]){
  for(int i=2; i<DATA_SIZE/2; i++){
    //print the corresponding FFT output
    SerialUSB.println(data[i]);
  }
}
