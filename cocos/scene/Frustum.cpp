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

#include "scene/Frustum.h"
#include "scene/Define.h"

namespace cc {
namespace scene {
namespace {
const std::vector<cc::Vec3> VEC_VALS{
    {1, 1, 1},
    {-1, 1, 1},
    {-1, -1, 1},
    {1, -1, 1},
    {1, 1, -1},
    {-1, 1, -1},
    {-1, -1, -1},
    {1, -1, -1}};
} // namespace

// Define from 3 vertices.
void Plane::define(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2) {
    const Vec3 dist1 = v1 - v0;
    const Vec3 dist2 = v2 - v0;

    define(dist1.crossProduct(dist2), v0);
}

// Define from a normal vector and a point on the plane.
void Plane::define(const Vec3 &normal, const Vec3 &point) {
    n = normal.getNormalized();
    d = -normal.dot(point);
}

// Return signed distance to a point.
float Plane::distance(const Vec3 &point) const {
    return n.dot(point) + d;
}

void Frustum::define(const Vec3& near, const Vec3& far, const Mat4& transform) {
    vertices[0] = transform * near;
    vertices[1] = transform * Vec3(-near.x, near.y, near.z);
    vertices[2] = transform * Vec3(-near.x, -near.y, near.z);
    vertices[3] = transform * Vec3(near.x, -near.y, near.z);
    vertices[4] = transform * far;
    vertices[5] = transform * Vec3(-far.x, far.y, far.z);
    vertices[6] = transform * Vec3(-far.x, -far.y, far.z);
    vertices[7] = transform * Vec3(far.x, -far.y, far.z);

    updatePlanes();
}

void Frustum::defineOrtho(float orthoSize, float aspectRatio, float nearZ, float farZ, const Mat4& transform) {
    nearZ                = std::max(nearZ, 0.0f);
    farZ                 = std::max(farZ, nearZ);
    const float   halfViewSize = orthoSize * 0.5f;
    Vec3 near, far;

    near.z = nearZ;
    far.z  = farZ;
    far.y = near.y = halfViewSize;
    far.x = near.x = near.y * aspectRatio;

    define(near, far, transform);
}

void Frustum::update(const Mat4 &m, const Mat4 &inv) {
    // left plane
    planes[PLANE_LEFT].n.set(m.m[3] + m.m[0], m.m[7] + m.m[4], m.m[11] + m.m[8]);
    planes[PLANE_LEFT].d = -(m.m[15] + m.m[12]);
    // right plane
    planes[PLANE_RIGHT].n.set(m.m[3] - m.m[0], m.m[7] - m.m[4], m.m[11] - m.m[8]);
    planes[PLANE_RIGHT].d = -(m.m[15] - m.m[12]);
    // bottom plane
    planes[PLANE_BOTTOM].n.set(m.m[3] + m.m[1], m.m[7] + m.m[5], m.m[11] + m.m[9]);
    planes[PLANE_BOTTOM].d = -(m.m[15] + m.m[13]);
    // top plane
    planes[PLANE_TOP].n.set(m.m[3] - m.m[1], m.m[7] - m.m[5], m.m[11] - m.m[9]);
    planes[PLANE_TOP].d = -(m.m[15] - m.m[13]);
    // near plane
    planes[PLANE_NEAR].n.set(m.m[3] + m.m[2], m.m[7] + m.m[6], m.m[11] + m.m[10]);
    planes[PLANE_NEAR].d = -(m.m[15] + m.m[14]);
    // far plane
    planes[PLANE_FAR].n.set(m.m[3] - m.m[2], m.m[7] - m.m[6], m.m[11] - m.m[10]);
    planes[PLANE_FAR].d = -(m.m[15] - m.m[14]);

    if (type != ShapeEnums::SHAPE_FRUSTUM_ACCURATE) {
        return;
    }

    for (Plane &plane : planes) {
        const float invDist = 1 / plane.n.length();
        plane.n *= invDist;
        plane.d *= invDist;
    }
    uint32_t i = 0;
    for (const Vec3 &vec : VEC_VALS) {
        vertices[i].transformMat4(vec, inv);
        i++;
    }
}

void Frustum::split(const Mat4 &projection, float near, float far) {
    const Mat4 projInverse = projection.getInversed();

    // Figure out depth values for near & far
    const Vec4  nearTemp = projection * Vec4(0.0f, 0.0f, near, 1.0f);
    const Vec4  farTemp  = projection * Vec4(0.0f, 0.0f, far, 1.0f);
    const float nearZ    = nearTemp.z / nearTemp.w;
    const float farZ     = farTemp.z / farTemp.w;

    vertices[0] = projInverse * Vec3(1.0f, 1.0f, nearZ);
    vertices[1] = projInverse * Vec3(-1.0f, 1.0f, nearZ);
    vertices[2] = projInverse * Vec3(-1.0f, -1.0f, nearZ);
    vertices[3] = projInverse * Vec3(1.0f, -1.0f, nearZ);
    vertices[4] = projInverse * Vec3(1.0f, 1.0f, farZ);
    vertices[5] = projInverse * Vec3(-1.0f, 1.0f, farZ);
    vertices[6] = projInverse * Vec3(-1.0f, -1.0f, farZ);
    vertices[7] = projInverse * Vec3(1.0f, -1.0f, farZ);

    updatePlanes();
}

void Frustum::zero() {
    vertices[0] = Mat4::ZERO * vertices[0];
    vertices[1] = Mat4::ZERO * vertices[1];
    vertices[2] = Mat4::ZERO * vertices[2];
    vertices[3] = Mat4::ZERO * vertices[3];
    vertices[4] = Mat4::ZERO * vertices[4];
    vertices[5] = Mat4::ZERO * vertices[5];
    vertices[6] = Mat4::ZERO * vertices[6];
    vertices[7] = Mat4::ZERO * vertices[7];

    updatePlanes();
}

void Frustum::updatePlanes() {
    // left plane
    planes[PLANE_LEFT].define(vertices[1], vertices[5], vertices[6]);
    // right plane
    planes[PLANE_RIGHT].define(vertices[3], vertices[7], vertices[4]);
    // bottom plane
    planes[PLANE_BOTTOM].define(vertices[6], vertices[7], vertices[3]);
    // top plane
    planes[PLANE_TOP].define(vertices[0], vertices[4], vertices[5]);
    // near plane
    planes[PLANE_NEAR].define(vertices[2], vertices[3], vertices[0]);
    // far plane
    planes[PLANE_FAR].define(vertices[7], vertices[6], vertices[5]);

    if (type != ShapeEnums::SHAPE_FRUSTUM_ACCURATE) {
        return;
    }

    // Check if we ended up with inverted planes (reflected transform) and flip in that case
    planes[PLANE_NEAR].define(planes[PLANE_FAR].n.getNegate(), vertices[2]);

    if (planes[PLANE_NEAR].distance(vertices[5]) < 0.0f) {
        for (auto& plane : planes) {
            plane.n = -plane.n;
            plane.d = -plane.d;
        }
    }
}

} // namespace scene
} // namespace cc