# Hoe maak ik mijn eigen drone!

## Onderdelen
Dit is alles dat je nodig hebt om de drone temaken:
<img alt="afbeelding" src="https://github.com/Kaasijs/ProjectVVV/blob/main/art/Benodigdheden.png" />
Daarnaast heb je ook het frame nodig. Als je een 3D printer hebt kan je [deze](https://github.com/Kaasijs/ProjectVVV/blob/main/models/Both.stl) uitprinten
En het kan geen kwaad om je eigen soldeer station te hebben.

## Solderen
LETOP bij het solderen is het belangrijk om niet je vingers te verliezen. kijk [dit](https://youtu.be/4YCihI_FIHg?t=10) filmpje als je het nognooit gedaan heb
Zorg er voor dat je naukurig de connecties volgens het diagram volgt, en geen kortsluiting veroorzaakt met de batterij.
Dit zijn alle connecties die vorzien moeten worden van kables:
<img alt="afbeelding" src="https://github.com/Kaasijs/ProjectVVV/blob/main/art/Drone%20Diagram.png" />

## Firmware
Om dit programma op een eigen ESP32 te zetten, moet eerst de [Arduino IDE](https://www.arduino.cc/) worden gedownload. Na installatie moet in de Arduino IDE de ondersteuning voor de ESP32 worden toegevoegd via de Board Manager. Hiervoor wordt in de instellingen de ESP32 board URL toegevoegd en daarna wordt het ESP32 boardpakket geïnstalleerd.
Daarna moeten de afhankelijkheden worden geïnstalleerd via de "Library Manager" in de Arduino IDE. Dit zijn de [Bluepad32](https://github.com/ricardoquesada/bluepad32) library voor de Bluetooth controller en de [MPU9250_asukiaaa](https://github.com/asukiaaa/MPU9250_asukiaaa) library voor de gyroscoop en versnellingsmeter. Daarna wordt de ESP32 met een USB-kabel aangesloten op de computer. In de Arduino IDE wordt het juiste board (bijvoorbeeld Wemos D1 Mini ESP32) en de juiste COM-poort geselecteerd. en met de upload-knop naar de ESP32 worden geüpload.
Na het uploaden start de ESP32 automatisch met het uitvoeren van het droneprogramma.

## Controler
Je kan alle bluetooth gamepad controlers gebruiken om de drone tebesturen, ik raad aan een xbox controler omdat die blijft zoeken voor een verbinding als hij verbinding verliest. als de drone opstart zet je de controler in pairing mode, en klaar is kees! veel vlieg plezier!
