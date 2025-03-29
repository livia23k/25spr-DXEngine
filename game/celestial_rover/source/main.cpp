#include "utils/forward.h"
#include "game_3dbasic.h"

int main(void)
{
    Logger::LoggerInstance::GetInstance().SetVerbosity(Logger::LogLevel::INFO);
    Logger::LoggerInstance::GetInstance().SetLogFile("log/game_log.txt");

    try
    {
        Game3DBasic game(800, 600, "Celestial Rover");
        game.onCreate();
        game.run();
    }
    catch (const std::exception &e)
    {
        Logger::LogError(std::format("Exception: {}", e.what()));
        return 1;
    }

    return 0;
}
