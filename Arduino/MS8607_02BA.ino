// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// MS8607_02BA
// This code is designed to work with the MS8607_02BA_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <Wire.h>

// MS8607_02BA I2C address for temperature and pressure measurement is 0x76(118)
#define Addr 0x76
// MS8607_02BA I2C address for humidity measurement is 0x40(118)
#define Addr1 0x40

unsigned long Coff[6];
unsigned long Ti = 0;
unsigned long offi = 0;
unsigned long sensi = 0;
unsigned int data[3];
void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication, set baud rate = 9600
  Serial.begin(9600);

  // Read cofficients values stored in EPROM of the device
  for (int i = 0; i < 6; i++)
  {
    // Start I2C Transmission
    Wire.beginTransmission(Addr);
    // Select data register
    Wire.write(0xA2 + (2 * i));
    // Stop I2C Transmission
    Wire.endTransmission();
    delay(500);

    // Request 2 bytes of data
    Wire.requestFrom(Addr, 2);

    // Read 2 bytes of data
    // Coff msb, Coff lsb
    if (Wire.available() == 2)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
    }
    // Convert the data
    Coff[i] = ((data[0] * 256.0) + data[1]);
  }
  delay(300);
}

void loop()
{
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Reset command
  Wire.write(0x1E);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Refresh pressure with the OSR = 256
  Wire.write(0x40);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(500);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Request 3 bytes of data
  Wire.requestFrom(Addr, 3);

  // Read 3 bytes of data
  // ptemp_msb1, ptemp_msb, ptemp_lsb
  if (Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }

  // Convert the data
  unsigned long ptemp = ((data[0] * 65536.0) + (data[1] * 256.0) + data[2]);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Refresh temperature with the OSR = 256
  Wire.write(0x50);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(500);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Request 3 bytes of data
  Wire.requestFrom(Addr, 3);

  // Read 3 bytes of data
  // temp_msb1, temp_msb, temp_lsb
  if (Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }

  // Convert the data
  unsigned long temp = ((data[0] * 65536.0) + (data[1] * 256.0) + data[2]);

  // Start I2C Transmission
  Wire.beginTransmission(Addr1);
  // Reset command
  Wire.write(0xFE);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr1);
  // Measure relative humidity, No hold master mode
  Wire.write(0xF5);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(500);

  // Request 2 bytes of data
  Wire.requestFrom(Addr1, 2);

  // Read 2 bytes of data
  // humidity_msb, humidity_lsb
  if (Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }

  // Convert the data
  float humidity = ((data[0] * 256.0) + data[1]);

  // Pressure, Temperature and Humidity Calculations
  // 1st order temperature and pressure compensation
  // Difference between actual and reference temperature
  unsigned long dT = temp - ((Coff[4] * 256));
  temp = 2000 + (dT * (Coff[5] / 8388608.0));
  unsigned long long off = (long long) Coff[1] * (long long)131072 + ((long long)Coff[3] * dT) / 64;
  unsigned long long sens = (long long)Coff[0] * (long long)65536 + ((long long)Coff[2] * dT) / 128;

  // 2nd order temperature and pressure compensation
  if (temp < 2000)
  {
    Ti = 3 * ((dT * dT) / 8589934592);
    offi = 61 * ((pow((temp - 2000), 2))) / 16;
    sensi =  29 * ((pow((temp - 2000), 2))) / 16;
    if (temp < - 1500)
    {
      offi = offi + 17 * ((pow((temp + 1500), 2)));
      sensi = sensi + 9 * ((pow((temp + 1500), 2)));
    }
  }
  else if (temp >= 2000)
  {
    Ti = 5 * (((long double)dT * (long double)dT) / 274877906944.0);
    offi = 0;
    sensi = 0;
  }

  // Adjust temp, off, sens based on 2nd order compensation
  temp -= Ti;
  off -= offi;
  sens -= sensi;

  // Convert the final data
  ptemp = (((ptemp * sens) / 2097152) - off);
  ptemp /= 32768.0;
  float pressure = ptemp / 100.0;
  float ctemp = temp / 100.0;
  float fTemp = ctemp * 1.8 + 32.0;
  humidity = -6.0 + (125.0 * humidity) / 65536.0;

  // Output data to serial monitor
  Serial.print("Pressure : ");
  Serial.print(pressure);
  Serial.println(" mbar");
  Serial.print("Relative Humidity : ");
  Serial.print(humidity);
  Serial.println(" %RH");
  Serial.print("Temperature in Celsius : ");
  Serial.print(ctemp);
  Serial.println(" C");
  Serial.print("Temperature in Fahrenheit : ");
  Serial.print(fTemp);
  Serial.println(" F");
  delay(500);
}
