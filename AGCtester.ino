// Tester für Automatic Gain Control

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Nextion.h>

//#include <AutomaticGainControl.h>
//-----------------------------------------------------------------------------------------
// AUDIO
//-----------------------------------------------------------------------------------------
AudioInputI2S            i2s2 ;           //xy=105,63
AudioAnalyzePeak         peak1;          //xy=278,108
// AUDIO ANALYZE RMS HINZUFUEGEN
AudioAnalyzeRMS          rms_mono;
AudioRecordQueue         queue1;         //xy=281,63
AudioPlaySdRaw           playRaw1;       //xy=302,157
AudioOutputI2S           i2s1;           //xy=470,120
AudioConnection          patchCord1(i2s2, 0, queue1, 0);
AudioConnection          patchCord2(i2s2, 0, peak1, 0);
AudioConnection          patchCord3(playRaw1, 0, i2s1, 0);
AudioConnection          patchCord4(playRaw1, 0, i2s1, 1);
AudioConnection          patchCord6(i2s2, 0, i2s1, 0);
AudioConnection          patchCord7(i2s2, 0, i2s1, 1);
AudioConnection          patchCord5(i2s2, 0, rms_mono, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=265,212

//-----------------------------------------------------------------------------------------
// TIMING
//-----------------------------------------------------------------------------------------
elapsedMillis            TimerLvl;

//-----------------------------------------------------------------------------------------
//BUTTONS
//-----------------------------------------------------------------------------------------
//NexButton buttonAGC = NexButton(0, 18, "AGC");
NexSlider sliderVolume = NexSlider(0, 15, "Volume");
NexSlider sliderGain = NexSlider(0, 11, "Gain");
//NexSlider sliderAGChang = NexSlider(2, 26, "AGChang");
//NexSlider sliderAGCslopeInc = NexSlider(2, 25, "AGCslopeInc");
//NexSlider sliderAGCslopeDec = NexSlider(2, 24, "AGCslopeDec");
//NexSlider sliderAGCtresh = NexSlider(2, 7, "AGCtresh");

NexTouch *nex_listen_list[] =
{
  //&buttonAGC,
  &sliderVolume,
  &sliderGain,
  //&sliderAGChang,
  //&sliderAGCslopeInc,
  //&sliderAGCslopeDec,
  //&sliderAGCtresh,
  NULL
};

//int myInput = AUDIO_INPUT_LINEIN;
int myInput =  AUDIO_INPUT_MIC;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

int mode = 0;  // 0=stopped, 1=recording, 2=playing


//-----------------------------------------------------------------------------------------
// Automatic Gain Control
//-----------------------------------------------------------------------------------------
int AGCMode = 2;                     // Default : 1   AGCoff

int AGChangtime = 3;                 // Default : 500
double AGCtresh = 0.8;               // Default : 0.95  
double AGCslopeIncrease  = 1.0;      // Default : 0.1     
double AGCslopeDecrease  = 1.0;      // Default : 1.0


elapsedMillis MilliSec=0;
//double AGCvalue;

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
  //buttonAGC.attachPush(AGCButtonCallback);
  sliderVolume.attachPop(sliderVolumeCallback);
  sliderGain.attachPop(sliderGainCallback);
  //sliderAGChang.attachPop(sliderAGChangCallback);
  //sliderAGCslopeInc.attachPop(sliderAGCslopeIncCallback);
  //sliderAGCslopeDec.attachPop(sliderAGCslopeDecCallback);
  //sliderAGCtresh.attachPop(sliderAGCtreshCallback);
}

//-----------------------------------------------------------------------------------------
// LOOP
//-----------------------------------------------------------------------------------------
void loop() {
  nexLoop(nex_listen_list);

  if (AGCMode == 2){
  AutoGain();
  }
}

//-----------------------------------------------------------------------------------------
// BASIS FUNKTIONEN
//-----------------------------------------------------------------------------------------
#define AGCMAX 63
#define AGCMIN 1

int gainReduceChecker;
int gainReduceCounter;
int agcAttack = 1;
double agcGainOut;
    
void AutoGain() 
{
  if (MilliSec > 500)           //Default : 2 
  {
    static int hangtimer;
    static double agcGain = 1;
    double peak;                         

    // read peak
    peak = peak1.read();
    Serial.println("peak=");
    Serial.println(peak);

    // Check if signal is too loud
    for (gainReduceCounter = 0, gainReduceChecker = 0; gainReduceCounter < agcAttack; gainReduceCounter++)
    {
        if (peak > AGCtresh) gainReduceChecker++;
    }

    // Gain Decrease
    //if ((gainReduceChecker == agcAttack) || ((gainReduceChecker > 0) && (hangtimer >= AGChangtime)))
    if (gainReduceChecker == agcAttack)
    {
        agcGainOut = (agcGain - AGCslopeDecrease);

        // Reduce limit 
        if (agcGainOut <= AGCMIN) agcGainOut = AGCMIN;

        agcGain = agcGainOut;

        // Reset hangtimer
        hangtimer = 0;
    }

    // hangtimer before increasing
    else if (peak < AGCtresh)
    {
        hangtimer++;
    }

    // Gain Increase
    if (hangtimer >= AGChangtime)
    {
        agcGainOut = agcGain + AGCslopeIncrease;

        // Gain limit 
        if (agcGainOut >= AGCMAX) agcGainOut = AGCMAX;

        agcGain = agcGainOut;
    }
    Serial.println("Gain=");
    Serial.println(agcGainOut);

    // set micGain
    sgtl5000_1.micGain(agcGainOut);
    
    MilliSec = 0;
  }
}

//-----------------------------------------------------------------------------------------
// PUSH CALLBACKS
//-----------------------------------------------------------------------------------------

void sliderVolumeCallback(void *ptr)
{
  uint32_t slideValue = 0;
  sliderVolume.getValue(&slideValue);
  double volumeValue = slideValue /100.0;
  sgtl5000_1.volume(volumeValue);
}

void sliderGainCallback(void *ptr)
{
  uint32_t slideGainValue = 0;
  sliderGain.getValue(&slideGainValue);
  double gainValue = slideGainValue; 
  sgtl5000_1.micGain(gainValue);
}
/*
void AGCButtonCallback(void *ptr)
{
  Serial.println("AGC Button Press");
    if (AGCMode == 1)
      {
      AGCMode = 2;                             
      Serial.println("AGC On");
      }      
    else 
      {
      AGCMode = 1;
      Serial.println("AGC Off");                 
      }  
          
}

void sliderAGChangCallback(void *ptr)
{
  uint32_t hangSetting = 0;
  sliderAGChang.getValue(&hangSetting);
  if (hangSetting == 1)
  {
    AGChangtime = 0;         //Off
    Serial.println("AGC Hangtime Off");
  }

  if (hangSetting == 2)
  {
    AGChangtime = 250;         //500ms
    Serial.println("AGC Hangtime 500ms");
  }

  if (hangSetting == 3)
  {
    AGChangtime = 500;         //1000ms
    Serial.println("AGC Hangtime 1000ms");
  }

  if (hangSetting == 4)
  {
    AGChangtime = 1000;        //2000ms
    Serial.println("AGC Hangtime 2000ms");
  }
}

void sliderAGCslopeIncCallback(void *ptr)
{
  uint32_t slopeIncSetting = 0;
  sliderAGCslopeInc.getValue(&slopeIncSetting);
  if (slopeIncSetting == 1)
  {
    AGCslopeIncrease = 0.05;        
    Serial.println("AGCslopeIncrease = 0.05");
  }

  if (slopeIncSetting == 2)
  {
    AGCslopeIncrease = 0.1;         
    Serial.println("AGCslopeIncrease = 0.1");
  }

  if (slopeIncSetting == 3)
  {
    AGCslopeIncrease = 0.5;         
    Serial.println("AGCslopeIncrease = 0.5");
  }

  if (slopeIncSetting == 4)
  {
    AGCslopeIncrease = 1;        
    Serial.println("AGCslopeIncrease = 1");
  }
}
void sliderAGCslopeDecCallback(void *ptr)
{
  uint32_t slopeDecSetting = 0;
  sliderAGCslopeDec.getValue(&slopeDecSetting);
  if (slopeDecSetting == 1)
  {
    AGCslopeDecrease = 0.1;         
    Serial.println("AGCslopeDecrease = 0.1");
  }

  if (slopeDecSetting == 2)
  {
    AGCslopeDecrease = 0.5;         
    Serial.println("AGCslopeDecrease = 0.5");
  }

  if (slopeDecSetting == 3)
  {
    AGCslopeDecrease = 1.0;         
    Serial.println("AGCslopeDecrease = 1.0");
  }

  if (slopeDecSetting == 4)
  {
    AGCslopeDecrease = 2.0;        
    Serial.println("AGCslopeDecrease = 2.0");
  }
}

void sliderAGCtreshCallback(void *ptr)
{
  uint32_t treshSetting = 0;
  sliderAGCtresh.getValue(&treshSetting);
  double treshSet = (treshSetting + 60);
  AGCtresh = treshSet/100;
  Serial.println("AGCtresh = ");
  Serial.println(AGCtresh);
}
*/
