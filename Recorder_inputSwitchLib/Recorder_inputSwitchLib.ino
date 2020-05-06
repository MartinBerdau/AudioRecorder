// Record sound as raw data to a SD card, and play it back.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// Four pushbuttons need to be connected:
//   Record Button: pin 0 to GND
//   Stop Button:   pin 1 to GND
//   Play Button:   pin 2 to GND
//   Input-Switch Button: pin 3 to GND
//
// This example code is in the public domain.

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <WaveHeader.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=105,63
AudioAnalyzePeak         peak1;          //xy=278,108
AudioAnalyzePeak         peak2;          
AudioRecordQueue         queueInt;         //xy=281,63
AudioRecordQueue         queueExt;
AudioPlaySdRaw           playWav1;       //xy=302,157
AudioOutputI2S           i2s1;           //xy=470,120
AudioConnection          patchCord1(i2s2, 0, queueInt, 0);
AudioConnection          patchCord2(i2s2, 0, peak1, 0);
AudioConnection          patchCord3(i2s2, 1, queueExt, 0);  
AudioConnection          patchCord4(i2s2, 1, peak2, 0);
AudioConnection          patchCord5(playWav1, 0, i2s1, 0); //3
AudioConnection          patchCord6(playWav1, 0, i2s1, 1); //4 

AudioControlSGTL5000     sgtl5000_1;     //xy=265,212
// GUItool: end automatically generated code

// For a stereo recording version, see this forum thread:
// https://forum.pjrc.com/threads/46150?p=158388&viewfull=1#post158388

// A much more advanced sound recording and data logging project:
// https://github.com/WMXZ-EU/microSoundRecorder
// https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup
// https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop =   Bounce(1, 8);  // 8 = 8 ms debounce time
Bounce buttonPlay =   Bounce(2, 8);
Bounce buttonSwitch = Bounce(3, 8);  // Schaltet zwischen internem und externem Mikrofon um

// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playing
int switchMode = 0;               // 0 = internes Mikrofon, 1 = externes

// The file where data is recorded
File frec;

WaveHeader waveheader;

// Speichert Datei-Länge für Wave-Header
unsigned long recByteSaved = 0L;

void setup() {
  // Configure the pushbutton pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  
  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(256);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.7);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}


void loop() {
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();
  buttonSwitch.update();
  
  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == 2) stopPlaying();
    if (mode == 0) startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1) stopRecording();
    if (mode == 2) stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1) stopRecording();
    if (mode == 0) startPlaying();
  }
  if(buttonSwitch.fallingEdge()) {
    Serial.print("Input Switched To ");
    if (switchMode == 0) {
      switchMode = 1;                             // intern zu extern
      Serial.println("external");
      }      
    else {
      switchMode = 0;
      Serial.println("internal");                 // extern zu intern
      }                       
  }
  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }


}


void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.WAV")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("RECORD.WAV");
  }
  frec = SD.open("RECORD.WAV", FILE_WRITE);
  if (frec) {
    queueInt.begin();
    queueExt.begin();
    mode = 1;
    recByteSaved = 0L;
  }
}
    
void continueRecording() {
  if (switchMode == 0) {
    if (queueInt.available() >= 2) {
      byte bufferIntern[512];
      
      memcpy(bufferIntern, queueInt.readBuffer(), 256);
      queueInt.freeBuffer();
      memcpy(bufferIntern+256, queueInt.readBuffer(), 256);
      queueInt.freeBuffer(); 
      
      frec.write(bufferIntern, 512);

      recByteSaved += 512;                        // Addiert in jedem Durchlauf 512 Bytes auf die File-Länge (für den Wave-Header)
    }
  }
  if (switchMode == 1) {
    if (queueExt.available() >= 2) {
      byte bufferExtern[512];

      memcpy(bufferExtern, queueExt.readBuffer(), 256);
      queueExt.freeBuffer(); 
      memcpy(bufferExtern+256, queueExt.readBuffer(), 256);
      queueExt.freeBuffer();

      frec.write(bufferExtern, 512);

      recByteSaved += 512;                   // Addiert in jedem Durchlauf 512 Bytes auf die File-Länge (für den Wave-Header)
    }
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queueInt.end();
  queueExt.end();
  if (mode == 1) {
    while (queueInt.available() > 0 && queueExt.available() > 0) {
      if (switchMode == 0) {
      frec.write((byte*)queueInt.readBuffer(), 256); }

      if (switchMode == 1) {
      frec.write((byte*)queueExt.readBuffer(), 256); }    
      
      queueInt.freeBuffer();
      queueExt.freeBuffer();
      
      recByteSaved += 256;                                        // Addiert die letzten 256 Bytes bei Aufnahme-Stopp
      }
    waveheader.writeWaveHeader(recByteSaved, frec);                                            // Schreibt den Wave-Header auf die SD-Karte
    frec.close();
  }
  mode = 0;
}


void startPlaying() {
  Serial.println("startPlaying");
  playWav1.play("RECORD.WAV");
  mode = 2;
}

void continuePlaying() {
  if (!playWav1.isPlaying()) {
    playWav1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playWav1.stop();
  mode = 0;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
