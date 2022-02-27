// Specifically for use with the Adafruit Feather, the pins are pre-set here!
/*
 * sounds to get
 * AOL connect
 * model dialup
 * reggae sound
 * mac boot sound
 * windows 7 boot sound
 * 
 */
// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)

// Feather M4, M0, 328, ESP32S2, nRF52840 or 32u4
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
// DREQ should be an Int pin *if possible* (not possible on 32u4)
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

#define VBATPIN A6

// holds filenames for /base, /batt, /startup directories
String *baseSongList;
int nBaseSongs = 0;
String *battSongList;
int nBattSongs = 0;
String *startupSongList;
int nStartupSongs = 0;

const int unsetPin = 10;
const int setPin = 11;
const int buttonPin = 12; 
bool lightsOn = false;

Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

void flashEyes(int numFlashes, int flashDelay=200) {
  // flashes the eyes numFlashes times with flashDelay ms between
  for (int i=0; i<numFlashes; i++) {
      digitalWrite(setPin, HIGH);
      delay(10);
      digitalWrite(setPin, LOW);

      delay(flashDelay);

      digitalWrite(unsetPin, HIGH);
      delay(10);
      digitalWrite(unsetPin, LOW);

      delay(flashDelay);
  }
}

void listBaseSongs(){
  nBaseSongs = 0;
  File folder = SD.open("/base");
  while(true){
    File entry = folder.openNextFile();
    if(!entry){
      folder.rewindDirectory();
      break;
    }else{
    nBaseSongs++;
    }   
    entry.close();
  }

//  Serial.print("Songs found:");
//  Serial.println(nBaseSongs);

  baseSongList =new String[nBaseSongs];

//  Serial.println("Songs List:");

  for(int i = 0; i < nBaseSongs; i++){
    File entry = folder.openNextFile();
    baseSongList[i] = entry.name();
    entry.close();
//    Serial.println(baseSongList[i]);
  }
}

void listBattSongs(){
  nBattSongs = 0;
  File folder = SD.open("/batt");
  while(true){
    File entry = folder.openNextFile();
    if(!entry){
      folder.rewindDirectory();
      break;
    }else{
    nBattSongs++;
    }   
    entry.close();
  }

  Serial.print("Batt Songs found:");
  Serial.println(nBattSongs);

  battSongList =new String[nBattSongs];

  Serial.println("Songs List:");

  for(int i = 0; i < nBattSongs; i++){
    File entry = folder.openNextFile();
    battSongList[i] = entry.name();
    entry.close();
    Serial.println(battSongList[i]);
  }
}

void listStartupSongs(){
  nStartupSongs = 0;
  File folder = SD.open("/startup");
  while(true){
    File entry = folder.openNextFile();
    if(!entry){
      folder.rewindDirectory();
      break;
    }else{
    nStartupSongs++;
    }   
    entry.close();
  }

  Serial.print("Songs found:");
  Serial.println(nStartupSongs);

  startupSongList =new String[nStartupSongs];

  Serial.println("Songs List:");

  for(int i = 0; i < nStartupSongs; i++){
    File entry = folder.openNextFile();
    startupSongList[i] = entry.name();
    entry.close();
    Serial.println(startupSongList[i]);
  }
}
/// File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void setup() {

  pinMode(unsetPin, OUTPUT);
  pinMode(setPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);

  // Wait for serial port to be opened, remove this line for 'standalone' operation
//  while (!Serial) { delay(1); }
  delay(500);
//  Serial.println("\n\nAdafruit VS1053 Feather Test");
  
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }

  Serial.println(F("VS1053 found"));
 
//  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  
  // list files and build file name arrays
//printDirectory(SD.open("/"), 0);

listBaseSongs();
listBattSongs();
listStartupSongs();
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(8, 8);
  
#if defined(__AVR_ATmega32U4__) 
  // Timer interrupts are not suggested, better to use DREQ interrupt!
  // but we don't have them on the 32u4 feather...
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int
#else
  // If DREQ is on an interrupt pin we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
#endif
//  
//   flashEyes(5, 200);
  for (int i = 1; i<6; i++) {
      digitalWrite(setPin, HIGH);
      delay(10);
      digitalWrite(setPin, LOW);

      delay(i * 50);

      digitalWrite(unsetPin, HIGH);
      delay(10);
      digitalWrite(unsetPin, LOW);

      delay(i * 50);
  }
  
  playStartupSong();
}

void playBaseSong() {
  // play random base song
  /*
   * lots of code duplication.. how to pass 
   */
  int songNo = random(nBaseSongs);
  String filePath = ("/base/" + baseSongList[songNo]);
  int filePathLen = filePath.length() + 1;
  char char_array[filePathLen];
  filePath.toCharArray(char_array, filePathLen);

  Serial.println(char_array);
  musicPlayer.playFullFile(char_array);
}

void playBattSong() {
  // play random low battery  song
  int songNo = int(random(nBattSongs));
  String filePath = ("/batt/" + battSongList[songNo]);
  int filePathLen = filePath.length() + 1;
  char char_array[filePathLen];
  filePath.toCharArray(char_array, filePathLen);

  Serial.println(char_array);
  musicPlayer.playFullFile(char_array);
}

void playStartupSong() {
  // play random base song
//  int songNo = random(nStartupSongs);
//  String filePath = ("/startup/" + startupSongList[songNo]);
//  int filePathLen = filePath.length() + 1;
//  char char_array[filePathLen];
//  filePath.toCharArray(char_array, filePathLen);
//
//  Serial.println(char_array);
  musicPlayer.playFullFile("/startup/MacBoot.mp3");
}

bool lowBattery() {
      // check and print battery voltage
    float measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    Serial.print("VBat: " ); Serial.println(measuredvbat);
    if (measuredvbat < 3.6) {
      return true;
    } else {
      return false;
    }
}
void loop() {
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("button pressed!");
    flashEyes(5, 50);
    if (lowBattery()) {
      playBattSong();
    }else {
      playBaseSong();
    }
    delay(500);
  }
  delay(40);
}
