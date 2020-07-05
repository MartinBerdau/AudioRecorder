# AudioRecorder
Diese Seite enthält Code, um ein Aufnahmegerät mit einem Teensy + Audio Shield zu bauen.

## Hardware
Im folgenden wird die verwendete Hardware aufgelistet. Hierbei handelt es sich um die verwendeten und empfohlenen Komponenten.

- Teensy 4.0
- Teensy Audio Shield
- Nextion Basic Series
- 2 microSD-Karten: 1x für Nextion-Display,  1x für Teensy (FAT32, maximal 32 GB)
- XLR/Line-Kombi-Buchse
- MAX4466 mit Elektret-Mikrofon
- 100μF Kondensator
- Knopfzelle mit Halterung
- LiPo-Akku (3,7 V)
- Lademodul
- Wippschalter
- Kabel (am besten geschirmt)
- Gehäuse (+Schrauben)

## Software
Um das Programm auf den Teensy zu laden wird die Software Teensyduino benötigt. Diese kann vom [Teensy-Hersteller](https://www.pjrc.com/teensy/teensyduino.html) runtergeladen werden. Dadurch ist automatisch die [Audio-Library](https://github.com/PaulStoffregen/Audio) installiert, welche für das Programm benötigt wird.
Für die Kommunikation zwischen Teensy und Nextion wird die [ITEADLIB_Arduino_Nextion-Library](https://github.com/itead/ITEADLIB_Arduino_Nextion) verwendet. Diese wurde für das Aufnahmegerät geringfügig verändert, weshalb der veränderte Code ebenfalls hier runterzuladen ist.
Des Weiteren wird die AudioRecorderLibrary benötigt, welche Code enthält, welcher lediglich aus dem Hauptprogramm ausgegliedert wurde.

## Installation
Der Aufbau der Hardware kann dem Manual entnommen werden.

Um die [ITEADLIB_Arduino_Nextion-Library](https://github.com/itead/ITEADLIB_Arduino_Nextion) und die AudioRecorderLibrary einzubauen, müssen diese in den library-Ordner verschoben werden. Üblicherweise wird dieser hier installiert:
```
Dokumente/Arduino/libraries
```
Um die GUI zu installieren muss das File *DisplayRecorder.tft* aus dem Display-Ordner auf die für das Display bestimmte microSD-Karte geladen und in das Nextion-Display eingesetzt werden.

Das Hauptprogramm *RecorderNextion.ino* im gleichnamigen Ordner kann nun mit Teensyduino auf den Teensy hochgeladen werden.

## Lizenz
Der Code ist durch die [MIT-Lizenz](https://opensource.org/licenses/MIT) lizensiert.
