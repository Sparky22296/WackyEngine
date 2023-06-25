#ifndef WACKYENGINE_GRAPHICS_UNIFORMBUFFEROBJECT_H_
#define WACKYENGINE_GRAPHICS_UNIFORMBUFFEROBJECT_H_

#include "WackyEngine/Math/Matrix4.h"

namespace WackyEngine
{
    struct UniformBufferObject
    {
        Matrix4 ModelMatrix;
        Matrix4 ViewMatrix;
        Matrix4 ProjectionMatrix;
    };
}

#endif