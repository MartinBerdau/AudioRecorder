// Record sound as raw data to a SD card, and play it back.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// Three pushbuttons need to be connected:
//   Record Button: pin 0 to GND
//   Stop Button:   pin 1 to GND
//   Play Button:   pin 2 to GND
//
// This example code is in the public domain.

#include <Bounce.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=105,63
AudioAnalyzePeak         peak1;          //xy=278,108
AudioRecordQueue         queue1;         //xy=281,63
AudioPlaySdRaw           playRaw1;       //xy=302,157
AudioOutputI2S           i2s1;           //xy=470,120
AudioConnection          patchCord1(i2s2, 0, queue1, 0);
AudioConnection          patchCord2(i2s2, 0, peak1, 0);
AudioConnection          patchCord3(playRaw1, 0, i2s1, 0);
AudioConnection          patchCord4(playRaw1, 0, i2s1, 1);
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


// which input on the audio shield will be used?
const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;


// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// Use these with the Teensy 3.5 & 3.6 SD card
//#define SDCARD_CS_PIN    BUILTIN_SDCARD
//#define SDCARD_MOSI_PIN  11  // not actually used
//#define SDCARD_SCK_PIN   13  // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13


// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
File frec;

void setup() {
  // Configure the pushbutton pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(256);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.9);

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

  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (myInput == AUDIO_INPUT_MIC) adjustMicLevel();
}


void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("RECORD.RAW");
  }
  frec = SD.open("RECORD.RAW", FILE_WRITE);
  if (frec) {
    queue1.begin();
    mode = 1;
  }
}
// Unterschied Raw zu Wave ist nur der Header bei Wave -> Filegröße im Header nach der Aufnahme einfügen
// Code im Moodle-Kurs (Java)
//
//Jump-Befehl: goto
// goto xxx;
// ........
// xxx:
//.........

// RIFF-Header:
// chunkID (char[4], "RIFF")
// ChunkSize (unit32_t, = Dateilänge in Bytes-8;)
// riffType (char[4], "WAVE")
// => 12 Byte lang

// Format-Abschnitt:
// beginnt mit Kennung 'fmt ', genau einmal enthalten
// Auf ChunkSize folgt Inhalt, bestehend aus allgemeinem und speziellem Teil
//    Allgemein: wFormatTag: Format-Identifizierung,0x0001 für PCM (kanonisch, unkomprimiert)
//               wChannels(uint16_t)
//               dwSamplesPerSec (uint32_t): Abtastrate in Hz
//               dwAvgBytesPerSec (unint32_t): nötige Übertragungsbandbreite (wenn keine Kompression verwendet: Produkt aus Abtastrate und Framegröße
//               wBlockAlign (unint16_t): Größe der Frames in Bytes, für PCM: wBlockAlign = wChannels * ((wBitsPerSample + 7) / 8);
//    Für PCM





String riffID = "RIFF";    
int fileSize = "    ";      // Länge des Files - 8 Bytes
String riffType = "WAVE";

String formatID = "fmt ";   // Start des Format-Chunks
int formatLen = 16;         // Länge des restlichen Headers
int formatType = 1;         // 1 = PCM = kanonisch, unkomprimiert 
int nrOfChannels = 1;       // 1 = Mono, 2 = Stereo
int sampleRate = 44100;
int bytesPerSec = (sampleRate * bitsPerSample * nrOfChannels) / 8;
int blockAlign = (bitsPerSample * nrOfChannels) / 8;
int bitsPerSample = 16;

String dataID = "data";     // Start des Data-Chunks
int dataSize = "    ";      // Länge des Data-Chunks

byte headerBuffer[];

//frec.write((byte*)queue1.readBuffer(), 256);   //write(buf, len),   buf = byte-array, len = anzahl El. in buf
//frec.write(headerBuffer, 44);
/* write WAV (RIFF) header
    public void writeWavHeader() {

        byte[] GROUP_ID = "RIFF".getBytes();
        byte[] RIFF_TYPE = "WAVE".getBytes();
        byte[] FORMAT_ID = "fmt ".getBytes();
        byte[] DATA_ID = "data".getBytes();
        short FORMAT_TAG = 1; // PCM
        int FMT_LENGTH = 16;

        short bitsize = 16; // TODO

            RandomAccessFile raFile = new RandomAccessFile(file, "rw");

            int fileLength = (int) raFile.length(); // [bytes]
            int chunkSize = fileLength - 8;
            int dataSize = fileLength - 44;
            short blockAlign = (short) ((channels) * (bitsize % 8));
            int bytesPerSec = samplerate * blockAlign;

            // RIFF-Header
            raFile.write(GROUP_ID);
            raFile.writeInt(Integer.reverseBytes(chunkSize));
            raFile.write(RIFF_TYPE);

            // fmt
            raFile.write(FORMAT_ID);
            raFile.writeInt(Integer.reverseBytes(FMT_LENGTH));
            raFile.writeShort(Short.reverseBytes(FORMAT_TAG));
            raFile.writeShort(Short.reverseBytes((short) channels));
            raFile.writeInt(Integer.reverseBytes(samplerate));
            raFile.writeInt(Integer.reverseBytes(bytesPerSec));
            raFile.writeShort(Short.reverseBytes(blockAlign));
            raFile.writeShort(Short.reverseBytes(bitsize));

            // data
            raFile.write(DATA_ID);
            raFile.writeInt(Integer.reverseBytes(dataSize));

            raFile.close();

    }
*/    
void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    //elapsedMicros usec = 0;
    frec.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  mode = 0;
}


void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play("RECORD.RAW");
  mode = 2;
}

void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playRaw1.stop();
  mode = 0;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
