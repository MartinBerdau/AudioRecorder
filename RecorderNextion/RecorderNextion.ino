// Master-Version
// Dieses File enthält bislang nur die Grundfunktionen.
// Sobald ein weiteres Feature effizient genug ausgearbeitet worden ist (idealerweise outsourcen in andere Files),
// kann es hier eingebaut werden. Am besten sollten aber alle davon wissen, was hier eingebaut wird, also
// idealerweise erst in der Besprechung erklären, wie das Feature zu nutzen ist.
//
// Es handelt sich um ein editiertes Beispiel-File!
// Die Eingaben sollen ueber ein Nextion-Display gemacht werden.
// Es wird zum Ausfuehren noch die Nextion-Library benoetigt.
// Es werden 3 Knöpfe auf dem Nextion erstellt und die Funktionen
// durch Callbacks (ganz untern zu finden) aufgerufen.
// Neben dem Beispielprojekt wurde das folgende Video verwendet:
// https://www.youtube.com/watch?v=mdkUBB60HoI
//
// Record sound as raw data to a SD card, and play it back.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// This example code is in the public domain.

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Nextion.h>

// eigene Files
#include <RMSLevel.h>
#include <WaveHeader.h>
#include <RunningTimeLabel.h>

//-----------------------------------------------------------------------------------------
// AUDIO
//-----------------------------------------------------------------------------------------
AudioInputI2S            i2s2 ;
AudioAnalyzePeak         peak1;
AudioAnalyzeRMS          rms_mono;
AudioRecordQueue         queue1;
AudioPlaySdWav           playWav1;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(i2s2, 0, queue1, 0);
AudioConnection          patchCord2(i2s2, 0, peak1, 0);
AudioConnection          patchCord3(playWav1, 0, i2s1, 0);
AudioConnection          patchCord4(playWav1, 0, i2s1, 1);
AudioConnection          patchCord6(i2s2, 0, i2s1, 0);
AudioConnection          patchCord7(i2s2, 0, i2s1, 1);
AudioConnection          patchCord5(i2s2, 0, rms_mono, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=265,212

//-----------------------------------------------------------------------------------------
// TIMING
//-----------------------------------------------------------------------------------------
elapsedMillis            TimerDisp;
elapsedMillis            TimePassed;

//-----------------------------------------------------------------------------------------
//BUTTONS
//-----------------------------------------------------------------------------------------
NexButton buttonRecord = NexButton(0,3,"Record");
NexButton buttonStop = NexButton(0,2,"Stop");
NexButton buttonPlay = NexButton(0,1,"Play");
NexProgressBar ProgBarLevel = NexProgressBar(0,10,"Pegel");
NexButton buttonCheckLvl = NexButton(0,12,"CheckLvl");
NexText textTimer = NexText(0,7,"Timer");

NexTouch *nex_listen_list[] =
{
  &buttonRecord,
  &buttonStop,
  &buttonPlay,
  &buttonCheckLvl,
  NULL
};

int myInput = AUDIO_INPUT_LINEIN;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

int mode = 0;  // 0=stopped, 1=recording, 2=playing

File frec;

//-----------------------------------------------------------------------------------------
// RUNNING TIME LABEL
//-----------------------------------------------------------------------------------------
char TimerVal[] = "00:00:00";
RunningTimeLabel tLabel;

//-----------------------------------------------------------------------------------------
// WAVE HEADER
//-----------------------------------------------------------------------------------------
WaveHeader wavHead;
unsigned long recByteSaved = 0L;

//-----------------------------------------------------------------------------------------
// RMS-METER
//-----------------------------------------------------------------------------------------
double tau = 0.125;
double f_refresh = 4;
RMSLevel rmsMeter(tau,f_refresh);
bool checkLvl = false;
unsigned int dispDelay = 1000/f_refresh;

//-----------------------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------------------
void setup() {

  nexInit();
  Serial7.print("baud=115200");
  Serial7.write(0xff);
  Serial7.write(0xff);
  Serial7.write(0xff);
  Serial7.end();
  Serial7.begin(115200);
  
  AudioMemory(256);

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // Link Button Callbacks
  buttonRecord.attachPush(RecordButtonCallback);
  buttonStop.attachPush(StopButtonCallback);
  buttonPlay.attachPush(PlayButtonCallback);
  buttonCheckLvl.attachPush(buttonCheckLvlCallback);
}

//-----------------------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------------------
void loop() {
  nexLoop(nex_listen_list);
  
  switch(mode){
    case 0:
      break;
    case 1:
      continueRecording();
      break;
    case 2:
      continuePlaying();
      break;
  }

  if (TimerDisp >=dispDelay){
    if(checkLvl){
      displayLvl();
    }
    
    if (mode == 1 /*|| mode == 2*/){
      tLabel.updateLabel(TimePassed,TimerVal);
      //Serial.println(TimerVal);
      textTimer.setText(TimerVal);
    }

    TimerDisp-=dispDelay;
  }
  
}

//-----------------------------------------------------------------------------------------
// BASIS FUNKTIONEN
//-----------------------------------------------------------------------------------------
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
    queue1.begin();
    mode = 1;
    recByteSaved = 0L;
    checkLvl = true;
    TimePassed = 0;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);

    recByteSaved += 512;   
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();

      recByteSaved += 256;  
    }
    wavHead.writeWaveHeader(recByteSaved, frec);
    //frec.close();
  }
  mode = 0;
  checkLvl = false;
}


void startPlaying() {
  /*
  Serial.println("startPlaying");
  playWav1.play("RECORD.WAV");
  mode = 2;
  */
}

void continuePlaying() {
  /*
  if (!playWav1.isPlaying()) {
    playWav1.stop();
    mode = 0;
  }
  */
}


void stopPlaying() {
  /*
  Serial.println("stopPlaying");
  if (mode == 2) playWav1.stop();
  mode = 0;
  */
}

//-----------------------------------------------------------------------------------------
// ANZEIGE FUNKTIONEN
//-----------------------------------------------------------------------------------------
void displayLvl() {
  uint32_t ProgBarVal = uint32_t(100*(1-(rmsMeter.updateRMS(double(rms_mono.read()))/-80)));
  Serial.println(ProgBarVal);
  ProgBarLevel.setValue(ProgBarVal); 
}

//-----------------------------------------------------------------------------------------
// PUSH CALLBACKS
//-----------------------------------------------------------------------------------------
// für Basis-Funktionen
void RecordButtonCallback(void *ptr)
{
  Serial.print("Record");
  switch(mode){
    case 0:
      startRecording();
      TimerDisp=0;
      break;
    case 1:
      stopRecording();
      break;
    case 2:
      break;
  }
}

void StopButtonCallback(void *ptr)
{
  Serial.println("Stop Button Press");
  switch(mode){
    case 0:
      break;
    case 1:
      stopRecording();
      break;
    case 2:
      stopPlaying();
      break;
  }
  checkLvl = false;
}

void PlayButtonCallback(void *ptr)
{
  Serial.println("Play Button Press");
  switch(mode){
    case 0:
      startPlaying();
      break;
    case 1:
      startPlaying();
      break;
    case 2:
      break;
  }
}

void buttonCheckLvlCallback(void *ptr)
{
  if (!checkLvl)
  {
    Serial.println("Checking Level");
    TimerDisp = 0;
    checkLvl = true;
  }
  else
  {
    Serial.println("Stop Checking Level");
    checkLvl = false;
  }
  
}
//-----------------------------------------------------------------------------------------
