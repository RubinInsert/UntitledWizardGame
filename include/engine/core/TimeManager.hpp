#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H
#include <cstdint>
class TimeManager {
    public:
        void update();
        double getDeltaTime() const;
        double getElapsedTime() const;

    private:
        double deltaTime;
        double elapsedTime;
        uint64_t lastTime{0};
};

#endif