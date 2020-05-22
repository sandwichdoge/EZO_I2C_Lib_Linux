// https://www.atlas-scientific.com/files/pH_EZO_Datasheet.pdf
#include "Ezo_i2c.h"
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

Ezo_board::Ezo_board(const char* dev, uint8_t address){
	this->fd = open(dev, O_RDWR);
    this->i2c_address = address;
}

Ezo_board::~Ezo_board() {
    close(this->fd);
}

uint8_t Ezo_board::get_address(){
    return i2c_address;
}

void Ezo_board::send_cmd(const char* command) {
    if (ioctl(this->fd, I2C_SLAVE, this->i2c_address) < 0) {
        printf("Failed to set i2c_slave address.\n");
        return;
    }
    write(this->fd, command, strlen(command));
    this->issued_read = false;
}

void Ezo_board::send_read_cmd(){
	send_cmd("R");
	this->issued_read = true;
}

void Ezo_board::send_cmd_with_num(const char* cmd, float num, uint8_t decimal_amount){
	std::string temp = std::string(cmd )+ std::string(num, decimal_amount);
	const char* pointer = temp.c_str();
	send_cmd(pointer);
}

void Ezo_board::send_read_with_temp_comp(float temperature){
	send_cmd_with_num("RT,", temperature, 3);
	this->issued_read = true;
}


enum Ezo_board::errors Ezo_board::receive_read_cmd(){
	char _sensordata[this->bufferlen] = {0};
	this->error = receive_cmd(_sensordata, bufferlen);

	if(this->error == SUCCESS){
		if(this->issued_read == false){
			this->error = NOT_READ_CMD;
		}
		else{
			this->reading = atof(_sensordata);
		}
	}

	return this->error;
}

bool Ezo_board::is_read_poll(){
	return this->issued_read;
}

float Ezo_board:: get_last_received_reading(){  
	return this->reading;
}

enum Ezo_board::errors Ezo_board::get_error(){
	return this->error;
}

void Ezo_board::send_custom_cmd(const char* cmd) {
    send_cmd(cmd);
}

// R | sleep(900ms) [Retcode] [string] [0]
enum Ezo_board::errors Ezo_board::receive_cmd( char * sensordata_buffer, uint8_t buffer_len) {
    uint8_t sensor_bytes_received = 0;
    uint8_t code = 0;
    uint8_t in_char = 0;

    memset(sensordata_buffer, 0, buffer_len);

    if (ioctl(this->fd, I2C_SLAVE, this->i2c_address)) {
        printf("Failed to set i2c_slave address.\n");
        return FAIL;
    }

    uint8_t buf[buffer_len];
    memset(buf, 0, buffer_len);

    while (1) {
        if (read(this->fd, buf, buffer_len) < 0) {
            printf("read() error\n");
            break;
        } else {
            code = buf[0];
            printf("[%d]%s\n", code, buf);
            memcpy(sensordata_buffer, buf + 1, buffer_len - 1); // NULL terminated.
            break;
        }
    }
    
    switch (code) {
        case 1:
        this->error = SUCCESS;
        break;

        case 2:
        this->error = FAIL;
        break;

        case 254:
        this->error = NOT_READY;
        break;

        case 255:
        this->error = NO_DATA;
        break;
    }

    return this->error;
}