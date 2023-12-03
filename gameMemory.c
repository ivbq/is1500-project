#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

// Helper functions for interacting with i2c memory 

/* Wait for I2C bus to become idle */
void i2c_idle() {
	while(I2C1CON & 0x1F || I2C1STAT & (1 << 14)); //TRSTAT
}

/* Send one byte on I2C bus, return ack/nack status of transaction */
uint8_t i2c_send(uint8_t data) {
	i2c_idle();
	I2C1TRN = data;
	i2c_idle();
	return !(I2C1STAT & (1 << 15)); //ACKSTAT
}

/* Receive one byte from I2C bus */
uint8_t i2c_recv() {
	i2c_idle();
	I2C1CONSET = 1 << 3; //RCEN = 1
	i2c_idle();
	I2C1STATCLR = 1 << 6; //I2COV = 0
	return I2C1RCV;
}

/* Send acknowledge conditon on the bus */
void i2c_ack() {
	i2c_idle();
	I2C1CONCLR = 1 << 5; //ACKDT = 0
	I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send not-acknowledge conditon on the bus */
void i2c_nack() {
	i2c_idle();
	I2C1CONSET = 1 << 5; //ACKDT = 1
	I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send start conditon on the bus */
void i2c_start() {
	i2c_idle();
	I2C1CONSET = 1 << 0; //SEN
	i2c_idle();
}

/* Send restart conditon on the bus */
void i2c_restart() {
	i2c_idle();
	I2C1CONSET = 1 << 1; //RSEN
	i2c_idle();
}

/* Send stop conditon on the bus */
void i2c_stop() {
	i2c_idle();
	I2C1CONSET = 1 << 2; //PEN
	i2c_idle();
}

#define EEPROM_ADDRESS 0b1010000 // Replace with the actual I2C address of your EEPROM

/* Write one byte to the EEPROM at a specific memory address */
void eeprom_write_byte(uint16_t memory_address, uint8_t data) {
    // Send start condition
    i2c_start();

    // Send device address with write bit
    i2c_send(EEPROM_ADDRESS | 0);

    // Send high and low bytes of the memory address
    i2c_send((uint8_t)(memory_address >> 8)); // MSB of memory address
    i2c_send((uint8_t)(memory_address & 0xFF)); // LSB of memory address

    // Send data byte
    i2c_send(data);

    // Send stop condition
    i2c_stop();
}

/* Read one byte from the EEPROM at a specific memory address */
uint8_t eeprom_read_byte(uint16_t memory_address) {
    uint8_t data;

    // Send start condition
    i2c_start();

    // Send device address with write bit
    i2c_send(EEPROM_ADDRESS | 0);

    // Send high and low bytes of the memory address
    i2c_send((uint8_t)(memory_address >> 8)); // MSB of memory address
    i2c_send((uint8_t)(memory_address & 0xFF)); // LSB of memory address

    // Send repeated start condition
    i2c_restart();

    // Send device address with read bit
    i2c_send(EEPROM_ADDRESS | 1);

    // Receive data byte
    data = i2c_recv();

    // Send not-acknowledge and stop condition
    i2c_nack();
    i2c_stop();

    return data;
}

void test_eeprom() {
    uint16_t test_address = 0x1234; // Replace with a valid address for your EEPROM
    uint8_t test_data = 0xAB; // Test data to write and then read

    // Write test data to EEPROM
    eeprom_write_byte(test_address, test_data);

    // Read data from EEPROM
    uint8_t read_data = eeprom_read_byte(test_address);

    // // Check if the written and read data are the same
    // if (read_data == test_data) {
    //     // If this prints, the EEPROM read/write functions are probably working
    //     printf("EEPROM test passed!\n");
    // } else {
    //     // If this prints, there's a problem with the EEPROM or the read/write functions
    //     printf("EEPROM test failed. Wrote: 0x%X, Read: 0x%X\n", test_data, read_data);
    // }
}

// 	/* Send start condition and address of the temperature sensor with
// 	write mode (lowest bit = 0) until the temperature sensor sends
// 	acknowledge condition */
// 	do {
// 		i2c_start();
// 	} while(!i2c_send(TEMP_SENSOR_ADDR << 1));
// 	/* Send register number we want to access */
// 	i2c_send(TEMP_SENSOR_REG_CONF);
// 	/* Set the config register to 0 */
// 	i2c_send(0x0);
// 	/* Send stop condition */
// 	i2c_stop();
	
// 	for(;;) {
// 		/* Send start condition and address of the temperature sensor with
// 		write flag (lowest bit = 0) until the temperature sensor sends
// 		acknowledge condition */
// 		do {
// 			i2c_start();
// 		} while(!i2c_send(TEMP_SENSOR_ADDR << 1));
// 		/* Send register number we want to access */
// 		i2c_send(TEMP_SENSOR_REG_TEMP);
		
// 		/* Now send another start condition and address of the temperature sensor with
// 		read mode (lowest bit = 1) until the temperature sensor sends
// 		acknowledge condition */
// 		do {
// 			i2c_start();
// 		} while(!i2c_send((TEMP_SENSOR_ADDR << 1) | 1));
		
// 		/* Now we can start receiving data from the sensor data register */
// 		temp = i2c_recv() << 8;
// 		i2c_ack();
// 		temp |= i2c_recv();
// 		/* To stop receiving, send nack and stop */
// 		i2c_nack();
// 		i2c_stop();
		
// 		s = fixed_to_string(temp, buf);
// 		t = s + strlen(s);
// 		*t++ = ' ';
// 		*t++ = 7;
// 		*t++ = 'C';
// 		*t++ = 0;
		
// 		display_string(1, s);
// 		display_update();
// 		delay(1000000);
// 	}
	
// 	return 0;
// }