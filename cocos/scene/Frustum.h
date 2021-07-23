/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.
 
 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#pragma once

#include <array>

#include "math/Vec3.h"

namespace cc {
namespace scene {
struct Camera;
enum class ShapeEnums {
    SHAPE_RAY              = (1 << 0),
    SHAPE_LINE             = (1 << 1),
    SHAPE_SPHERE           = (1 << 2),
    SHAPE_AABB             = (1 << 3),
    SHAPE_OBB              = (1 << 4),
    SHAPE_PLANE            = (1 << 5),
    SHAPE_TRIANGLE         = (1 << 6),
    SHAPE_FRUSTUM          = (1 << 7),
    SHAPE_FRUSTUM_ACCURATE = (1 << 8),
    SHAPE_CAPSULE          = (1 << 9),
};

/// Frustum planes.
enum FrustumPlane {
    PLANE_LEFT = 0,
    PLANE_RIGHT,
    PLANE_BOTTOM,
    PLANE_TOP,
    PLANE_NEAR,
    PLANE_FAR,
};

struct Plane final {
    float d{0.F};
    Vec3  n;

    void  define(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2);
    void  define(const Vec3 &normal, const Vec3 &point);
    float distance(const Vec3 &point) const;
};

struct Frustum final {
    std::array<Vec3, 8>  vertices;
    std::array<Plane, 6> planes;

    void                 define(const Vec3 &near, const Vec3 &far, const Mat4 &transform);
    void                 defineOrtho(float orthoSize, float aspectRatio, float nearZ, float farZ, const Mat4 &transform);
    void                 update(const Mat4 &m, const Mat4 &inv);
    void                 split(const Camera *camera, const Mat4 &m, float start, float end);
    void                 updatePlanes();
    void                 zero();
    ShapeEnums           type{ShapeEnums::SHAPE_FRUSTUM};
};

} // namespace scene
} // namespace cc
