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

#include "scene/Sphere.h"

namespace cc {
namespace scene {

int Sphere::interset(const Plane &plane) const {
    const float dot = plane.normal.dot(_center);
    const float r   = _radius * plane.normal.length();
    if (dot + r < plane.distance) {
        return -1;
    }

    if (dot - r > plane.distance) {
        return 0;
    }

    return 1;
}

bool Sphere::interset(const Frustum &frustum) const {
    for (const auto &plane : frustum.planes) { // NOLINT
        if (this->interset(plane) == -1) {
            return false;
        }
    }

    return true;
}

void Sphere::mergePoint(const Vec3 &point) {
    if (_radius < 0.0F) {
        _center = point;
        _radius = 0.0F;
        return;
    }

    auto offset   = point - _center;
    auto distance = offset.length();

    if (distance > _radius) {
        auto half = (distance - _radius) * 0.5F;
        _radius += half;
        offset.scale(half / distance);
        _center += offset;
    }
}

void Sphere::define(const AABB &aabb) {
    cc::Vec3 minPos;
    cc::Vec3 maxPos;
    aabb.getBoundary(&minPos, &maxPos);

    // Initialize sphere
    _center.set(minPos);
    _radius = 0.0F;

    // Calculate sphere
    const cc::Vec3 offset = maxPos - _center;
    const float    dist   = offset.length();

    const float half = dist * 0.5F;
    _radius += dist * 0.5F;
    _center += (half / dist) * offset;
}

void Sphere::mergeAABB(const AABB *aabb) {
    cc::Vec3 minPos;
    cc::Vec3 maxPos;
    aabb->getBoundary(&minPos, &maxPos);
    mergePoint(minPos);
    mergePoint(maxPos);
}

int Sphere::spherePlane(const Plane &plane) {
    const auto dot = cc::Vec3::dot(plane.normal, _center);
    const auto r   = _radius * plane.normal.length();
    if (dot + r < plane.distance) {
        return -1;
    }
    if (dot - r > plane.distance) {
        return 0;
    }
    return 1;
}

bool Sphere::sphereFrustum(const Frustum &frustum) {
    for (const auto &plane : frustum.planes) {
        // frustum plane normal points to the inside
        if (spherePlane(plane) == -1) {
            return false;
        }
    } // completely outside
    return true;
}

} // namespace scene
} // namespace cc
