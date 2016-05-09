// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// MS8607_02BA
// This code is designed to work with the MS8607_02BA_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class MS8607_02BA
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus Bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, MS8607_02BA I2C address is 0x76(118)
		I2CDevice device_PT = Bus.getDevice(0x76);
		
		// Reset command
		device.write((byte)0x1E);
		Thread.sleep(500);

		// Read 12 bytes of calibration data
		// Pressure sensitivity | SENST1
		byte[] data1= new byte[2];
		device.read(0xA2, data1, 0, 2);
		// Pressure offset | OFFT1
		byte[] data2= new byte[2];
		device.read(0xA4, data2, 0, 2);
		// Temperature coefficient of pressure sensitivity | TCS
		byte[] data3= new byte[2];
		device.read(0xA6, data3, 0, 2);
		// Temperature coefficient of pressure offset | TCO
		byte[] data4= new byte[2];
		device.read(0xA8, data4, 0, 2);
		// Reference temperature | TREF
		byte[] data5= new byte[2];
		device.read(0xAA, data5, 0, 2);
		// Temperature coefficient of the temperature | TEMPSENS
		byte[] data6= new byte[2];
		device.read(0xAC, data6, 0, 2);

		// Convert the data
		int c1 = ((data1[0] & 0xFF) * 256 + (data1[1] & 0xFF));
		int c2 = ((data2[0] & 0xFF) * 256 + (data2[1] & 0xFF));
		int c3 = ((data3[0] & 0xFF) * 256 + (data3[1] & 0xFF));
		int c4 = ((data4[0] & 0xFF) * 256 + (data4[1] & 0xFF));
		int c5 = ((data5[0] & 0xFF) * 256 + (data5[1] & 0xFF));
		int c6 = ((data6[0] & 0xFF) * 256 + (data6[1] & 0xFF));
		Thread.sleep(500);

		// Command to initiate pressure conversion(OSR = 256)
		device.write((byte)0x40);
		Thread.sleep(500);

		// Read 3 bytes of data, D1 msb2, D1 msb1, D1 lsb
		// Digital pressure value
		byte[] data = new byte[3];
		device.read(0x00, data, 0, 3);

		// Convert the data
		long D1= ((data[0] & 0xFF) * 256 * 256 + (data[1] & 0xFF) * 256 + (data[2] & 0xFF));

		// Command to initiate temperature conversion(OSR = 256)
		device.write((byte)0x50);
		Thread.sleep(500);

		// Read 3 bytes of data, D1 msb2, D1 msb1, D1 lsb
		// Digital temperature value
		byte[] data0  = new byte[3];
		device.read(0x00, data0, 0, 3);

		// Convert the data
		long D2= ((data0[0] & 0xFF) * 256 * 256 + (data0[1] & 0xFF) * 256 + (data0[2] & 0xFF));
		long dT = D2 - c5 * 256;
		long Temp = 2000 + dT * c6 / (long)8388608;
		long OFF = c2 * 131072L + (c4 * dT) / 64L;
		long SENS = c1 * 65536L + (c3 * dT ) / 128L;
		
		long Ti = 0;
		long OFFi = 0;
		long SENSi = 0;
		if(Temp >= 2000)
		{
			Ti = 5 * (dT * dT)/274877906944L;
			OFFi = 0;
			SENSi= 0;
		}
		else if(Temp < 2000)
		{
			Ti = 3 * (dT * dT) / 8589934592L;
			OFFi= 61 * ((Temp - 2000) * (Temp - 2000)) / 16;
			SENSi= 29 * ((Temp - 2000) * (Temp - 2000)) / 16;
			if(Temp < -1500)
			{
				OFFi = OFFi + 17 * ((Temp + 1500) * (Temp + 1500)) ;
				SENSi = SENSi + 9 * ((Temp + 1500) * (Temp + 1500));
			}
		}
		long OFF2 = OFF - OFFi;
		long SENS2= SENS - SENSi;
		double pressure = ((D1 * SENS2/2097152 - OFF2)/32768) /100;
		double cTemp = (Temp - Ti) / 100.0;
		double fTemp =  cTemp * 1.8 + 32;

		// Get I2C device, MS8607_02BA01 humditiy I2C address is 0x40(64)
		I2CDevice device_H = Bus.getDevice(0x40);

		// Send reset command
		device_H.write((byte)0xFE);
		Thread.sleep(100);
		// Send humditiy measurement command, NO HOLD MASTER
		device_H.write((byte)0xF5);
		Thread.sleep(500);

		// Read 2 bytes of data
		device_H.read(data, 0, 2);

		// Convert the data
		int hum = ((data[0] & 0xFF) * 256 + (data[1] & 0xFF));
		double humditiy = (-6.0 + 125.0 * hum / 65536.0) * 100;
		
		// Output data to screen
		System.out.printf("Relative Humidity : %.2f %%RH %n", humditiy);
		System.out.printf("Pressure : %.2f mbar %n", pressure);
		System.out.printf("Temperature in Celsius : %.2f C %n", cTemp);
		System.out.printf("Temperature in Fahrenheit : %.2f C %n", fTemp);
		}
}
