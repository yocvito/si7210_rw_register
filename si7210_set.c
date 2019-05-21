#include <unistd.h>		
#include <fcntl.h>		
#include <sys/ioctl.h>	 	
#include <linux/i2c-dev.h> 	
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

// Files
int file_i2c;

uint8_t buffer[2] = { 0 };

int addr = 0x31;

char *end;
char buf[12];
int ibuff;

static int wakeDevice()
{
	uint8_t null = 0;
	if (write(file_i2c, &null, 1) != 1)		
	{
		return 0;
	}
    return 1;
}

static int asleepDevice()
{
    buffer[0] = 0xC4;
	buffer[1] = 0x08;
	if (write(file_i2c, &buffer, 2) != 2)		
	{
		return 0;
	}
    return 1;
}

int main(void)
{

	//----- OPEN THE I2C BUS -----
	char *filename = (char *)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		fprintf(stderr, "Error open : Failed to open the i2c bus\r\n");
		return EXIT_FAILURE;
	}

	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		return EXIT_FAILURE;
	}

	wakeDevice();

	for( int i = 0xC0 ; i<0xE5 ; i++)
	{
		if( i < 0xD1 || i > 0xE0)
		{
			buffer[0] = i;
			printf("Register value for 0x%02X : ",buffer[0]);
			do {
     				if (!fgets(buf, sizeof(buf), stdin))
        				break;
     				buf[strlen(buf) - 1] = 0;
     				buffer[1] = (int)strtol(buf, &end, 2);
			} while (end != buf + strlen(buf));
			printf("\n");
			if (write(file_i2c, &buffer, 2) != 2)		
			{
				printf("Failed to write to the i2c bus.\n");
			}
		}
	}

	asleepDevice();

	close(file_i2c);
	return EXIT_SUCCESS;
}
