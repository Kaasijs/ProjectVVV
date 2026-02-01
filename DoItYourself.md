# Hoe maak ik mijn eigen drone!

## Onderdelen
Dit is alles dat je nodig hebt om de drone temaken:
<img alt="afbeelding" src="https://github.com/Kaasijs/ProjectVVV/blob/main/art/Benodigdheden.png" />
Naast deze onderdelen heb je ook een frame nodig. Als je een 3D-printer hebt, kun je [deze](https://github.com/Kaasijs/ProjectVVV/blob/main/models/Both.stl) zelf uitprinten. Het frame zorgt ervoor dat alle onderdelen stevig op hun plek blijven en dat de armen op de juiste afstand van elkaar zitten.
Daarnaast is het sterk aan te raden om een eigen soldeerstation te hebben. Hiermee kun je alle verbindingen netjes temaken. Een goede soldeerbout, soldeertin en eventueel krimpkous of isolatietape helpen om de elektronica veilig en netjes aan te sluiten.

## Solderen
LETOP bij het solderen is het belangrijk om niet je vingers te verliezen. kijk [dit](https://youtu.be/4YCihI_FIHg?t=10) filmpje als je het nognooit gedaan heb
Zorg er voor dat je naukurig de connecties volgens het diagram volgt, en geen kortsluiting veroorzaakt met de batterij.
Ik raad aan om de armen dus: mosfet, motor en weerstanden eerst tedoen. dit maakt het leven een stuk makelijker!
Dit zijn alle connecties die vorzien moeten worden van kables:
<img alt="afbeelding" src="https://github.com/Kaasijs/ProjectVVV/blob/main/art/Drone%20Diagram.png" />

hij moet er ongeveer zo komen uit tezien:
<img alt="afbeelding" src="https://github.com/Kaasijs/ProjectVVV/blob/main/art/Drone.png" />


## Firmware
Om dit programma op een eigen ESP32 te zetten, moet eerst de [Arduino IDE](https://www.arduino.cc/) worden gedownload. Na installatie moet in de Arduino IDE de ondersteuning voor de ESP32 worden toegevoegd via de Board Manager. Hiervoor wordt in de instellingen de ESP32 board URL toegevoegd en daarna wordt het ESP32 boardpakket geïnstalleerd.
Daarna moeten de afhankelijkheden worden geïnstalleerd via de "Library Manager" in de Arduino IDE. Dit zijn de [Bluepad32](https://github.com/ricardoquesada/bluepad32) library voor de Bluetooth controller en de [MPU9250_asukiaaa](https://github.com/asukiaaa/MPU9250_asukiaaa) library voor de gyroscoop en versnellingsmeter. Daarna wordt de ESP32 met een USB-kabel aangesloten op de computer. In de Arduino IDE wordt het juiste board (bijvoorbeeld Wemos D1 Mini ESP32) en de juiste COM-poort geselecteerd. en met de upload-knop naar de ESP32 worden geüpload.
Na het uploaden start de ESP32 automatisch met het uitvoeren van het droneprogramma.

## Controler
Je kunt vrijwel alle Bluetooth gamepad controllers gebruiken om de drone te besturen. Ik raad aan om de (nieuwe versie van de) Xbox controller te gebruiken, omdat deze automatisch blijft zoeken naar een verbinding als hij de verbinding verliest. Dit maakt het gebruik betrouwbaarder tijdens het vliegen.

Wanneer de drone opstart, zet je de controller in pairing mode. De ESP32 zal de controller automatisch detecteren en verbinden. Zodra de verbinding tot stand is gebracht, kun je direct beginnen met besturen.
BOOYAH!
