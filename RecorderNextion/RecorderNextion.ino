// Version von Martin und Tammo (für eigenen Branch)
// Erfordert als zusätzliche Library RMSLevel (von mir)
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
#include <RMSLevel.h>
#include <WaveHeader.h>
#include <RunningTimeLabel.h>
#include <FileBrowser.h>

// GUItool: begin automatically generated code
AudioInputI2S            AudioInput;           //xy=105,63
AudioAnalyzePeak         peak1;          //xy=278,108
AudioRecordQueue         queue1;         //xy=281,63
AudioPlaySdWav           playSdWav1;       //xy=302,157
AudioOutputI2S           AudioOutput;           //xy=470,120
AudioMixer4              mixer1;
AudioConnection          patchCord1(AudioInput, 1, queue1, 0);
AudioConnection          patchCord2(AudioInput, 1, peak1, 0);
AudioConnection          patchCord3(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord4(playSdWav1, 1, mixer1, 1);
AudioConnection          patchCord5(AudioInput, 0, mixer1, 2);
AudioConnection          patchCord6(AudioInput, 1, mixer1, 3);
AudioConnection          patchCord7(mixer1, 0, AudioOutput, 0);
AudioConnection          patchCord8(mixer1, 0, AudioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=265,212
elapsedMillis            TimerDisp;
elapsedMillis            TimePassed;
// GUItool: end automatically generated code

// Nextion Buttons: NexButton(int page, int objectID, string name)
NexButton buttonRecord = NexButton(0,3,"Record");
NexButton buttonStop = NexButton(0,2,"Stop");
NexButton buttonPlay = NexButton(0,1,"Play");
NexButton buttonSave = NexButton(0,9,"Save");
NexButton buttonCheckLvl = NexButton(0,12,"CheckLvl");
//NexButton buttonInput = NexButton(0,12,"Input");

NexSlider sliderGain = NexSlider(0,11,"Gain");
NexProgressBar ProgBarLevel = NexProgressBar(0,10,"Pegel");
NexText textTimer = NexText(0,7,"Timer");
NexText textFile = NexText(0,8,"FileName");
NexText textAvailable = NexText(0,13,"Verfg");

//Buttons und Textfeld fuer FileBrowser
NexText textWavFile = NexText(2,1,"WavFile");
NexText textWavSize = NexText(2,10,"WavSize");
NexText textWavLen = NexText(2,12,"WavLen");
NexText textWaveDate = NexText(2,14,"WavTime");
NexText textWavTimer = NexText(2,15,"Timer");
NexButton buttonPlayWav = NexButton(2,2,"PlayWav");
NexButton buttonStopWav = NexButton(2,3,"WavStop");
NexButton buttonWavUp = NexButton(2,4,"WavUp");
NexButton buttonWavDown = NexButton(2,5,"WavDown");

//MenuButtons
// hier noch richtig anpassen
NexButton buttonRecorder = NexButton(1,3,"Recorder");
NexButton buttonPlayer = NexButton(1,2,"FileBrowser");

//RecorderSettings
NexButton buttonEQSettings = NexButton(3,8,"EQSettings");
NexButton buttonEQReset = NexButton(3,9,"EQReset");
//Hier noch einstellungen fuer Datum Uhrzeit etc.

//EQ-Items
NexSlider EQf0Slider = NexSlider(5,2,"EQSlider0");
NexButton EQf0ButP = NexButton(5,8,"EQf0p");
NexButton EQf0ButM = NexButton(5,18,"EQf0m");
NexNumber EQf0Val = NexNumber(5,13,"Valf0");
NexSlider EQf1Slider = NexSlider(5,3,"EQSlider1");
NexButton EQf1ButP = NexButton(5,9,"EQf1p");
NexButton EQf1ButM = NexButton(5,19,"EQf1m");
NexNumber EQf1Val = NexNumber(5,14,"Valf1");
NexSlider EQf2Slider = NexSlider(5,4,"EQSlider2");
NexButton EQf2ButP = NexButton(5,10,"EQf2p");
NexButton EQf2ButM = NexButton(5,20,"EQf2m");
NexNumber EQf2Val = NexNumber(5,15,"Valf2");
NexSlider EQf3Slider = NexSlider(5,5,"EQSlider3");
NexButton EQf3ButP = NexButton(5,11,"EQf3p");
NexButton EQf3ButM = NexButton(5,21,"EQf3m");
NexNumber EQf3Val = NexNumber(5,16,"Valf3");
NexSlider EQf4Slider = NexSlider(5,6,"EQSlider4");
NexButton EQf4ButP = NexButton(5,12,"EQf4p");
NexButton EQf4ButM = NexButton(5,22,"EQf4m");
NexNumber EQf4Val = NexNumber(5,17,"Valf4");

// Liste mit Buttons
NexTouch *nex_listen_list[] =
{
  &buttonRecord,
  &buttonStop,
  &buttonPlay,
  &buttonSave,
  &buttonCheckLvl,
  &buttonPlayWav,
  &buttonStopWav,
  &buttonWavUp,
  &buttonWavDown,
  &sliderGain,
  &buttonRecorder,
  &buttonPlayer,
  &buttonEQSettings,
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
  NULL
};

// which input on the audio shield will be used?
int inputMode = 1;
int myInput = AUDIO_INPUT_LINEIN;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
File frec;

//Needed variables for saving stuff
FileBrowser filebrowser;
int fileCount = 0;
char filename[] = "RECORD01.WAV";
char lastSave[] = "RECORD01.WAV";
char MemoryDisp[] = "16,00 GB / 00:00:00";
uint32_t availableMemory_byte = 0;
uint32_t availableTime_sec = 0;
uint32_t usedMemory = 0;
Sd2Card card;
SdVolume volume;
File root;
double volumesize = 0.0;
unsigned long long SDSize = 0;

// Variablen für den Wave-Header
WaveHeader waveheader;
unsigned long recByteSaved = 0L;

// Initialising rms level meter
// Vielleicht noch 512 als Variable anlegen, so BlockLength oder so?
double tau = 0.125;
double f_refresh = 4;
RMSLevel rmsMeter(tau,f_refresh);
bool checkLvl = false;
unsigned int dispDelay = 1000/f_refresh;

uint32_t sliderValue = 50;

//variables for Timer
RunningTimeLabel tLabel;
char TimerVal[] = "00:00:00";

//Variables fuer FileBrowser
char CurWav[] = "RECORD01.WAV";
char WavSizeChar[]= "00.00 GB";
char WavLenChar[] = "00:00:00";
int WavCount = 0;
File WavFile;

bool saved = false;

//variables for EQ
double f0Gain = 0.0;
double f1Gain = 0.0;
double f2Gain = 0.0;
double f3Gain = 0.0;
double f4Gain = 0.0;

void setup() {

  nexInit();
  Serial7.print("baud=115200");
  Serial7.write(0xff);
  Serial7.write(0xff);
  Serial7.write(0xff);
  Serial7.end();

  Serial7.begin(115200);
//  sliderGain.setValue(sliderValue);
  
  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(256);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
  sgtl5000_1.audioPreProcessorEnable();
  sgtl5000_1.eqSelect(3);

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

  //Berechnung der SD-Kartengröße
  card.init(SPI_HALF_SPEED, SDCARD_CS_PIN);
  volume.init(card);
  
  SDSize = volume.blocksPerCluster();    // clusters are collections of blocks
  SDSize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize = SDSize;
  SDSize *= 512;
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 2;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  
  root = SD.open("/");
  computeUsedMemory(root);
  checkCurrentFile();

  //Mixereinstellungen beim Start
  SetRecordingInput();
  
  // Link Callbacks
  buttonRecord.attachPush(RecordButtonCallback);
  buttonStop.attachPush(StopButtonCallback);
  buttonPlay.attachPush(PlayButtonCallback);
  buttonSave.attachPush(SaveButtonCallback);
  buttonCheckLvl.attachPush(buttonCheckLvlCallback);
  buttonPlayWav.attachPush(buttonPlayWavCallback);
  buttonStopWav.attachPush(buttonStopWavCallback);
  buttonWavUp.attachPush(buttonWavUpCallback);
  buttonWavDown.attachPush(buttonWavDownCallback);
  buttonRecorder.attachPush(buttonRecorderCallback);
  buttonEQSettings.attachPush(buttonEQSettingsCallback);
  buttonEQReset.attachPush(buttonEQResetCallback);
  buttonPlayer.attachPush(buttonPlayerCallback);
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

}


void loop() {
  // Respond to button presses
  nexLoop(nex_listen_list);

  // If we're playing or recording, carry on...
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
    
    if (mode == 1 || mode == 2){
      tLabel.updateLabel(TimePassed,TimerVal);
      textTimer.setText(TimerVal);
    }

    TimerDisp-=dispDelay;
  }
}


void checkCurrentFile()
{
  while(fileCount>=0)
  {
    if(SD.exists(filename)){
      fileCount += 1;
      filebrowser.computeCurName(filename,1);
    }
    else{
      updateMemoryDisp();
      break;
    }
  }   
}

void saveCurrentFile(){
  strcpy(lastSave, filename);
  fileCount +=1;
  filebrowser.computeCurName(filename,1);
  usedMemory += recByteSaved+36;
  updateMemoryDisp();
  saved = true;
}

void computeUsedMemory(File dir){
  while(true){
    File entry = dir.openNextFile();
    usedMemory += entry.size();
    if(!entry){
      usedMemory -= 8192;
      break;
    }
    entry.close();
  }
  updateMemoryDisp();
}

void updateMemoryDisp(){
  filebrowser.computeAvailableMemory(MemoryDisp, SDSize, usedMemory, availableMemory_byte, availableTime_sec);
  textFile.setText(filename);
  textAvailable.setText(MemoryDisp);    
}

void startRecording() {
  Serial.println("startRecording");
    if (SD.exists(filename)) {
    SD.remove(filename);
  }
  frec = SD.open(filename, FILE_WRITE);
  if (frec) {
    queue1.begin();
    mode = 1;
    checkLvl = true;
    recByteSaved = 0L;
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
    recByteSaved += 512;                                // Addiert in jedem Durchlauf 512 Bytes   
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
      recByteSaved += 256;                                        // Addiert die letzten 256 Bytes bei Aufnahme-Stopp
    }
  }
  waveheader.writeWaveHeader(recByteSaved, frec);                 // Schreibt den Wave-Header auf die SD-Karte
  mode = 0;
  checkLvl = false;
  saved = false;
  TimePassed = 0;
}


void startPlaying() {
  Serial.println("startPlaying");
  TimePassed = 0;
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

void continuePlaying() {
  if (!playSdWav1.isPlaying()) {
    playSdWav1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playSdWav1.stop();
  mode = 0;
  TimePassed = 0;
}

void displayRefresh(){
  if (checkLvl){
    displayLvl();
  }
}

void displayLvl() {
  uint32_t ProgBarVal = uint32_t(100*(1-(rmsMeter.updateRMS(double(peak1.read()))/-80)));

  ProgBarLevel.setValue(ProgBarVal); 
  
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playSdWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);
}

void SetRecordingInput(){
  mixer1.gain(0,0);
  mixer1.gain(1,0);
  mixer1.gain(2,0.5);
  mixer1.gain(3,0.5);
}

void SetPlayingInput(){
  mixer1.gain(0,0.5);
  mixer1.gain(1,0.5);
  mixer1.gain(2,0);
  mixer1.gain(3,0);
}

void updateFileBrowser(){
  textWavFile.setText(CurWav);
  WavFile = SD.open(CurWav);
  unsigned long WavSize = WavFile.size();
  filebrowser.computeFileSizeChar(WavSizeChar,WavSize);
  filebrowser.computeFileLenChar(WavLenChar,WavSize);
  textWavSize.setText(WavSizeChar);
  textWavLen.setText(WavLenChar);
}

// PUSH CALLBACKS
void RecordButtonCallback(void *ptr)
{
  SetRecordingInput();
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
      SetRecordingInput();
      break;
  }
  checkLvl = false;
}

void PlayButtonCallback(void *ptr)
{
  SetPlayingInput();
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
  sgtl5000_1.micGain(sliderGain.getValue(&sliderValue));
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
  if (WavCount < (fileCount-1))
  {
    textWavFile.getText(CurWav, 12);
    filebrowser.computeCurName(CurWav,1);
    WavCount++;
    updateFileBrowser();
  }
}


void buttonWavDownCallback(void *ptr)
{
  textWavFile.getText(CurWav, 13);
  filebrowser.computeCurName(CurWav,0);
  WavCount--;
  updateFileBrowser();
}

void buttonRecorderCallback(void *ptr)
{
  Serial.println("Recorder");
  SetRecordingInput();
}

void buttonPlayerCallback(void *ptr)
{
  Serial.println("FileBrowser");
  SetPlayingInput();
}

//Callbacks fuer EQ Kram
void buttonEQSettingsCallback(void *ptr){
  EQf0Slider.setValue(uint32_t(f0Gain*12)+12);
  EQf1Slider.setValue(uint32_t(f1Gain*12)+12);
  EQf2Slider.setValue(uint32_t(f2Gain*12)+12);
  EQf3Slider.setValue(uint32_t(f3Gain*12)+12);
  EQf4Slider.setValue(uint32_t(f4Gain*12)+12);

  EQf0Val.setValue(uint32_t(f0Gain*12));
  EQf1Val.setValue(uint32_t(f1Gain*12));
  EQf2Val.setValue(uint32_t(f2Gain*12));
  EQf3Val.setValue(uint32_t(f3Gain*12));
  EQf4Val.setValue(uint32_t(f4Gain*12));
}

void buttonEQResetCallback(void *ptr){
  f0Gain = 0;
  f1Gain = 0;
  f2Gain = 0;
  f3Gain = 0;
  f4Gain = 0;

  sgtl5000_1.eqBands(f0Gain,f1Gain,f2Gain,f3Gain,f4Gain);
}

void EQf0SliderCallback(void *ptr){
  uint32_t f0Gain_dB = 0;
  EQf0Slider.getValue(&f0Gain_dB);
  f0Gain = (double(f0Gain_dB)-12.0)/12.0;
  sgtl5000_1.eqBand(0,f0Gain);
}

void EQf0ButPCallback(void *ptr){
  uint32_t f0Gain_dB = 0;
  EQf0Val.getValue(&f0Gain_dB);
  f0Gain = double(f0Gain_dB)/12.0;
  sgtl5000_1.eqBand(0,f0Gain);
}

void EQf0ButMCallback(void *ptr){
  uint32_t f0Gain_dB = 0;
  EQf0Val.getValue(&f0Gain_dB);
  f0Gain = double(f0Gain_dB)/12.0;
  sgtl5000_1.eqBand(0,f0Gain);
}

void EQf1SliderCallback(void *ptr){
  uint32_t f1Gain_dB = 0;
  EQf1Slider.getValue(&f1Gain_dB);
  f1Gain = (double(f1Gain_dB)-12)/12.0;
  sgtl5000_1.eqBand(1,f1Gain);  
}

void EQf1ButPCallback(void *ptr){
  uint32_t f1Gain_dB = 0;
  EQf1Val.getValue(&f1Gain_dB);
  f1Gain = double(f1Gain_dB)/12.0;
  sgtl5000_1.eqBand(1,f1Gain);
}

void EQf1ButMCallback(void *ptr){
if(f1Gain>-1.0){
    f1Gain-=1/12;
    sgtl5000_1.eqBand(2,f1Gain);
  }  
}

void EQf2SliderCallback(void *ptr){
  uint32_t f2Gain_dB = 0;
  EQf2Slider.getValue(&f2Gain_dB);
  f2Gain = (double(f2Gain_dB)-12)/12.0;
  sgtl5000_1.eqBand(2,f2Gain);  
}

void EQf2ButPCallback(void *ptr){
  if(f2Gain<1.0){
    f2Gain+=1/12;
    sgtl5000_1.eqBand(3,f2Gain);
  }  
}

void EQf2ButMCallback(void *ptr){
if(f2Gain>-1.0){
    f2Gain-=1/12;
    sgtl5000_1.eqBand(3,f2Gain);
  }  
}

void EQf3SliderCallback(void *ptr){
  uint32_t f3Gain_dB = 0;
  EQf3Slider.getValue(&f3Gain_dB);
  f3Gain = (double(f3Gain_dB)-12)/12.0;
  sgtl5000_1.eqBand(3,f3Gain);
}

void EQf3ButPCallback(void *ptr){
  if(f3Gain<1.0){
    f3Gain+=1/12;
    sgtl5000_1.eqBand(4,f3Gain);
  }  
}

void EQf3ButMCallback(void *ptr){
if(f3Gain>-1.0){
    f3Gain-=1/12;
    sgtl5000_1.eqBand(4,f3Gain);
  }  
}

void EQf4SliderCallback(void *ptr){
  uint32_t f4Gain_dB = 0;
  EQf4Slider.getValue(&f4Gain_dB);
  f4Gain = (double(f4Gain_dB)-12)/12.0;
  Serial.println(f4Gain);
  sgtl5000_1.eqBand(4,f4Gain);
}

void EQf4ButPCallback(void *ptr){
  if(f4Gain<1.0){
    f4Gain+=1/12;
    sgtl5000_1.eqBand(5,f4Gain);
  }  
}

void EQf4ButMCallback(void *ptr){
if(f4Gain>-1.0){
    f4Gain-=1/12;
    sgtl5000_1.eqBand(5,f4Gain);
  }  
}
