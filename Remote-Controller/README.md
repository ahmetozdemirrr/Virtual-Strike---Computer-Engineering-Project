# Remote Controller

This section is the remote control part of the Virtual Strike - Computer Engineering Project. In this project, serial communication and data transmission via WebSocket are performed using Raspberry Pi and Arduino.

## Contents

- `circuits-photos`: Directory containing circuit diagrams and setup photos.
- `script`: Directory containing startup scripts.
- `src`: Directory containing source codes.

## Setup Script

In the `script` directory, there is a startup script named `init.sh`. This script is used to run two different programs on the Raspberry Pi.

## Source Codes

There are two source files in the `src` directory:

1. `dummyController.py`: This Python script reads data from the serial port and prints the data to the screen.
2. `controller.cpp`: This C++ program reads data from sensors on the Raspberry Pi and sends this data to a remote server via a WebSocket.

## Installation and Running

1. Properly connect your Raspberry Pi and Arduino.
2. Run the `init.sh` script to start the necessary scripts:
    ```bash
    ./script/init.sh
    ```
3. Run the `dummyController.py` or `controller.cpp` file appropriately.

## Contact Us:
If you have any questions, suggestions, or feedback, feel free to reach out to us at [www.virtualstrike.com](https://virtualstrike.tech). We'd love to hear from you and are committed to making Virtual Strike the best it can be.

Thank you for your interest in Virtual Strike. Let the virtual shooting adventure begin!


![Gebze Technical University](https://abl.gtu.edu.tr/html/mobil/gtu_logo_en_500.png)
# CSE396 Computer Engineering Project

The course was given by Prof. Dr. Erkan Zergeroğlu at Gebze Technical University for the Department of Computer Engineering in the Spring 2024.

Links:
* [Computer Engineering Project Course](https://abl.gtu.edu.tr/ects/?duzey=ucuncu&modul=ders_bilgi_formu&dno=B%C4%B0L%20396&bolum=104&tip=lisans&dil=tr)
* [Prof. Dr. Erkan Zergeroğlu](https://www.gtu.edu.tr/tr/personel/98/10414/display.aspx)
* [Department of Computer Engineering](https://www.gtu.edu.tr/kategori/91/3/bilgisayar-muhendisligi.aspx?languageId=2)
* [Gebze Technical University](https://www.gtu.edu.tr/?languageId=2)

This module of the project was done by [Ahmet Özdemir](https://github.com/ahmetozdemirrr), [Doğukan Baş](https://github.com/Dogukan777), and [İlkay Bolat](https://github.com/ilkayofTarsus) (Source code and circuit design) [Yasir Güneş](https://github.com/yasirgunes) (remote-controller design, embedding source code with script).