
#include "Core.hpp"

void Core::run()
{
    display->init();
    while (true) {
        display->update();
        display->render();
    }
    display->shutdown();
}
