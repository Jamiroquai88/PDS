#include "errormsg.h"

void print_help_scanner() {
    std::cerr << "Usage: ./pds-scanner -i interface -f file" << std::endl;
}


void print_msg_and_abort(std::string msg) {
    std::cerr << "ERROR: " << msg << std::endl;
    exit(-1);
}
