#ifndef WACKYENGINE_CORE_TIMESTEP_H_
#define WACKYENGINE_CORE_TIMESTEP_H_

namespace WackyEngine
{
    class Timestep
    {
    private:
        float m_Time;

    public:
        inline Timestep(float time = 0.0f) : m_Time(time) { }

        inline float GetSeconds() { return m_Time * 1000.0f; }
        inline float GetMilliseconds() { return m_Time; }
    };
}

#endif