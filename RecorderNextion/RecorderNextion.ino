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
AudioConnection          patchCord1(AudioInput, 0, queue1, 0);
AudioConnection          patchCord2(AudioInput, 0, peak1, 0);
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

/* ES MUSS EIN NEUER SLIDER ERSTELLT WERDEN (PAGE UND ID
   INDIVIDUELL ANPASSEN). DIE SKALA DES SLIDERS MUSS
   ZWISCHEN 0 und 63 LIEGEN!
   
   ES WURDE GANZ UNTEN EIN CALLBACK HINZUGEFÜGT.
   DER SLIDER WURDE DER NEX_LISTEN_LIST HINZUGEFÜGT.
   IN DER SETUP WURDE EIN POP CALLBACK ATTACHED.
   FALLS FEHLER AUFTRETEN, IST WOHL AN DIESEN STELLEN
   NACHZUGUCKEN.

   FUNKTIONSWEISE:
   DER GAIN DES INPUTS KANN ZWISCHEN 0 UND 63 DB EINGESTELLT
   WERDEN (LAUT DOC). DAZU WIRD AUF DAS sgtl5000_1-OBJEKT
   ZUGEGRIFFEN, WELCHES DIE FUNKTION micGain BESITZT.
   DIESE WIRD IN DER SLIDER-CALLBACK FUNKTION AUF DEN WERT
   DES SLIDERS GESETZT.
   */
NexSlider sliderGain = NexSlider(0,11,"Gain");
NexProgressBar ProgBarLevel = NexProgressBar(0,10,"Pegel");
NexText textTimer = NexText(0,7,"Timer");
NexText textFile = NexText(0,8,"FileName");
NexText textAvailable = NexText(0,13,"Verfg");

//Buttons und Textfeld fuer FileBrowser
NexText textWavFile = NexText(2,1,"WavFile");
NexButton buttonPlayWav = NexButton(2,2,"PlayWav");
NexButton buttonStopWav = NexButton(2,3,"WavStop");
NexButton buttonWavUp = NexButton(2,5,"WavUp");
NexButton buttonWavDown = NexButton(2,6,"WavDown");

//MenuButtons
// hier noch richtig anpassen
NexButton buttonRecorder = NexButton(1,3,"Recorder");
NexButton buttonPlayer = NexButton(1,2,"FileBrowser");

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
int WavCount = 0;

bool saved = false;


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
  //buttonInput.attachPush(InputButtonCallback);
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
  buttonPlayer.attachPush(buttonPlayerCallback);
//  sliderGain.attachPop(sliderGainCallback);

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
    textWavFile.setText(CurWav);
  }
}


void buttonWavDownCallback(void *ptr)
{
  textWavFile.getText(CurWav, 13);
  filebrowser.computeCurName(CurWav,0);
  WavCount--;
  textWavFile.setText(CurWav);
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
