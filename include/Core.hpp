
#pragma once

#include <vector>
#include <unistd.h>
#include "IBadApple.hpp"

class Core {
    public:
        Core(BadApple::IBadApple *disp):display(disp){};
        ~Core() = default;

        void run();
    private:
        BadApple::IBadApple *display;
};
