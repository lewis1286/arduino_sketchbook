#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  SerialUSB.println("------------------------------------");
  SerialUSB.print  ("Sensor:       "); SerialUSB.println(sensor.name);
  SerialUSB.print  ("Driver Ver:   "); SerialUSB.println(sensor.version);
  SerialUSB.print  ("Unique ID:    "); SerialUSB.println(sensor.sensor_id);
  SerialUSB.print  ("Max Value:    "); SerialUSB.print(sensor.max_value); SerialUSB.println(" m/s^2");
  SerialUSB.print  ("Min Value:    "); SerialUSB.print(sensor.min_value); SerialUSB.println(" m/s^2");
  SerialUSB.print  ("Resolution:   "); SerialUSB.print(sensor.resolution); SerialUSB.println(" m/s^2");
  SerialUSB.println("------------------------------------");
  SerialUSB.println("");
  delay(500);
}

void setup(void)
{
    #ifndef ESP8266
    while (!SerialUSB);     // will pause Zero, Leonardo, etc until serial console opens*/
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

  /* Display some basic information on this sensor */
  displaySensorDetails();
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  SerialUSB.print("X: "); SerialUSB.print(event.acceleration.x); SerialUSB.print("  ");
  SerialUSB.print("Y: "); SerialUSB.print(event.acceleration.y); SerialUSB.print("  ");
  SerialUSB.print("Z: "); SerialUSB.print(event.acceleration.z); SerialUSB.print("  ");SerialUSB.println("m/s^2 ");

  /* Note: You can also get the raw (non unified values) for */
  /* the last data sample as follows. The .getEvent call populates */
  /* the raw values used below. */
  //SerialUSB.print("X Raw: "); SerialUSB.print(accel.raw.x); SerialUSB.print("  ");
  //SerialUSB.print("Y Raw: "); SerialUSB.print(accel.raw.y); SerialUSB.print("  ");
  //SerialUSB.print("Z Raw: "); SerialUSB.print(accel.raw.z); SerialUSB.println("");

  /* Delay before the next sample */
  delay(1000);
}
