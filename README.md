# AudioRecorder

## Verwendete Komponenten:
- Teensy 4.0
- Teensy Audio Shield
- Nextion Basic Series

## MyLibrary installieren:
MyLibrary muss in Teensyduino unter "Sketch/Bibliothek einbinden/.ZIP-Bibliothek hinzufügen..." ausgewählt werden.
Zuvor sollte dieser Ordner daher in den vorgesehenen library-Ordner kopiert werden. Falls neue Files zu MyLibrary hinzugefügt wurde, muss MyLibrary im vorgesehenen library-Ordner durch die neue Version ersetzt werden.

## Nextion-Library installieren:
Link zum Runterladen:
https://github.com/itead/ITEADLIB_Arduino_Nextion

Je nachdem, wie die Komponenten verlötet wurden, muss in der Nextion-Library in NexConfig.h in Zeile 37 der zu verwendende Serial eingetragen werden (z.B. #define nexSerial Serial1, keinen Serial wählen, der bereits verwendet wird!).

## Eingebaute Features:
- Pegelanzeige
