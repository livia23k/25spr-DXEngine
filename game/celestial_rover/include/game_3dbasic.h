#pragma once

#include "core/game.h"

class Game3DBasic : public Game
{
public:
    Game3DBasic(uint32_t width, uint32_t height, const std::string &title);
    ~Game3DBasic() = default;

    void onCreate() override;

protected:
    // void onGraphicsUpdate(float deltaTime) override;
    void onLogicUpdate(float deltaTime) override;
    void onInputUpdate(float deltaTime) override;
};
