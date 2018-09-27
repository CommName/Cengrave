Software created for controling homemade laser engraving machine.

While you are reviewing my code keep in mind that I am using this project to learn GitHub and OpenCV. Also, this is my first ever real project.

OpenCV
This project uses opencv 3.4.2 compiled with CMake and mingw32-make.
You will need to add OPENCV3_DIR environment variable that points to OpenCV folder.Also, add "%OPENCV3_DIR%\install\x86\mingw\bin" to the Path variable. 
Get the latest source code here: https://github.com/opencv/opencv

Highrez inpout32
Be sure to include inpout32.bin file to your build folder. You can download bin file here: http://www.highrez.co.uk/downloads/inpout32/

QextSerialPort
For USB connection (to Arduino) QextSerialPort library is used. QextSerialPort provides an interface to old fashioned serial ports for Qt-based applications. 
http://qextserialport.github.io/