# 1 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
# 1 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino"
# 2 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino" 2
# 3 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino" 2
# 4 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino" 2

# 6 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino" 2
# 7 "/home/lewis/Arduino/vibration_measurement/vibration_measurement.ino" 2

//the signal in signal.h has 2048 samples. Set this to a value between 16 and 2048 inclusive.
//this must be a power of 2


//the sample rate


/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);


const int NS = 10; // Number of samples to take in for a fft batch (?global?)
const int DL = 50; // Duratoin of delay within loop

void setup()
{

    while (!Serial); // will pause Zero, Leonardo, etc until serial console opens

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

float get_vector_len(float vector[]){
    float leng;
    leng = sqrt(
            ((vector[0])*(vector[0])) +
            ((vector[1])*(vector[1])) +
            ((vector[2])*(vector[2]))
    );
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

void get_cos_sim(float sim, float i[3], float f[3]){
    /* compute cosine similarity of two vectors */
    float len_i = get_vector_len(i);
    float len_f = get_vector_len(f);
    sim = (f[0] * i[0] + f[1] * i[1] + f[2] * i[2]) / (len_i * len_f);
}

void shape_array(float float_array[NS], int16_t int_array[NS]){
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
    int16_t intcossim[NS];
    float accel_values[3];
    float sim;
    float old_direction[3];
    float new_direction[3];
    float leng;

    for(int i=0; i<3; i++){old_direction[i] = 0.0;}
    for(int i=0; i<3; i++){new_direction[i] = 0.0;}

    for(int i=0; i<NS; i++){
        read_imu(accel_values);
        /* save length into array of lengths */
        /* lengths are not being used but for cossims, what to do w them? */
        leng = get_vector_len(accel_values);

        /*SerialUSB.print("old direction: ");*/
        /*SerialUSB.print(old_direction[0]);*/
        /*SerialUSB.print(' ');*/
        /*SerialUSB.print(old_direction[1]);*/
        /*SerialUSB.print(' ');*/
        /*SerialUSB.println(old_direction[2]);*/
        get_direction(new_direction, accel_values, leng);
        /*SerialUSB.print("new direction: ");*/
        /*SerialUSB.print(new_direction[0]);*/
        /*SerialUSB.print(' ');*/
        /*SerialUSB.print(new_direction[1]);*/
        /*SerialUSB.print(' ');*/
        /*SerialUSB.println(new_direction[2]);*/
        /* compute cosine similarity between new direction and old */
        if(i==0){
            cossim[i] = 0;
            // dirs[i] = 0;
        }
        else {
            get_cos_sim(
                sim,
                old_direction,
                new_direction
            );
            cossim[i] = sim;
        }
        SerialUSB.print("coscim" );
        SerialUSB.println(cossim[i]);
        // copy save new_direction
        old_direction[0] = new_direction[0];
        old_direction[1] = new_direction[1];
        old_direction[2] = new_direction[2];

        /*sleep by some variables*/
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
    ZeroFFT(intcossim, 256);
    /* Print some output to serial monitor */
    for(int i=0; i<10; i++){
        SerialUSB.print(intcossim[i]);
    }
    SerialUSB.println(' ');
}
