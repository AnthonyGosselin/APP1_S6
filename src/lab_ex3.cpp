#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>  // for strtol

int main(int argc, char** argv)
{
    std::ifstream file_in;

    int fact = 2;

    if (argc >= 2 && (strcmp(argv[1], "-") != 0)) {
        file_in.open(argv[1]);
        if (argv[2] != NULL) {
            printf("Got argv[2]: %s", argv[2]);
            //fact = strtol(argv[1], NULL, 10);
        }
        if (file_in.is_open()) {
            std::cin.rdbuf(file_in.rdbuf());
            std::cerr << "Using " << argv[1] << "..." << std::endl;
        } else {
            std::cerr   << "Error: Cannot open '"
                        << argv[1] 
                        << "', using stdin (press CTRL-D for EOF)." 
                        << std::endl;
        }
    } else {
        std::cerr << "Using stdin (press CTRL-D for EOF)." << std::endl;
    }

    while (!std::cin.eof()) {

        std::string line;

        std::getline(std::cin, line);
        if (!line.empty()) {
            int val = atoi(line.c_str());
            std::cout << val * fact << std::endl;
        }
    }

    if (file_in.is_open()) {
        file_in.close();
    }

    return 0;
}
