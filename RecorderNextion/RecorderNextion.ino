/*
Copyright 2020 <Martin Berdau, Tammo Sander>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


<Main File of the Project, containing little Functions, which weren't outsourced, 
as well as functions to comunicate with the Nextion Display.
This code ist based on the Recorder-Example from the Audio-library.>
*/


// QUELLE: TIMESTAMP: https://forum.arduino.cc/index.php?topic=348562.0
//
// Editiertes Beispiel-File.
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
// For Saving stuff the listfiles eample was used
//
// This example code is in the public domain.


#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Nextion.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

// own Files
#include <RMSLevel.h>
#include <WaveHeader.h>
#include <RunningTimeLabel.h>
#include <FileBrowser.h>
#include <thirdOctAnalyze.h>
#include <AutomaticGainControl.h>
#include <AGC.h>

//-----------------------------------------------------------------------------------------
// AUDIO OBJECTS
//-----------------------------------------------------------------------------------------
AudioInputI2S            AudioInput;
AudioAnalyzePeak         peak1;
AudioAnalyzePeak         peak2;
AudioRecordQueue         queue1;
AudioPlaySdWav           playSdWav1;
AudioOutputI2S           AudioOutput;
AudioMixer4              mixer1;
AudioAnalyzeFFT1024      fft;
AudioAmplifier           amp1;
AudioControlSGTL5000     sgtl5000_1;

//-----------------------------------------------------------------------------------------
// CONNECTIONS
//-----------------------------------------------------------------------------------------
AudioConnection          patchCord1(AudioInput, 1, amp1, 0);
AudioConnection          patchCord2(amp1, queue1);
AudioConnection          patchCord3(amp1, peak1);
AudioConnection          patchCord4(amp1, peak2);
AudioConnection          patchCord5(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord6(playSdWav1, 1, mixer1, 1);
AudioConnection          patchCord7(amp1, 0, mixer1, 2);
AudioConnection          patchCord8(amp1, 0, mixer1, 3);
AudioConnection          patchCord9(mixer1, 0, AudioOutput, 0);
AudioConnection          patchCord10(mixer1, 0, AudioOutput, 1);
AudioConnection          patchCord11(amp1, fft);

//-----------------------------------------------------------------------------------------
// TIMING
//-----------------------------------------------------------------------------------------
elapsedMillis            TimerDisp;
elapsedMillis            TimerFFT;
elapsedMillis            TimePassed;
elapsedMillis            TimerAGC;


//-----------------------------------------------------------------------------------------
// BUTTONS
//-----------------------------------------------------------------------------------------
// Buttons used on the recording screen
NexButton buttonRecord = NexButton(0, 3, "Record");
NexButton buttonStop = NexButton(0, 2, "Stop");
NexButton buttonPlay = NexButton(0, 1, "Play");
NexButton buttonSave = NexButton(0, 9, "Save");
NexButton buttonCheckLvl = NexButton(0, 12, "CheckLvl");
NexButton buttonVolUp = NexButton(0, 19, "VolUp");
NexButton buttonVolDown = NexButton(0, 20, "VolDown");
NexButton buttonMute = NexButton(0, 21, "Mute");
NexSlider sliderGain = NexSlider(0, 11, "Gain");
NexProgressBar ProgBarLevel = NexProgressBar(0, 10, "Pegel");
NexText textTimer = NexText(0, 7, "Timer");
NexText textFile = NexText(0, 8, "FileName");
NexText textAvailable = NexText(0, 13, "Verfg");

// Elements used in the file browser
NexText textWavFile = NexText(2, 1, "WavFile");
NexText textWavSize = NexText(2, 10, "WavSize");
NexText textWavLen = NexText(2, 12, "WavLen");
NexText textWavTimer = NexText(2, 15, "Timer");
NexButton buttonPlayWav = NexButton(2, 2, "PlayWav");
NexButton buttonStopWav = NexButton(2, 3, "WavStop");
NexButton buttonWavUp = NexButton(2, 4, "WavUp");
NexButton buttonWavDown = NexButton(2, 5, "WavDown");

// Menu buttons
NexButton buttonRecorder = NexButton(1, 3, "Recorder");
NexButton buttonPlayer = NexButton(1, 2, "FileBrowser");
NexButton buttonSpectrum = NexButton(1, 7, "Spectrum");
NexButton buttonAGCSet = NexButton(1, 6, "AGCSettings");

// Buttons recorder settings
NexButton buttonEQSettings = NexButton(3, 8, "EQSettings");
NexButton buttonEQReset = NexButton(3, 9, "EQReset");

// Elements used in the Equalizer
NexSlider EQf0Slider = NexSlider(5, 2, "EQSlider0");
NexButton EQf0ButP = NexButton(5, 8, "EQf0p");
NexButton EQf0ButM = NexButton(5, 18, "EQf0m");
NexNumber EQf0Val = NexNumber(5, 13, "Valf0");
NexSlider EQf1Slider = NexSlider(5, 3, "EQSlider1");
NexButton EQf1ButP = NexButton(5, 9, "EQf1p");
NexButton EQf1ButM = NexButton(5, 19, "EQf1m");
NexNumber EQf1Val = NexNumber(5, 14, "Valf1");
NexSlider EQf2Slider = NexSlider(5, 4, "EQSlider2");
NexButton EQf2ButP = NexButton(5, 10, "EQf2p");
NexButton EQf2ButM = NexButton(5, 20, "EQf2m");
NexNumber EQf2Val = NexNumber(5, 15, "Valf2");
NexSlider EQf3Slider = NexSlider(5, 5, "EQSlider3");
NexButton EQf3ButP = NexButton(5, 11, "EQf3p");
NexButton EQf3ButM = NexButton(5, 21, "EQf3m");
NexNumber EQf3Val = NexNumber(5, 16, "Valf3");
NexSlider EQf4Slider = NexSlider(5, 6, "EQSlider4");
NexButton EQf4ButP = NexButton(5, 12, "EQf4p");
NexButton EQf4ButM = NexButton(5, 22, "EQf4m");
NexNumber EQf4Val = NexNumber(5, 17, "Valf4");

// Items used in the Spectrum-Analyzer
NexProgressBar f0Band = NexProgressBar(6, 2, "f0FFT");
NexProgressBar f1Band = NexProgressBar(6, 3, "f1FFT");
NexProgressBar f2Band = NexProgressBar(6, 4, "f2FFT");
NexProgressBar f3Band = NexProgressBar(6, 5, "f3FFT");
NexProgressBar f4Band = NexProgressBar(6, 6, "f4FFT");
NexProgressBar f5Band = NexProgressBar(6, 7, "f5FFT");
NexProgressBar f6Band = NexProgressBar(6, 8, "f6FFT");
NexProgressBar f7Band = NexProgressBar(6, 9, "f7FFT");
NexProgressBar f8Band = NexProgressBar(6, 10, "f8FFT");
NexProgressBar f9Band = NexProgressBar(6, 11, "f9FFT");
NexProgressBar f10Band = NexProgressBar(6, 12, "f10FFT");
NexProgressBar f11Band = NexProgressBar(6, 13, "f11FFT");
NexProgressBar f12Band = NexProgressBar(6, 14, "f12FFT");
NexProgressBar f13Band = NexProgressBar(6, 15, "f13FFT");
NexProgressBar f14Band = NexProgressBar(6, 16, "f14FFT");
NexProgressBar f15Band = NexProgressBar(6, 17, "f15FFT");
NexNumber AveBlocks = NexNumber(6, 23, "ValAve");
NexButton ApplyAve = NexButton(6, 24, "ApplyAve");
NexButton SpecMenu = NexButton(6, 19, "Menu");

// Elements used for the AGC settings
NexButton buttonAGC = NexButton(0, 16, "AGC");
NexSlider sliderSetPoint = NexSlider(4,4,"SliderSetPoint");
NexSlider sliderRange = NexSlider(4,7,"SliderRange");
NexSlider sliderReact = NexSlider(4,10,"SliderReact");
NexSlider sliderRatio = NexSlider(4,17,"SliderRatio");

// Elements used for date settings
NexButton buttonApplyD = NexButton(7, 16, "ApplyD");
NexButton buttonApplyT = NexButton(7, 28, "ApplyT");
NexNumber NumDay = NexNumber(7, 5, "Day");
NexNumber NumMonth = NexNumber(7, 7, "Month");
NexNumber NumYear = NexNumber(7, 9, "Year");
NexNumber NumHour = NexNumber(7, 17, "Hour");
NexNumber NumMin = NexNumber(7, 19, "Min");
NexNumber NumSec = NexNumber(7, 21, "Sec");


// button list
NexTouch *nex_listen_list[] =
{
  &buttonRecord,
  &buttonStop,
  &buttonPlay,
  &buttonSave,
  &buttonCheckLvl,
  &buttonVolUp,
  &buttonVolDown,
  &buttonMute,
  &buttonPlayWav,
  &buttonStopWav,
  &buttonWavUp,
  &buttonWavDown,
  &sliderGain,
  &buttonRecorder,
  &buttonPlayer,
  &buttonSpectrum,
  &buttonEQSettings,
  &buttonAGCSet,
  &buttonEQReset,
  &EQf0Slider,
  &EQf0ButP,
  &EQf0ButM,
  &EQf1Slider,
  &EQf1ButP,
  &EQf1ButM,
  &EQf2Slider,
  &EQf2ButP,
  &EQf2ButM,
  &EQf3Slider,
  &EQf3ButP,
  &EQf3ButM,
  &EQf4Slider,
  &EQf4ButP,
  &EQf4ButM,
  &ApplyAve,
  &SpecMenu,
  &buttonAGC,
  &sliderSetPoint,
  &sliderRange,
  &sliderReact,
  &sliderRatio,
  &buttonApplyD,
  &buttonApplyT,
  NULL
};

//-----------------------------------------------------------------------------------------
// GENERAL SETTINGS
//-----------------------------------------------------------------------------------------
int inputMode = 1; //Input mode: 0 = internal mic, 1 = external mic , 2 = line-in
int myInput = AUDIO_INPUT_LINEIN; //set line input as used input

//define pins connecting teensy 4.0 and Audio shield
#define SDCARD_CS_PIN    10 
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

int mode = 0;  //recording mode: 0=stopped, 1=recording, 2=playing
double InGain = 1.0; //input gain (gain = 1.0 (bypass), gain < 1.0 (attenuation), gain > 1.0 (amplification)
double HpVol = 0.5; //Headphone volume
bool muted = false;

// file where data is recorded
File frec;

//-----------------------------------------------------------------------------------------
// FILE BROWSER
//-----------------------------------------------------------------------------------------
//variables used on main recording screen
FileBrowser filebrowser; //create object to use file browser
int fileCount = 0; //Number of files on the sd card
char filename[] = "RECORD01.WAV"; //standard file name
char lastSave[] = "RECORD01.WAV"; //copy of standard file name
char MemoryDisp[] = "16,00 GB / 00:00:00"; //char array to display available Memory and recording time
uint32_t availableMemory_byte = 0; //available Memory in bytes
uint32_t availableTime_sec = 0; //available recording time in sceonds
uint32_t usedMemory = 0;  //used memory in bytes
Sd2Card card; //sd card object
SdVolume volume; //sd volume object
File root; //root file to compute used memory
double volumesize = 0.0; //volume size in bytes
unsigned long long SDSize = 0; //sd card size
bool saved = false; //bool to check if file is already saved

//variables used on file browser screen
char CurWav[] = "RECORD01.WAV"; //current file name
char WavSizeChar[] = "00.00 GB"; //current file size
char WavLenChar[] = "00:00:00"; //current file length
int WavCount = 0; //counter which file is selected
File WavFile; //file to open current file


//-----------------------------------------------------------------------------------------
// WAVE HEADER
//-----------------------------------------------------------------------------------------
//variables used to write the wave header
WaveHeader waveheader;
unsigned long recByteSaved = 0L; //saved bytes
unsigned long bytesPerSample = 2L; //Bytes per sample

//-----------------------------------------------------------------------------------------
// RMS-METER
//----------------------------------------------------------------------------------------
//Variables used in the RMS-Meter
double tau = 0.125; //time constant in seconds (125 ms = fast)
double f_refresh = 4; //refreshrate in Hertz
RMSLevel rmsMeter(tau, f_refresh); 
bool checkLvl = false; //bool if level shall be checked
unsigned int dispDelay = 1000 / f_refresh; //refresh rate of displaying level

//-----------------------------------------------------------------------------------------
// AUTOMATIC GAIN CONTROL
//-----------------------------------------------------------------------------------------
//Variables used in the AGC
AutomaticGainControl agc; 
bool AGCOn = false;   //bool if level shall be checked
double peak; //peak level
double AGCgain; //linear gain 
double AGCthresh; //linear threshold

//initial settings for the AGC
uint32_t hangSetting = 3;
uint32_t slopeIncSetting = 2;
uint32_t slopeDecSetting = 2;
uint32_t threshSetting = 90;

//-----------------------------------------------------------------------------------------
// RUNNING TIME LABEL
//-----------------------------------------------------------------------------------------
//Variables used to display running time labels
RunningTimeLabel tLabel;
char TimerVal[] = "00:00:00";

//-----------------------------------------------------------------------------------------
// EQ
//-----------------------------------------------------------------------------------------
//Variables used in the Equalizer, initalized with a flat 0 dB gain
double f0Gain = 0.0;
double f1Gain = 0.0;
double f2Gain = 0.0;
double f3Gain = 0.0;
double f4Gain = 0.0;

//-----------------------------------------------------------------------------------------
// SPECTRUM ANALYZER
//-----------------------------------------------------------------------------------------
//Variables for the spectrum analyzer
static const int nrOfBands = 16; //Number of displayed frequency bands
double dataVec[nrOfBands]; //Vector containing the data
double averages = 50; //averages
thirdOctAnalyze thirdOctValues(averages);
bool analyzeActiv = false; //bool if spectrum analyzer is active
unsigned int FFTupdate = 1000 / 20; //refreshrate of displaying spectrum
int bandCounter = 0; //counter which band is displayed

//-----------------------------------------------------------------------------------------
// DATE
//-----------------------------------------------------------------------------------------
//Variables used to set Date
char timestamp[30];
uint32_t Day = 1;
uint32_t Month = 1;
uint32_t Year = 2020;
uint32_t hours = 0;
uint32_t mins = 0;
uint32_t secs = 0;

//-----------------------------------------------------------------------------------------
// SETUP
//-----------------------------------------------------------------------------------------
//Setup function, which is called once at start of the programm
void setup() {
  //initialize Nextion display and set baud rate to 115200 Hz
  nexInit();
  Serial1.print("baud=115200");
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.end();
  Serial1.begin(115200);
  
  //set Audiomemory to 256 samples
  AudioMemory(256);
  
  //enable sgtl5000, audio preprocessor, set standard values to input and volume and select
  //Equalizer
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(HpVol);
  sgtl5000_1.audioPreProcessorEnable();
  sgtl5000_1.eqSelect(3);

  //set input gain to standard value (1.0)
  amp1.gain(InGain);

  //set Pins to connect Teensy and Audio shield
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  //check if sd card is enable
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // calculating sd card size
  // Source from Arduino IDE: Examples/SD/Cardinfo
  card.init(SPI_HALF_SPEED, SDCARD_CS_PIN);
  volume.init(card);
  SDSize = volume.blocksPerCluster();
  SDSize *= volume.clusterCount();
  volumesize = SDSize;
  SDSize *= 512;
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 2;
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.print("Volume size (Gbytes): ");
  volumesize /= 1024;

  //open file tree, compute used memory and check current file
  root = SD.open("/");
  computeUsedMemory(root);
  checkCurrentFile();

  // initial mixer settings
  SetRecordingInput();

  // link callbacks
  buttonRecord.attachPush(RecordButtonCallback);
  buttonStop.attachPush(StopButtonCallback);
  buttonPlay.attachPush(PlayButtonCallback);
  buttonSave.attachPush(SaveButtonCallback);
  buttonCheckLvl.attachPush(buttonCheckLvlCallback);
  sliderGain.attachPop(sliderGainCallback);
  buttonVolUp.attachPush(buttonVolUpCallback);
  buttonVolDown.attachPush(buttonVolDownCallback);
  buttonMute.attachPush(buttonMuteCallback);
  buttonPlayWav.attachPush(buttonPlayWavCallback);
  buttonStopWav.attachPush(buttonStopWavCallback);
  buttonWavUp.attachPush(buttonWavUpCallback);
  buttonWavDown.attachPush(buttonWavDownCallback);
  buttonRecorder.attachPush(buttonRecorderCallback);
  buttonEQSettings.attachPush(buttonEQSettingsCallback);
  buttonEQReset.attachPush(buttonEQResetCallback);
  buttonPlayer.attachPush(buttonPlayerCallback);
  buttonSpectrum.attachPush(buttonSpectrumCallback);
  buttonAGCSet.attachPush(buttonAGCSetCallback);
  EQf0Slider.attachPop(EQf0SliderCallback);
  EQf0ButP.attachPush(EQf0ButPCallback);
  EQf0ButM.attachPush(EQf0ButMCallback);
  EQf1Slider.attachPop(EQf1SliderCallback);
  EQf1ButP.attachPush(EQf1ButPCallback);
  EQf1ButM.attachPush(EQf1ButMCallback);
  EQf2Slider.attachPop(EQf2SliderCallback);
  EQf2ButP.attachPush(EQf2ButPCallback);
  EQf2ButM.attachPush(EQf2ButMCallback);
  EQf3Slider.attachPop(EQf3SliderCallback);
  EQf3ButP.attachPush(EQf3ButPCallback);
  EQf3ButM.attachPush(EQf3ButMCallback);
  EQf4Slider.attachPop(EQf4SliderCallback);
  EQf4ButP.attachPush(EQf4ButPCallback);
  EQf4ButM.attachPush(EQf4ButMCallback);
  ApplyAve.attachPush(ApplyAveCallback);
  SpecMenu.attachPush(SpecMenuCallback);
  buttonAGC.attachPush(AGCButtonCallback);
  sliderSetPoint.attachPop(sliderSetPointCallback);
  sliderRange.attachPop(sliderRangeCallback);
  sliderReact.attachPop(sliderReactCallback);
  sliderRatio.attachPop(sliderRatioCallback);
  buttonApplyD.attachPush(buttonApplyDCallback);
  buttonApplyT.attachPush(buttonApplyTCallback);
  
  //set global time to initial values
  setTime(hours, mins, secs, Day, Month, Year);
  SdFile::dateTimeCallback(dateTime);
}

//-----------------------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------------------
void loop() {
  // Respond to button presses
  nexLoop(nex_listen_list);

  // by checking which mode, record or play
  switch (mode) {
    case 0:
      break;
    case 1:
      continueRecording();
      break;
    case 2:
      continuePlaying();
      break;
  }

  //if diplay timer is greater than disp delay, display the current level
  if (TimerDisp >= dispDelay) {
    if (checkLvl) {
      displayLvl();
    }
    if (mode == 1 || mode == 2) {
      tLabel.updateLabel(TimePassed, TimerVal);
      textTimer.setText(TimerVal);
    }

    TimerDisp -= dispDelay;
  }

  //if fft timer is greater than fft update delay, display fft
  if (TimerFFT >= FFTupdate) {
    if (analyzeActiv) {
      UpdateFFTValue();
    }
    TimerFFT -= FFTupdate;
  }

  //if AGC timer is greater than AGC refresh delay, call AGC
  if (TimerAGC >= AGCRefresh)
  {
    if (AGCOn) {
      useAGC();
   }
   TimerAGC -= AGCRefresh;
  }
}

//-----------------------------------------------------------------------------------------
// BASIC FUNCTIONS
//-----------------------------------------------------------------------------------------
//functions to start continue and end recording, based on Example: RECORDER.ino from the Audio library by Paul Stoffregen
//Source: https://github.com/PaulStoffregen/Audio
void startRecording() {
  Serial.println("startRecording");
  //check if current file exists, if so remove it
  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  //open file
  frec = SD.open(filename, FILE_WRITE);
  if (frec) {
    queue1.begin(); //begin queue
    mode = 1; //set mode to recording
    checkLvl = true; //to display level
    recByteSaved = 0L; //reset file size
    TimePassed = 0; //reset timer
  }
}

//function to continue recording by witing bytes from queue to file
void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512]; //audio buffer
    memcpy(buffer, queue1.readBuffer(), 256); //write 256 samples from queue to buffer
    queue1.freeBuffer(); //erase samples from queue
    memcpy(buffer + 256, queue1.readBuffer(), 256); //write 256 samples from queue to end of buffer
    queue1.freeBuffer(); //erase samples from queue
    frec.write(buffer, 512); //write 512 samples from buffer to file
    recByteSaved += (512*bytesPerSample); //add 1024 bytes to file size
  }
}

//function to stop recording
void stopRecording() {
  Serial.println("stopRecording");
  queue1.end(); //end queue
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256); //write the last 256 samples to the file
      queue1.freeBuffer(); //erase samples from queue
      recByteSaved += (256*bytesPerSample); //add the last 512 bytes to file length                                      
    }
  }
  waveheader.writeWaveHeader(recByteSaved, frec); //write header to file
  mode = 0; //set mode back to 0
  checkLvl = false; //enable display level
  saved = false; //file can be overwritten
  TimePassed = 0; // reset timer
}

////functions to start, continue and end playing, based on Example: WavFilePlayer.ino from the Audio library by Paul Stoffregen
//Source: https://github.com/PaulStoffregen/Audio
void startPlaying() {
  Serial.println("startPlaying");
  TimePassed = 0; //reset timer
  //check if file was saved already, to choose which one shall be played
  if (saved == true)
  {
    playFile(lastSave);
    mode = 2;
    return;
  }
  else
  {
    playFile(filename);
    mode = 2;
    return;
  }

}

//function to play file
void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  playSdWav1.play(filename); //begin to play file
  delay(5); //a delay to read data properly
}

//function continue playing
void continuePlaying() {
  //check if the file is still playing, if not stop and change mode
  if (!playSdWav1.isPlaying()) {
    playSdWav1.stop();
    mode = 0;
  }
}

//function to stop playing
void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playSdWav1.stop(); //stop playing the file
  mode = 0; //set mode to 0
  TimePassed = 0; //reset timer
}

//-----------------------------------------------------------------------------------------
// DISPLAY FUNCTIONS
//-----------------------------------------------------------------------------------------
//functions used to display infos for the user

//function thats called in loop to refresh the display
void displayRefresh() {
  if (checkLvl) {
    displayLvl();
  }
}


void displayLvl() {
  uint32_t ProgBarVal = uint32_t(100 * (1 - (rmsMeter.updateRMS(double(peak1.read())) / -80)));
  ProgBarLevel.setValue(ProgBarVal);
  if (AGCOn) {
    uint32_t gainVal = uint32_t(20*log10(InGain))+12;
    sliderGain.setValue(gainVal);
  }
}

void updateMemoryDisp() {
  filebrowser.computeAvailableMemory(MemoryDisp, SDSize, usedMemory, availableMemory_byte, availableTime_sec);
  textFile.setText(filename);
  textAvailable.setText(MemoryDisp);
}

//-----------------------------------------------------------------------------------------
// FILE BROWSER FUNCTIONS
//-----------------------------------------------------------------------------------------
void checkCurrentFile()
{
  while (fileCount >= 0)
  {
    if (SD.exists(filename)) {
      fileCount += 1;
      filebrowser.computeCurName(filename, 1);
    }
    else {
      updateMemoryDisp();
      break;
    }
  }
}

void saveCurrentFile() {
  if (recByteSaved > 0) {
    strcpy(lastSave, filename);
    fileCount += 1;
    filebrowser.computeCurName(filename, 1);
    usedMemory += recByteSaved + 36;
    updateMemoryDisp();
    recByteSaved = 0;
    saved = true;
  }

}

void computeUsedMemory(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    usedMemory += entry.size();
    if (!entry) {
      usedMemory -= 8192;
      break;
    }
    entry.close();
  }
  updateMemoryDisp();
}

void updateFileBrowser() {
  textWavFile.setText(CurWav);
  WavFile = SD.open(CurWav);
  unsigned long WavSize = WavFile.size();
  filebrowser.computeFileSizeChar(WavSizeChar, WavSize);
  filebrowser.computeFileLenChar(WavLenChar, WavSize);
  textWavSize.setText(WavSizeChar);
  textWavLen.setText(WavLenChar);
}

//-----------------------------------------------------------------------------------------
// MIXER SETTINGS FUNCTIONS
//-----------------------------------------------------------------------------------------
void SetRecordingInput() {
  mixer1.gain(0, 0);
  mixer1.gain(1, 0);
  mixer1.gain(2, 0.5);
  mixer1.gain(3, 0.5);
}

void SetPlayingInput() {
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);
}

//-----------------------------------------------------------------------------------------
// DATE FUNCTIONS
//-----------------------------------------------------------------------------------------
void dateTime(uint16_t* date, uint16_t* time) {
  time_t t = now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(year(t), month(t), day(t));

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(hour(t), minute(t), second(t));
}

//-----------------------------------------------------------------------------------------
// FFT FUNCTIONS
//-----------------------------------------------------------------------------------------
void UpdateFFTValue() {
  thirdOctValues.updateData(fft, dataVec);
  switch (bandCounter) {
    case 0:
      f0Band.setValue(uint32_t(20 * log10(dataVec[0]) + 80));
      break;
    case 1:
      f1Band.setValue(uint32_t(20 * log10(dataVec[1]) + 80));
      break;
    case 2:
      f2Band.setValue(uint32_t(20 * log10(dataVec[2]) + 80));
      break;
    case 3:
      f3Band.setValue(uint32_t(20 * log10(dataVec[3]) + 80));
      break;
    case 4:
      f4Band.setValue(uint32_t(20 * log10(dataVec[4]) + 80));
      break;
    case 5:
      f5Band.setValue(uint32_t(20 * log10(dataVec[5]) + 80));
      break;
    case 6:
      f6Band.setValue(uint32_t(20 * log10(dataVec[6]) + 80));
      break;
    case 7:
      f7Band.setValue(uint32_t(20 * log10(dataVec[7]) + 80));
      break;
    case 8:
      f8Band.setValue(uint32_t(20 * log10(dataVec[8]) + 80));
      break;
    case 9:
      f9Band.setValue(uint32_t(20 * log10(dataVec[9]) + 80));
      break;
    case 10:
      f10Band.setValue(uint32_t(20 * log10(dataVec[10]) + 80));
      break;
    case 11:
      f11Band.setValue(uint32_t(20 * log10(dataVec[11]) + 80));
      break;
    case 12:
      f12Band.setValue(uint32_t(20 * log10(dataVec[12]) + 80));
      break;
    case 13:
      f13Band.setValue(uint32_t(20 * log10(dataVec[13]) + 80));
      break;
    case 14:
      f14Band.setValue(uint32_t(20 * log10(dataVec[14]) + 80));
      break;
    case 15:
      f15Band.setValue(uint32_t(20 * log10(dataVec[15]) + 80));
      bandCounter += -16;
      break;
  }
  bandCounter += 1;
}

//-----------------------------------------------------------------------------------------
// AGC FUNCTIONS
//-----------------------------------------------------------------------------------------
void useAGC(){
  InGain = agc.getAGCGain(peak2.read());
  amp1.gain(InGain);
  Serial.println(InGain);
}

//-----------------------------------------------------------------------------------------
// CALLBACK FUNCTIONS
//-----------------------------------------------------------------------------------------
void RecordButtonCallback(void *ptr)
{
  SetRecordingInput();
  Serial.print("Record");
  switch (mode) {
    case 0:
      startRecording();
      TimerDisp = 0;
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
  switch (mode) {
    case 0:
      break;
    case 1:
      stopRecording();
      break;
    case 2:
      stopPlaying();
      SetRecordingInput();
      break;
  }
  checkLvl = false;
}

void PlayButtonCallback(void *ptr)
{
  SetPlayingInput();
  Serial.println("Play Button Press");
  switch (mode) {
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

void SaveButtonCallback(void *ptr)
{
  Serial.println("Save Button Press");
  saveCurrentFile();
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

void sliderGainCallback(void *ptr)
{
  uint32_t Gain = 0;
  sliderGain.getValue(&Gain);
  double Gain_d = double(Gain) - 12;
  InGain = pow(10.0, Gain_d / 20.0);
  amp1.gain(InGain);
}

void buttonVolUpCallback(void *ptr)
{
  if (HpVol < 1.0) {
    HpVol += 0.05;
    sgtl5000_1.volume(HpVol);
  }
}

void buttonVolDownCallback(void *ptr)
{
  if (HpVol > 0.0) {
    HpVol -= 0.05;
    sgtl5000_1.volume(HpVol);
  }
}

void buttonMuteCallback(void *ptr)
{
  if (!muted) {
    sgtl5000_1.volume(0.0);
    muted = true;
  }
  else {
    sgtl5000_1.volume(HpVol);
    muted = false;
  }
}

//Callbacks fuer FileBrowser
void buttonPlayWavCallback(void *ptr)
{
  textWavFile.getText(CurWav, 13);
  playFile(CurWav);
  Serial.println(CurWav);
}

void buttonStopWavCallback(void *ptr)
{
  playSdWav1.stop();
}

void buttonWavUpCallback(void *ptr)
{
  if (WavCount < (fileCount - 1))
  {
    textWavFile.getText(CurWav, 12);
    filebrowser.computeCurName(CurWav, 1);
    WavCount++;
    updateFileBrowser();
  }
}


void buttonWavDownCallback(void *ptr)
{
  textWavFile.getText(CurWav, 13);
  filebrowser.computeCurName(CurWav, 0);
  WavCount--;
  updateFileBrowser();
}

void buttonRecorderCallback(void *ptr)
{
  Serial.println("Recorder");
  SetRecordingInput();
  updateMemoryDisp();
}

void buttonPlayerCallback(void *ptr)
{
  Serial.println("FileBrowser");
  SetPlayingInput();
}

void buttonSpectrumCallback(void *ptr) {
  analyzeActiv = true;
  TimerFFT = 0;
  Serial.println("Spectrum Analyzer");
}

//Callbacks fuer EQ Kram
void buttonEQSettingsCallback(void *ptr) {
  EQf0Slider.setValue(uint32_t(f0Gain * 12) + 12);
  EQf1Slider.setValue(uint32_t(f1Gain * 12) + 12);
  EQf2Slider.setValue(uint32_t(f2Gain * 12) + 12);
  EQf3Slider.setValue(uint32_t(f3Gain * 12) + 12);
  EQf4Slider.setValue(uint32_t(f4Gain * 12) + 12);

  EQf0Val.setValue(uint32_t(f0Gain * 12));
  EQf1Val.setValue(uint32_t(f1Gain * 12));
  EQf2Val.setValue(uint32_t(f2Gain * 12));
  EQf3Val.setValue(uint32_t(f3Gain * 12));
  EQf4Val.setValue(uint32_t(f4Gain * 12));
}

void buttonEQResetCallback(void *ptr) {
  f0Gain = 0;
  f1Gain = 0;
  f2Gain = 0;
  f3Gain = 0;
  f4Gain = 0;

  sgtl5000_1.eqBands(f0Gain, f1Gain, f2Gain, f3Gain, f4Gain);
}

void EQf0SliderCallback(void *ptr) {
  uint32_t f0Gain_dB = 0;
  EQf0Slider.getValue(&f0Gain_dB);
  f0Gain = (double(f0Gain_dB) - 12.0) / 12.0;
  sgtl5000_1.eqBand(0, f0Gain);
}

void EQf0ButPCallback(void *ptr) {
  uint32_t f0Gain_dB = 0;
  EQf0Val.getValue(&f0Gain_dB);
  f0Gain = double(f0Gain_dB) / 12.0;
  sgtl5000_1.eqBand(0, f0Gain);
}

void EQf0ButMCallback(void *ptr) {
  uint32_t f0Gain_dB = 0;
  EQf0Val.getValue(&f0Gain_dB);
  f0Gain = double(f0Gain_dB) / 12.0;
  sgtl5000_1.eqBand(0, f0Gain);
}

void EQf1SliderCallback(void *ptr) {
  uint32_t f1Gain_dB = 0;
  EQf1Slider.getValue(&f1Gain_dB);
  f1Gain = (double(f1Gain_dB) - 12) / 12.0;
  sgtl5000_1.eqBand(1, f1Gain);
}

void EQf1ButPCallback(void *ptr) {
  uint32_t f1Gain_dB = 0;
  EQf1Val.getValue(&f1Gain_dB);
  f1Gain = double(f1Gain_dB) / 12.0;
  sgtl5000_1.eqBand(1, f1Gain);
}

void EQf1ButMCallback(void *ptr) {
  uint32_t f1Gain_dB = 0;
  EQf1Val.getValue(&f1Gain_dB);
  f1Gain = double(f1Gain_dB) / 12.0;
  sgtl5000_1.eqBand(1, f1Gain);
}

void EQf2SliderCallback(void *ptr) {
  uint32_t f2Gain_dB = 0;
  EQf2Slider.getValue(&f2Gain_dB);
  f2Gain = (double(f2Gain_dB) - 12) / 12.0;
  sgtl5000_1.eqBand(2, f2Gain);
}

void EQf2ButPCallback(void *ptr) {
  uint32_t f2Gain_dB = 0;
  EQf2Val.getValue(&f2Gain_dB);
  f2Gain = double(f2Gain_dB) / 12.0;
  sgtl5000_1.eqBand(2, f2Gain);
}

void EQf2ButMCallback(void *ptr) {
  uint32_t f2Gain_dB = 0;
  EQf2Val.getValue(&f2Gain_dB);
  f2Gain = double(f2Gain_dB) / 12.0;
  sgtl5000_1.eqBand(2, f2Gain);
}

void EQf3SliderCallback(void *ptr) {
  uint32_t f3Gain_dB = 0;
  EQf3Slider.getValue(&f3Gain_dB);
  f3Gain = (double(f3Gain_dB) - 12) / 12.0;
  sgtl5000_1.eqBand(3, f3Gain);
}

void EQf3ButPCallback(void *ptr) {
  uint32_t f3Gain_dB = 0;
  EQf3Val.getValue(&f3Gain_dB);
  f3Gain = double(f3Gain_dB) / 12.0;
  sgtl5000_1.eqBand(3, f3Gain);
}

void EQf3ButMCallback(void *ptr) {
  uint32_t f3Gain_dB = 0;
  EQf3Val.getValue(&f3Gain_dB);
  f3Gain = double(f3Gain_dB) / 12.0;
  sgtl5000_1.eqBand(3, f3Gain);
}

void EQf4SliderCallback(void *ptr) {
  uint32_t f4Gain_dB = 0;
  EQf4Slider.getValue(&f4Gain_dB);
  f4Gain = (double(f4Gain_dB) - 12) / 12.0;
  Serial.println(f4Gain);
  sgtl5000_1.eqBand(4, f4Gain);
}

void EQf4ButPCallback(void *ptr) {
  uint32_t f4Gain_dB = 0;
  EQf4Val.getValue(&f4Gain_dB);
  f4Gain = double(f4Gain_dB) / 12.0;
  sgtl5000_1.eqBand(4, f4Gain);
}

void EQf4ButMCallback(void *ptr) {
  uint32_t f4Gain_dB = 0;
  EQf4Val.getValue(&f4Gain_dB);
  f4Gain = double(f4Gain_dB) / 12.0;
  sgtl5000_1.eqBand(4, f4Gain);
}

//Callbacks Spectrum
void ApplyAveCallback(void *ptr) {
  uint32_t blockAve = 0;
  AveBlocks.getValue(&blockAve);
  thirdOctValues.setAverages(double(blockAve));
}

void SpecMenuCallback(void *ptr) {
  analyzeActiv = false;
  Serial.println("Quit Spectrum");
  thirdOctValues.reset(dataVec);
}

void AGCButtonCallback(void *ptr)
{
  Serial.println("AGC Button Press");
  if (!AGCOn)
  {
    AGCOn = true;
    Serial.println("AGC On");
  }
  else
  {
    AGCOn = false;
    Serial.println("AGC Off");
  }
}

void sliderSetPointCallback(void *ptr){
  uint32_t SetPointVal = 0;
  sliderSetPoint.getValue(&SetPointVal);
  SetPoint = -20.0 + double(SetPointVal);
  agc.setSetpoint(SetPoint);
  Serial.println("SetPoint changed");
}

void sliderRangeCallback(void *ptr){
  uint32_t RangeVal = 0;
  sliderRange.getValue(&RangeVal);
  Range = double(RangeVal);
  agc.setRange(Range);
  Serial.println("Range changed");
}

void sliderReactCallback(void *ptr){
  sliderReact.getValue(&ReactVal);
  if(ReactVal == 1){
    timeConst = AGC::timeConstants::fast;
    Serial.println("TimeConst = fast");
  }
  if(ReactVal == 2){
    timeConst = AGC::timeConstants::medium;
    Serial.println("TimeConst = medium");
  }
  if(ReactVal == 3){
    timeConst = AGC::timeConstants::slow;
    Serial.println("TimeConst = slow");
  }
  if(ReactVal == 4){
    timeConst = AGC::timeConstants::veryslow;
    Serial.println("TimeConst = veryslow");
  }
  agc.setTimeConst(timeConst);
  Serial.println("Time const changed");
}

void sliderRatioCallback(void *ptr){
  uint32_t RatioVal = 0;
  sliderRatio.getValue(&RatioVal);
  Ratio = double(RatioVal);
  agc.setRatio(Ratio);
}

void buttonApplyDCallback(void *ptr) {
  Serial.println("Hello");
  NumDay.getValue(&Day);
  delay(5);
  NumMonth.getValue(&Month);
  delay(5);
  NumYear.getValue(&Year);
  setTime(hours, mins, secs, Day, Month, Year);
  SdFile::dateTimeCallback(dateTime);
}

void buttonApplyTCallback(void *ptr) {
  Serial.println("Hello");
  NumHour.getValue(&hours);
  delay(5);
  NumMin.getValue(&mins);
  delay(5);
  NumSec.getValue(&secs);
  setTime(hours, mins, secs, Day, Month, Year);
  SdFile::dateTimeCallback(dateTime);
}

void buttonAGCSetCallback(void *ptr) {
  sliderSetPoint.setValue(uint32_t(SetPoint)+20);
  sliderRange.setValue(uint32_t(Range));
  sliderReact.setValue(ReactVal);
  sliderRatio.setValue(uint32_t(Ratio));
}
