
#pragma once

#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <strings.h>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace BadApple {
    class IBadApple {
        public:
            IBadApple() = default;
            virtual ~IBadApple() = default;

            virtual void init() = 0;      // Initialize display
            virtual void update() = 0;    // Update displayed info
            virtual void render() const = 0;    // Render on screen
            virtual void shutdown() = 0;  // Clean up resources
        };
}
