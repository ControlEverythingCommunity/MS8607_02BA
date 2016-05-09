[![MS8607-02BA](MS8607-02BA_I2CS.png)](https://www.controleverything.com/products)
# MS8607-02BA
MS8607-02BA Pressure, Humidity and Temperature Sensor.

The MS8607-02BA device provides a digital humidity, 24-bit pressure and temperature value.

This Device is available from ControlEverything.com [SKU: MS8607-02BA_I2CS]

https://www.controleverything.com/products

This Sample code can be used with Raspberry pi and Arduino.

## Java
Download and install pi4j library on Raspberry pi. Steps to install pi4j are provided at:

http://pi4j.com/install.html

Download (or git pull) the code in pi.

Compile the java program.
```cpp
$> pi4j MS8607-02BA.java
```

Run the java program.
```cpp
$> pi4j MS8607-02BA
```

## Python
Download and install smbus library on Raspberry pi. Steps to install smbus are provided at:

https://pypi.python.org/pypi/smbus-cffi/0.5.1

Download (or git pull) the code in pi. Run the program.

```cpp
$> python MS8607-02BA.py
```

## Arduino
Download and install Arduino Software (IDE) on your machine. Steps to install Arduino are provided at:

https://www.arduino.cc/en/Main/Software

Download (or git pull) the code and double click the file to run the program.

Compile and upload the code on Arduino IDE and see the output on Serial Monitor.
#####The code output is the relative humidity in %RH, pressure in mbar and temperature reading in degree celsius and fahrenheit.
