#include <Bela.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h> // Add this header for rt_printf
#include <libraries/OscSender/OscSender.h>
#include <libraries/OscReceiver/OscReceiver.h>

OscReceiver oscReceiver;
OscSender oscSender;
int localPort = 7562;
int remotePort = 7563;
const char* remoteIp1 = "127.0.0.1";
const char* remoteIp2 = "192.168.7.1";

#define I2C_BUS 1 // Use 0 for the other I2C channel on Bela

struct Sensor {
    int file;
    int address;
    int xMag, yMag, zMag;
};

Sensor sensors[] = {
    {0, 0x0C, 0, 0, 0},
    {0, 0x0D, 0, 0, 0},
    {0, 0x0E, 0, 0, 0},
    {0, 0x0F, 0, 0, 0},
};

AuxiliaryTask I2CTask;

void setupSensor(Sensor& sensor);
void readSensorData(Sensor& sensor);
void ReadSensor(void*);

bool setup(BelaContext *context, void *userData)
{
    oscSender.setup(remotePort, remoteIp1);
    oscSender.setup(remotePort, remoteIp2);
    

    // Create I2C bus
    char bus[] = "/dev/i2c-1";

    for (int i = 0; i < 4; i++) {
        if ((sensors[i].file = open(bus, O_RDWR)) < 0) {
            rt_printf("Failed to open the I2C bus for sensor %d.\n", i);
            return false;
        }
        setupSensor(sensors[i]);
    }

    // Initialise auxiliary tasks
    if ((I2CTask = Bela_createAuxiliaryTask(&ReadSensor, 60, "sensors_read")) == 0)
        return false;

    return true;
}

void setupSensor(Sensor& sensor) {
    if (ioctl(sensor.file, I2C_SLAVE, sensor.address) < 0) {
        rt_printf("Failed to get I2C device for sensor at address 0x%X.\n", sensor.address);
        return;
    }

    // Write register command, AH = 0x00, AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5(0x60, 0x00, 0x5C, 0x07)
    // Address register, (0x00 << 2)
    char config[4] = {0x00, 0x00, 0x5C, 0x07};
    write(sensor.file, config, 4);

    // Read 1 Byte of data (status byte)
    char data[7]; // Add this line to declare the data array
    read(sensor.file, data, 1);

    // Write register command, AH = 0x02, AL = 0xB4, RES for magnetic measurement = 0(0x60, 0x02, 0xB4, 0x08)
    // Address register, (0x02 << 2)
    config[0] = 0x60;
    config[1] = 0x02;
    config[2] = 0xB4;
    config[3] = 0x08;
    write(sensor.file, config, 4);

    // Read 1 Byte of data (status byte)
    read(sensor.file, data, 1);
}

void ReadSensor(void*) {
    for (int i = 0; i < 4; i++) {
        

        readSensorData(sensors[i]);
        
        //rt_printf("Sensor%d: %d %d %d\n", i + 1, sensors[i].xMag, sensors[i].yMag, sensors[i].zMag);
        oscSender.newMessage("/mag").add(i + 1).add(sensors[i].xMag).add(sensors[i].yMag).add(sensors[i].zMag).send();
    }
}

void readSensorData(Sensor& sensor) {
    // Start single measurement mode, X, Y, Z-Axis enabled (0x3E)
    char config[1] = {0x3E};
    write(sensor.file, config, 1);

    // Read 1 byte (status)
    char data[7]; // Add this line to declare the data array
    read(sensor.file, data, 1);
    usleep(8000);

    // Send read measurement command, d(0x4E)
    config[0] = 0x4E;
    write(sensor.file, config, 1);

    // Read 7 bytes of data
    if (read(sensor.file, data, 7) != 7) {
        rt_printf("Error reading from the device at address 0x%X.\n", sensor.address);
        return;
    }

    // Convert the data
    sensor.xMag = (data[1] * 256 + data[2]);
    if (sensor.xMag > 32767) {
        sensor.xMag -= 65536;
    }

    sensor.yMag = (data[3] * 256 + data[4]);
    if (sensor.yMag > 32767) {
        sensor.yMag -= 65536;
    }

    sensor.zMag = (data[5] * 256 + data[6]);
    if (sensor.zMag > 32767) {
        sensor.zMag -= 65536;
    }
}

void render(BelaContext *context, void *userData)
{
    Bela_scheduleAuxiliaryTask(I2CTask);
}

void cleanup(BelaContext *context, void *userData)
{
}