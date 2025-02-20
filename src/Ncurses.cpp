
#include "Ncurses.hpp"
#include <sstream>
#include <iomanip>

namespace BadApple
{
    NcursesBadApple::NcursesBadApple(const std::string& videoPath) :
    cap(videoPath), videoFilename(videoPath), frameCount(0) {
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open the video file!" << std::endl;
            exit(1);
        }
        fps = cap.get(cv::CAP_PROP_FPS);
    }

    void NcursesBadApple::init() {
        mousemask(0, nullptr);
        set_escdelay(0);
        setlocale(LC_ALL, "");
        initscr();

        start_color();
        init_pair(0, COLOR_BLACK, COLOR_BLACK);
        init_pair(1, COLOR_RED, COLOR_YELLOW);  // Orange (approximation)
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_GREEN, COLOR_GREEN); // Dark Green
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLUE); // Blue Violet
        init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(8, COLOR_RED, COLOR_BLACK);
        init_pair(9, COLOR_WHITE, COLOR_BLACK);

        raw();
        keypad(stdscr, TRUE);
        noecho();
        nodelay(stdscr, TRUE);
        curs_set(0);
        timeout(0);
        playSound(extractAudio(videoFilename));
        startTime = std::chrono::steady_clock::now();
    }

    void NcursesBadApple::update() {
        int rows, cols, i = 0;

        clear();
        getmaxyx(stdscr, rows, cols);

        auto currentTime = std::chrono::steady_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - startTime).count();

        double expectedFrames = elapsedTime * fps;
        while (frameCount < expectedFrames && cap.read(currentFrame))
            frameCount++;

        std::vector<std::vector<std::pair<char, int>>> frame = getFrame(cols, rows);
        for (const auto& row : frame) {
            move(i++, 0);
            for (const auto& [pixel, color] : row) {
                attron(COLOR_PAIR(color));
                printw("%c", pixel);
                attroff(COLOR_PAIR(color));
            }
        }
        events();
        usleep(1000000 / fps);  // Delay for frame rate
    }

    void NcursesBadApple::render() const
    {
        refresh();
    }

    void NcursesBadApple::shutdown() {
        Mix_CloseAudio(); // Properly close SDL_mixer
        Mix_FreeMusic(music);
        Mix_Quit();
        SDL_Quit();
        cap.release();  // Release OpenCV VideoCapture
        system("rm -rf audio.mp3 &>/dev/null");
        endwin();
    }

    void NcursesBadApple::events()
    {
        int ch = getch();
        if (ch == ERR)
            return;

        static const std::unordered_map<int, std::function<void()>> keyActions = {
            {'q', [this]() { shutdown(); exit(0); }},
            {'Q', [this]() { shutdown(); exit(0); }},
        };

        if (keyActions.find(ch) != keyActions.end())
            keyActions.at(ch)();
        flushinp();
    }

    std::vector<std::vector<std::pair<char, int>>> NcursesBadApple::getFrame(int width, int height) {
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::cout << "End of video reached!" << std::endl;
            shutdown();
            exit(0);
        }
        //frame = resizeFrame(frame, 1920, 1080);
        cv::resize(frame, frame, cv::Size(width, height));
        std::vector<std::vector<std::pair<char, int>>> pixelArray(height, std::vector<std::pair<char, int>>(width));
        const std::string grayscale = " .:-=+*#%@"; // ASCII shading

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                cv::Vec3b color = frame.at<cv::Vec3b>(y, x); // BGR format
                int b = color[0];
                int g = color[1];
                int r = color[2];
                int colorCode = getColorCode(r, g, b);
                int brightness = (r + g + b) / 3;
                char pixelChar = grayscale[brightness * grayscale.size() / 256];

                pixelArray[y][x] = {pixelChar, colorCode};
            }
        }
        frameCount++;
        return pixelArray;
    }

    cv::Mat NcursesBadApple::resizeFrame(cv::Mat &frame, int targetWidth, int targetHeight) {
        int originalWidth = frame.cols;
        int originalHeight = frame.rows;

        double scaleX = (double)targetWidth / originalWidth;
        double scaleY = (double)targetHeight / originalHeight;
        double scale = std::min(scaleX, scaleY);

        int newWidth = originalWidth * scale;
        int newHeight = originalHeight * scale;

        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(newWidth, newHeight));

        cv::Mat outputFrame(targetHeight, targetWidth, frame.type(), cv::Scalar(0, 0, 0));

        int xOffset = (targetWidth - newWidth) / 2;
        int yOffset = (targetHeight - newHeight) / 2;

        resizedFrame.copyTo(outputFrame(cv::Rect(xOffset, yOffset, newWidth, newHeight)));
        return outputFrame;
    }

    int NcursesBadApple::getColorCode(int r, int g, int b) {
        if (r < 50 && g < 50 && b < 50) return 0; // Black
        if (r > 150 && g < 100 && b < 50) return 1; // Orange
        if (r > 200 && g > 150 && b < 100) return 2; // Yellow
        if (r < 100 && g > 180 && b < 100) return 3; // Light Green
        if (r < 50 && g > 100 && b < 50) return 4; // Dark Green
        if (b > 150 && r < 100 && g < 100) return 5; // Blue
        if (b > 150 && r > 100 && g < 100) return 6; // Blue Violet
        if (r > 100 && g < 50 && b > 100) return 7; // Purple
        if (r > 150 && g < 100 && b < 100) return 8; // Red
        return 9; // White (default)
    }

    std::string NcursesBadApple::extractAudio(const std::string& videoPath) {
        std::string audioOutput = "audio.mp3";
        std::string command = "ffmpeg -i " + videoPath + " -q:a 0 -map a " + audioOutput + " -y &>/dev/null";

        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << "Error extracting audio!" << std::endl;
            return "";
        }
        return audioOutput;
    }

    void NcursesBadApple::playSound(const std::string& audioPath) {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
            return;
        }

        music = Mix_LoadMUS(audioPath.c_str());
        if (!music) {
            std::cerr << "Failed to load music! Mix_Error: " << Mix_GetError() << std::endl;
            return;
        }

        if (Mix_PlayMusic(music, 1) == -1) {
            std::cerr << "Mix_PlayMusic failed! Mix_Error: " << Mix_GetError() << std::endl;
        }
    }
}