#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "errormsg.h"

/**
 * @brief Main function for pds-scanner.
 */
int main(int argc, char * argv[] ) {
    std::string interface("");
    std::string out_xml("");
    int c;
    while ((c = getopt (argc, argv, "i:f:")) != -1) {
        switch (c) {
            case 'i':
                interface = optarg;
                break;
            case 'f':
                out_xml = optarg;
                break;
        }
    }
    if (interface.length() == 0 || out_xml.length() == 0) {
        print_abort("Invalid parameters!");
    }
    return 0;
}
