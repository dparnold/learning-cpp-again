/* to compile execute:
 *  gcc -std=c99 templogger.c -lbcm2835 -o ../templogger.out
 *
 *
 */

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include <bcm2835.h>

/* I2C registers in ADT74x0 */
#define T_MSB           0x00
#define T_LSB           0x01
#define STATUS          0x02
#define CONFIG          0x03
#define IDREG           0x0b
#define RESET           0x2f
#define LEDCONTROL  0x14

/* measurement modes */
#define CONT16B         0x80
#define ONESHOT16B      0xA0

#define INTERVALL       300     //measurement interval in seconds

/* bool */
#define TRUE            0x00
#define FALSE           0x01

/* devices */
#define MCPIO           0x20
#define SENSOR1         0x48
#define SENSOR2         0x49
#define SENSOR3         0x4a
#define SENSOR4         0x4b

#define OUTFILE         0x200

/* error codes */
#define ADTNOTFOUND     0x10
#define ADTMISSMATCH    0x20
#define ADTSETUPERR     0x30
#define ADTWRITEERR     0x40
#define ADTREADERR      0x50

#define FILEERROR       0x100

unsigned char errorbit;         //status of red LED in binary
unsigned char runningbit;       //status of yellow LED in binary
unsigned char measuringbit;     //status of green LED in binary
unsigned char allbits;          //status of all LED combined in binary
#define NOLED   0x07        //all LEDs off

int init_sensor(const uint8_t address);
int set_sensor(const uint8_t address, int mode);
int read_sensor(const uint8_t address, double *temp);

void setLED();
void setError(int device, int errorcode);
void setRunning(int iRunning);
void setMeasuring(int iMeasuring);

void getTimeStamp(char *timestamp, int length);

void exithandler(int signal);


int main(int argc, const char *argv[])
{
  printf("\033[2J");

  printf("Ion-Lab temperature logging\n");
  printf("version 1.1 - Dec 21 2015\n \n");
  printf("program running - press CTRL+C to quit\n\n");

  signal(SIGINT, exithandler);


  bcm2835_init();
  bcm2835_i2c_begin();

  // slowing down to 10kHz (std is 100kHz) works better
  // when the cables are long and termination dodgy...
  bcm2835_i2c_set_baudrate(10000);

  //init MCP23017 IO-chip
  uint8_t buffer[2];
  int status;
  bcm2835_i2c_setSlaveAddress(MCPIO);
  buffer[0] = 0x00;
  buffer[1] = 0x00;
  status = bcm2835_i2c_write( (char *)buffer, 2);
  if (status != 0)
  {
      setError(MCPIO, status);
  }
  setRunning(TRUE);

  int8_t sensors[4];    // Keep track of the status of all sensors
  int run = 0;          // handler to break loop if required
  int file_error = 0;   //
  FILE *file;

  //LOOP
  while(run == 0)
  {
    setError(0,0);

  //initialize sensors
  for(int i = 0; i < 4; i++)
   {
     int status = init_sensor(i+0x48);
     sensors[i]  =  status;
     if(status != 0)
     {
      //error
      printf("no ADT7420 found with address: 0x%02x \n",i+0x48);
     }
     else
     {
       printf("found ADT7420 with address: 0x%02x:\n",i+0x48);
     }
   }

   printf("\n starting measurement...\n \n");


   //set sensors to measurement mode
   setMeasuring(TRUE);
   for (int i = 0; i < 4; i++)
   {
     if(sensors[i] == 0)
     {
      int status = set_sensor(i+0x48, ONESHOT16B);
      sensors[i]  =  status;
      if(status != 0)
      {
        //error   //error code in set_sensor

      }
     }
   }

   //wait for sensors to complete measurement
   sleep(1);

   file = fopen("data/tempdata.dat","a");
   if (file == NULL)
   {
     printf("error opening data file\n");
     setError(OUTFILE, FILEERROR);

   }
   else
   {
    //write timestamp
    char timestamp[30];
    getTimeStamp(timestamp, sizeof(timestamp) );
    fprintf(file,"%s\t", timestamp);

   }

   //read sensors
   for (int i = 0; i < 4; i++)
   {
     double temperature;

     if(sensors[i] == 0)
     {

      int status = read_sensor(i+0x48, &temperature);
      sensors[i]  =  status;
      if(status != 0)
      {
        //error   //error code in read_sensor

      }
      else
      {
        //measurement successful
        printf("sensor 0x%02x: %8f\n",i+0x48, temperature);
      }
     }
     else
     {
       printf("sensor 0x%02x not connected\n",i+0x48);
       temperature = -1000.;
     }

     //write data to file
     if (file != NULL)
     {
       fprintf(file,"%8f\t", temperature);
     }

        }

        if (file != NULL)
        {
                fprintf(file,"\r\n");
                fclose(file);
        }

        setMeasuring(FALSE);

   sleep(INTERVALL-1); // 1 second sleep during data taking
   printf("\033[11A"); // move up

  }  //ENDOFLOOP

}


/* Search for sensors and initialize the ones found
 *
 *
 */
int init_sensor(const uint8_t address)
{
  int status = 0;
  uint8_t buffer[2];

  bcm2835_i2c_setSlaveAddress(address);

  char reg = IDREG;
  status = bcm2835_i2c_read_register_rs(&reg, (char *)buffer,1);
  if (status != 0)
  {
    status = ADTNOTFOUND;
    return (status);

  }
  else
  {
    if (buffer[0] != 0xcb)
    {
      //error
      status = ADTMISSMATCH;
      setError(address, status);
      return(status);
    }
    else
    {
      //reset device
      buffer[0] = RESET;
      status = bcm2835_i2c_write((char *)buffer,1);
      if (status != 0)
      {
        //error
        status = ADTWRITEERR;
        setError(address, status);
        return (status);
      }

      bcm2835_delay(1); // Device needs 200us after reset, give it 1ms

      return (status);
    }
  }

}

/* Set sensor in measurement mode
 *
 *
 */
int set_sensor(const uint8_t address, int mode)
{
  int status = 0;
  uint8_t buffer[2];

  bcm2835_i2c_setSlaveAddress(address);

  // set chip for 16 bit accuracy
  buffer[0] = CONFIG;
  buffer[1] = ONESHOT16B; // = mode
  status = bcm2835_i2c_write( (char *)buffer, 2);
  if (status != 0)
  {
    //error
    status = ADTWRITEERR;
    setError(address, status);
    return (status);
  }
  return (status);
}




/* read sensor
 *
 */
int read_sensor(const uint8_t address, double *temp)
{
  int status;
  uint8_t buff[2];

  bcm2835_i2c_setSlaveAddress(address);

  char reg = T_MSB;
  status = bcm2835_i2c_read_register_rs(&reg, (char *)buff, 2);
  if(status != 0)
  {
    //error
    status = ADTREADERR;
    setError(address, status);
    *temp = -1000.;
    return(status);
  }

  // convert data
  // ADT74x0 puts MSB first so flip order
  int16_t hi = buff[0];
  int16_t lo = buff[1];

  int16_t t128 = hi << 8 | lo;

  *temp = t128 / 128.0;

  return (status);

}

void setLED()
{
  allbits = errorbit + runningbit + measuringbit;

  uint8_t buffer[2];
  int status;

  bcm2835_i2c_setSlaveAddress(MCPIO);

  // set chip for 16 bit accuracy
  buffer[0] = LEDCONTROL;
  buffer[1] = allbits;
  status = bcm2835_i2c_write( (char *)buffer, 2);
  if (status != 0)
  {
      printf("communication error with LED");
      setError(MCPIO, status);
  }

}

//turn error LED (red) on or off
void setError(int device, int errorcode)
{
  if (device == 0 && errorcode == 0)
  {
    errorbit = 0b00000100;
  }
  else
  {
    errorbit = 0b0000000;
    // open logfile and print timestamp

    FILE *logfile;

   logfile = fopen("data/error.log","a");
   if (logfile == NULL)
   {
     printf("error opening log file\n");

   }
   else
   {
    //write timestamp
    char timestamp[30];
    getTimeStamp(timestamp, sizeof(timestamp) );
    fprintf(logfile,"%s\t", timestamp);

    //write device ID
    switch(device)
    {
      case OUTFILE :
      {
        fprintf(logfile,"file:\t");
        break;
      }
        case MCPIO :
      {
        fprintf(logfile,"MCP23017 IO port expander (the thing controlling the LED):\t");
        break;
      }
      case SENSOR1 :
      {
        fprintf(logfile,"sensor 1 (0x%02x):\t", device);
        break;
      }
      case SENSOR2 :
      {
        fprintf(logfile,"sensor 2 (0x%02x):\t", device);
        break;
      }
      case SENSOR3 :
      {
        fprintf(logfile,"sensor 3 (0x%02x):\t", device);
        break;
      }
      case SENSOR4 :
      {
        fprintf(logfile,"sensor 4 (0x%02x):\t", device);
        break;
      }
      default :
      {
        fprintf(logfile,"undefined device: (0x%02x):\t", device);
      }
    }

    //write error code
    switch(errorcode)
    {
      case ADTNOTFOUND :
      {
        fprintf(logfile,"sensor not found\t");
        break;
      }
      case ADTMISSMATCH :
      {
        fprintf(logfile,"sensor has wrong ID\t");
        break;
      }
      case ADTSETUPERR :
      {
        fprintf(logfile,"error initializing sensor\t");
        break;
      }
      case ADTWRITEERR :
      {
        fprintf(logfile,"error writing to device\t");
        break;
      }
      case ADTREADERR :
      {
        fprintf(logfile,"error reading from device\t");
        break;
      }
      case FILEERROR :
      {
        fprintf(logfile,"error opening file\t");
        break;
      }
         default :
      {
        fprintf(logfile,"undefined error code: (0x%02x)\t", errorcode);
      }

    }
    //close logfile
    fprintf(logfile,"\r\n");
    fclose(logfile);

   }
  }

  setLED();
}

//turn running LED (yellow) on or off
void setRunning(int iRunning)
{
  if (iRunning == TRUE)
    runningbit = 0b00000000;
  else
    runningbit = 0b00000010;

  setLED();
}

//turn measuring LED (green) on or off
void setMeasuring(int iMeasuring)
{
  if (iMeasuring == TRUE)
    measuringbit = 0b00000000;
  else
    measuringbit = 0b00000001;

  setLED();
}

void getTimeStamp(char *timestamp, int length)
{
    time_t rawtime = time(0);
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (timestamp,length,"%Y-%m-%d\t%H:%M:%S",timeinfo);
}

void exithandler(int signal)
{
  printf("closing program\n");
  printf("\033[2J");
  //cleanup

  //turn off LED
  uint8_t buffer[2];
  int status;

  bcm2835_i2c_setSlaveAddress(MCPIO);

  buffer[0] = LEDCONTROL;
  buffer[1] = NOLED;
  status = bcm2835_i2c_write( (char *)buffer, 2);
  if (status != 0)
  {
      setError(MCPIO, status);
  }

  exit(0);
}
