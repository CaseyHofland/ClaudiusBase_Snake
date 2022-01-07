#pragma once

#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include "SDL.h"
#pragma warning(pop)

#include <vector>

class Input
{
    std::vector<Uint8> currentFrame{0};
    std::vector<Uint8> lastFrame{0};

public:
    void ProcessInput();

    bool GetKey(SDL_Scancode) const noexcept;
    bool GetKeyDown(SDL_Scancode) const noexcept;
    bool GetKeyUp(SDL_Scancode) const noexcept;
};

