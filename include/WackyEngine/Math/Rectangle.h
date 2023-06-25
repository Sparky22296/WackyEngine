#ifndef WACKYENGINE_MATH_RECTANGLE_H_
#define WACKYENGINE_MATH_RECTANGLE_H_

#include <iostream>

#include "WackyEngine/Math/Vector2.h"

namespace WackyEngine
{
    class Rectangle
    {
    public:
        int X;
        int Y;
        int Width;
        int Height;

        Rectangle();
        Rectangle(const Vector2& position, const int width, const int height);
        Rectangle(const int x, const int y, const int width, const int height);

        inline int GetTop() const noexcept { return Y; }
        inline int GetBottom() const noexcept { return Y + Height; }
        inline int GetLeft() const noexcept { return X; }
        inline int GetRight() const noexcept { return X + Width; }
        inline Vector2 GetPosition() const noexcept { return Vector2(X, Y); }
        inline Vector2 GetCentre() const noexcept { return Vector2(X + (Width / 2), Y + (Height / 2)); }
        inline Vector2 GetSize() const noexcept { return Vector2(Width, Height); }

        std::string ToString() const noexcept;

        bool Contains(const Vector2& vector) const noexcept;
        bool Contains(const Rectangle& rect) const noexcept;
        bool CompletelyContains(const Vector2& vector) const noexcept;
        bool CompletelyContains(const Rectangle& rect) const noexcept;
        bool Intersects(const Rectangle& rect) const noexcept;
        bool IntersectsOrTouches(const Rectangle& rect) const noexcept;
        bool Equals(const Rectangle& rect) const noexcept;

        void operator+=(const Rectangle& rect) noexcept;
    };
}

std::ostream& operator<<(std::ostream& cout, const WackyEngine::Rectangle& rect) noexcept;
WackyEngine::Rectangle operator+(WackyEngine::Rectangle& firstRect, WackyEngine::Rectangle& secondRect) noexcept;
bool operator==(const WackyEngine::Rectangle& firstRect, const WackyEngine::Rectangle& secondRect) noexcept;
bool operator!=(const WackyEngine::Rectangle& firstRect, const WackyEngine::Rectangle& secondRect) noexcept;

#endif