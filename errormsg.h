#ifndef ERROR_MSG
#define ERROR_MSG

#include <iostream>


void print_help_scanner() {
    std::cerr << "Usage: ./pds-scanner -i interface -f file" << std::endl;
}


void print_abort(std::string msg) {
    std::cerr << "ERROR: " << msg << std::endl;
    print_help_scanner();
    exit(-1);
}

#endif
