#include <unistd.h>		//Needed for I2C port
#include <fcntl.h>		//Needed for I2C port
#include <sys/ioctl.h>	 	//Needed for I2C port
#include <linux/i2c-dev.h> 	//Needed for I2C port
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

// Files
int file_i2c;

int addr = 0x31;

int main(void)
{

	//----- OPEN THE I2C BUS -----
	char *filename = (char *)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		fprintf(stderr, "Error open : Failed to open the i2c bus\r\n");
		return EXIT_FAILURE;
	}

	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return EXIT_FAILURE;
	}

	uint8_t register_addr;
	uint8_t str;
	for( int i = 0xC0 ; i<0xE5 ; i++)
	{
		if( i < 0xD1 || i > 0xE0)
		{
			register_addr = i;
			if(write(file_i2c, &register_addr, 1) == 1)
			{
				if (read(file_i2c, &str, 1) != 1)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
				{
					printf("Failed to write to the i2c bus.\n");
					printf("%d\n",str);
				}
				else
				{
					printf("data on 0x%02X : 0x%02X\n",register_addr,str);
				}
			}
		}
	}
	close(file_i2c);
	return EXIT_SUCCESS;
}
