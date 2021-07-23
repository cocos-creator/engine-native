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
#include <algorithm>

namespace cc {
namespace scene {

Sphere::Sphere() noexcept {
    _center = Vec3();
    _radius = -1.0F;
}

int Sphere::interset(const Plane &plane) const {
    const float dot = plane.n.dot(_center);
    const float r   = _radius * plane.n.length();
    if (dot + r < plane.d) {
        return -1;
    }

    if (dot - r > plane.d) {
        return 0;
    }

    return 1;
}

bool Sphere::interset(const Frustum &frustum) const {
    const auto &planes = frustum.planes;
    const auto *self   = this;
    return std::all_of(planes.begin(),
                       planes.end(),
                       // frustum plane normal points to the inside
                       [self](const Plane &plane) { return self->interset(plane) != -1; });
}

void Sphere::mergePoint(const Vec3 &point) {
    if (_radius < 0.0F) {
        _center = point;
        _radius = 0.0F;
        return;
    }

    auto offset   = point - _center;
    const float distance = offset.length();

    if (distance > _radius) {
        const float half = (distance - _radius) * 0.5F;
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

void Sphere::mergeFrustum(const Frustum &frustum) {
    const std::vector<Vec3> vertices{frustum.vertices.begin(), frustum.vertices.end()};

    mergePoints(vertices);
}

void Sphere::mergePoints(const std::vector<Vec3> &vertices) {
    for (const Vec3 &vertex : vertices) {
        mergePoint(vertex);
    }
}

int Sphere::spherePlane(const Plane &plane) const{
    const auto dot = cc::Vec3::dot(plane.n, _center);
    const auto r   = _radius * plane.n.length();
    if (dot + r < plane.d) {
        return -1;
    }
    if (dot - r > plane.d) {
        return 0;
    }
    return 1;
}

bool Sphere::sphereFrustum(const Frustum &frustum) const {
    const auto &planes = frustum.planes;
    const auto *self   = this;
    return std::all_of(planes.begin(),
                       planes.end(),
                       // frustum plane normal points to the inside
                       [self](const Plane &plane) { return self->interset(plane) != -1; });
}

} // namespace scene
} // namespace cc
