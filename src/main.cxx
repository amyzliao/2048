#include <iostream>
#include "controller.hxx"

int
main(int argc, char *argv[])
{
    std::string command;
    int run_mode; // 0 = normal, 1 = lose, 2 = win

    switch(argc) {
        case 1:
            run_mode = 0;
            break;
        case 2:
            command = argv[1];
            if (command == "win") {
                run_mode = 2;
            } else if (command == "lose") {
                run_mode = 1;
            } else {
                std::cerr << "Usage: " << argv[0] << " [win/lose]\n";
                return 1;
            }
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " [win/lose]\n";
            return 1;
    }

    Controller(run_mode).run();

    return 0;
}
