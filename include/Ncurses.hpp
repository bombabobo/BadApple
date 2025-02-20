
#pragma once

#include "IBadApple.hpp"
#include <ncurses.h>
#include <unordered_map>

namespace BadApple {
    class NcursesBadApple : public IBadApple {
        private:
            cv::VideoCapture cap;
            std::string videoFilename;
            double fps;
            int frameCount;
            std::chrono::_V2::steady_clock::time_point startTime;
            cv::Mat currentFrame;
            Mix_Music* music;

        public:
            NcursesBadApple(const std::string& videoPath = "./BadApple.mp4");
            ~NcursesBadApple() = default;

            void init() override;
            void update() override;
            void render() const override;
            void shutdown() override;

            double getFPS() const { return fps; }
            std::vector<std::vector<std::pair<char, int>>> getFrame(int width, int height);
            void events();
            void playSound(const std::string& audioPath);
            std::string extractAudio(const std::string& videoPath);
            cv::Mat resizeFrame(cv::Mat &frame, int targetWidth, int targetHeight);
            int getColorCode(int r, int g, int b);
    };
}
