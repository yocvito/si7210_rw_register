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

uint8_t register_addr;
uint8_t buffer[2] = { 0 };

int addr = 0x31;

char *end;
char buf[12];
int ibuff;

static int wakeDevice()
{
    buffer[0] = 0;
	buffer[1] = 0;
	if (write(file_i2c, &buffer, 2) != 2)		
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

int main(int argc, char**argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s register_address\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        char buff[6];
        strcpy(buff, argv[1]);
        register_addr = (int)strtol(buff, &end, 16);
    }

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

    if(!wakeDevice())
    {
        fprintf(stderr,"Failed to wake up si7210.\n");
        exit(EXIT_FAILURE);
    }

    //Write on a register
	uint8_t str; 
    buffer[0] = register_addr;
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

    //Read the same register
    if(write(file_i2c, &register_addr, 1) == 1)
    {
        if (read(file_i2c, &str, 1) != 1)
        {
            printf("Failed to read to the i2c bus.\n");
        }
        else
        {
            printf("Data on 0x%02X : 0x%02X\n",register_addr,str);
        }
    }

    //asleep the device
    if(!asleepDevice())
    {
        fprintf(stderr,"Failed to put to sleep the device.\n");
    }

	close(file_i2c);

	return EXIT_SUCCESS;
}
