#include <iostream>
#include "Ezo_I2c_lib/Ezo_i2c.h"
#include <unistd.h>

enum EZO_STATE {STATE_REQUEST, STATE_READ};

int main(int argc, char** argv) {
    Ezo_board PH("/dev/i2c-1", 0x63);
    enum EZO_STATE state = STATE_REQUEST;

    while (1) {
        switch (state) {
            case STATE_REQUEST: {
                PH.send_read_cmd();
                state = STATE_READ;
                break;
            }
            case STATE_READ: {
                usleep(900000);
                PH.receive_read_cmd();

                switch (PH.get_error()) {
                    case (Ezo_board::SUCCESS):
                        std::cout << "SUCCESS\n";
                        break;
                    case (Ezo_board::FAIL):
                        std::cout << "FAIL\n";
                        break;
                    case (Ezo_board::NOT_READY):
                        std::cout << "PENDING\n";
                        break;
                    default:
                        std::cout << "OTHER STATUS: " << PH.get_error() << "\n";
                        break;
                }

                state = STATE_REQUEST;
                break;
            }
        }
        sleep(1);
    }

    return 0;
}