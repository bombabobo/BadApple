
#include "Ncurses.hpp"
#include "Core.hpp"

void StartNcurses(const std::string &videoPath = "./BadApple.mp4")
{
    //./BadApple.mp4
    //./SAO_op.mp4
    BadApple::IBadApple *display = new BadApple::NcursesBadApple(videoPath);
    Core monitor(display);
    BadApple::NcursesBadApple *ncursesDisplay = dynamic_cast<BadApple::NcursesBadApple*>(display);

    if (ncursesDisplay) {
        monitor.run();
        delete display;
    }
}

int main(int ac, char **av)
{
    std::cerr.rdbuf( NULL );
    if (ac == 1) {
        StartNcurses();
    } else if (ac == 2) {
        StartNcurses(av[1]);
    } else {
        std::cerr << "Error : Invalid argument" << std::endl;
        return (84);
    }
    return (0);
}
