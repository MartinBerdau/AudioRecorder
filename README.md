# AudioRecorder

## Verwendete Komponenten:
- Teensy 4.0
- Teensy Audio Shield
- Nextion Basic Series
- 2 SD-Karten (1x für Nextion-Display, 1x für Teensy [FAT32, SDHC-Karte])

## MyLibrary installieren:
MyLibrary muss in Teensyduino unter "Sketch/Bibliothek einbinden/.ZIP-Bibliothek hinzufügen..." ausgewählt werden.
Zuvor sollte dieser Ordner daher in den vorgesehenen library-Ordner kopiert werden. Falls neue Files zu MyLibrary hinzugefügt wurde, muss MyLibrary im vorgesehenen library-Ordner durch die neue Version ersetzt werden.

## Nextion-Library installieren:
Link zum Runterladen:
https://github.com/itead/ITEADLIB_Arduino_Nextion

Je nachdem, wie die Komponenten verlötet wurden, muss in der Nextion-Library in NexConfig.h in Zeile 37 der zu verwendende Serial eingetragen werden (z.B. #define nexSerial Serial1, keinen Serial wählen, der bereits verwendet wird!).

## Ausführung:
Die tft-File muss auf die SD-Karte für das Nextion geladen werden..
Alternativ kann der Code für das Nextion-Display im Nextion-Editor in ein tft-File umgewandelt werden, welche im Anschluss auf die SD-Karte kopiert werden kann.
Der ino-Code muss auf den Teensy geladen werden. Das Display muss vor dem Teensy angeschaltet werden, ansonsten Funktioniert die Kommunikation möglicherweise nicht

## Eingebaute Features:
Speicherung:
- Abspeicherung als wav-Datei
Aufnahme:
- Pegelanzeige
- Zeitanzeige

Achtung: in dieser Version kann der Teensy noch keine Aufnahmen wiedergeben!
