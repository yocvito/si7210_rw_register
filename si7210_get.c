#include <unistd.h>		//Needed for I2C port
#include <fcntl.h>		//Needed for I2C port
#include <sys/ioctl.h>	 	//Needed for I2C port
#include <linux/i2c-dev.h> 	//Needed for I2C port
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define OTP_BUSY_MASK				0x1
#define DSPSIGM_REGADDR				0xC1
#define DSPSIGM_FIELD_MASK			63
#define DSPSIGL_REGADDR				0xC2
#define DSPSIGL_FIELD_MASK			127



// Files
int file_i2c;

int addr = 0x31;
uint8_t buffer[2];
uint8_t register_addr;
uint8_t str;
char *end;

/*!
 *	Wake device from sleep
 */
static int wakeDevice()
{
	uint8_t null = 0;
	if (write(file_i2c, &null, 1) != 1)		
	{
		return 0;
	}
    return 1;
}

/*!
 *	put device to sleep
 */
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

/*!
 *	displays an error message when usage of program is bad
 */
void usage(char *progname)
{
	fprintf(stderr, "Usage: \t%s -otp [register_addr]\n       \t%s -pt\n",progname,progname);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

	if( argc > 1)
    {
        switch(argc)
		{
			case 2:
					if(strcmp(argv[1],"-pt") != 0)
						usage(argv[0]);
				break;
			
			case 3:
					if(strcmp(argv[1],"-otp") != 0 )
						usage(argv[0]);
				break;

			default:
				usage(argv[0]);
				break;
		}
    }
		//----- OPEN THE I2C BUS -----
	char *filename = (char *)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		fprintf(stderr, "Error open : Failed to open the i2c bus\n");
		return EXIT_FAILURE;
	}

	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		fprintf(stderr,"Failed to acquire bus access and/or talk to slave.\n");
		return EXIT_FAILURE;
	}

	wakeDevice();

	if(argc == 3)
	{
		//print a register value memorized in otp
		register_addr = 0xE3;
		if(write(file_i2c, &register_addr, 1) == 1)
		{
			if (read(file_i2c, &str, 1) != 1)
			{
				fprintf(stderr"Failed to read 0xE3.\n");
			}
			else
			{
				if((str & OTP_BUSY_MASK) == 0)
				{
					//otp_reg
					char buff[6];
					strcpy(buff, argv[2]);
					buffer[0] = 0xE1;
					buffer[1] = (int)strtol(buff, &end, 16);
					if (write(file_i2c, &buffer, 2) != 2)		
					{
						fprintf(stderr,"Failed to write otp register addr.\n");
					}
					
					//otp_read_en
					buffer[0] = 0xE3;
					buffer[1] = 0x02;
					if (write(file_i2c, &buffer, 2) != 2)		
					{
						fprintf(stderr,"Failed to write otp read bit.\n");
					}
					else
					{
						register_addr = 0xE2;
						if(write(file_i2c, &register_addr, 1) == 1)
						{					
							if (read(file_i2c, &str, 1) != 1)
							{
								fprintf(stderr,"Failed to read otp_data of %s.\n",argv[2]);
							}
							else
							{
								printf("Data on otp at address %s : 0x%02X\n\n",argv[2],str);
							}
						}
					}							
				}
				else
				{
					fprintf(stderr,"Error read OTP : OTP_BUSY\n");
				}
				
			}
		}
		else
		{
			fprintf(stderr,"Failed to write 0xE3 address.\n");
		}	
	}
	else if(argc == 2)
	{
		int dspsigm60 = 0;
		int dspsigl70 = 0;
		
		//print all registers every 2s with the magnetic field value
		while(true)
		{
			system("clear");

			wakeDevice();

			for( int i = 0xC0 ; i<0xE5 ; i++)
			{
				if( i < 0xD1 || i > 0xE0)
				{
					register_addr = i;
					if(write(file_i2c, &register_addr, 1) == 1)
					{
						if (read(file_i2c, &str, 1) != 1)
						{
							printf("Failed to read to the i2c bus.\n");
						}
						else
						{
							printf("data on 0x%02X : 0x%02X\n",register_addr,str);

							if(register_addr == DSPSIGM_REGADDR)
								dspsigm60 = ((int)str & DSPSIGM_FIELD_MASK);

							if(register_addr == DSPSIGL_REGADDR)
								dspsigl70 = ((int)str & DSPSIGL_FIELD_MASK);
						}
					}
				}
			}

			//Print the formula for the magnetic field
			printf("\n(256 * Dspsigm[6:0] + Dspsigl[7:0] - 16384) * (0.00125 or 0.0125)\n");
			printf("(256 * %d + %d - 16384) * (0.00125) = %f\n",dspsigm60, dspsigl70, fabsf((256*dspsigm60+dspsigl70-16384)*(0.00125)) );
			printf("(256 * %d + %d - 16384) * (0.0125) = %f\n\n",dspsigm60, dspsigl70, fabsf((256*dspsigm60+dspsigl70-16384)*(0.0125)) ); 

			sleep(2);

			asleepDevice();
		}
	}
	else if(argc == 1)
	{
		//print all registers one time
		for( int i = 0xC0 ; i<0xE5 ; i++)
		{
			if( i < 0xD1 || i > 0xE0)
			{
				register_addr = i;
				if(write(file_i2c, &register_addr, 1) == 1)
				{
					if (read(file_i2c, &str, 1) != 1)
					{
						printf("Failed to read to the i2c bus.\n");
					}
					else
					{
						printf("data on 0x%02X : 0x%02X\n",register_addr,str);
					}
				}
			}
		}
	}

	if(!asleepDevice())
	{
		fprintf(stderr,"Failed to put to sleep the device.\n");
	}

	close(file_i2c);
	return EXIT_SUCCESS;
}
