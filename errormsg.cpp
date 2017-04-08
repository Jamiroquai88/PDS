#include "errormsg.h"

void print_help_scanner() {
    std::cerr << "Usage: ./pds-scanner -i interface -f file" << std::endl;
}

void print_help_chooser() {
    std::cerr << "Usage: ./pds-chooser -f input_xml_file -o output_xml_file" << std::endl;
}

void print_msg_and_abort(std::string msg) {
    std::cerr << "ERROR: " << msg << std::endl;
    exit(-1);
}
