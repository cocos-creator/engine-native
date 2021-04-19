
#include "PhysXShape.h"
#include "PhysXCylinder.h"
#include "../PhysXUtils.h"
#include "../PhysXWorld.h"
#include "math/Quaternion.h"
#include "renderer/pipeline/helper/SharedMemory.h"
#include <algorithm>

namespace cc {
namespace physics {

PhysXCylinder::PhysXCylinder() : _mMesh(nullptr){};

void PhysXCylinder::setConvex(uintptr_t handle) {
    if (reinterpret_cast<uintptr_t>(_mMesh) == handle) return;
    _mMesh = reinterpret_cast<physx::PxConvexMesh *>(handle);
    if (_mShape) {
        // TODO(Administrator): ...
    }
}

void PhysXCylinder::onComponentSet() {
    if (_mMesh) {
        physx::PxConvexMeshGeometry geom;
        geom.convexMesh = _mMesh;
        // geom.meshFlags = PxConvexMeshGeometryFlags::eTIGHT_BOUNDS;
        _mShape = PxGetPhysics().createShape(geom, getDefaultMaterial(), true);
        updateGeometry();
    }
}

void PhysXCylinder::setCylinder(float r, float h, EAxisDirection d) {
    _mData.radius    = r;
    _mData.height    = h;
    _mData.direction = d;
    updateGeometry();
}

void PhysXCylinder::updateGeometry() {
    if (!_mShape) return;
    static physx::PxMeshScale scale;
    pxSetVec3Ext(scale.scale, getSharedBody().getNode().worldScale);
    scale.scale.y *= std::max(0.0001F, _mData.height / 2);
    const auto xz = std::max(0.0001F, _mData.radius * 2);
    scale.scale.x *= xz;
    scale.scale.z *= xz;
    Quaternion quat;
    switch (_mData.direction) {
        case EAxisDirection::X_AXIS:
            quat.set(Vec3::UNIT_Z, physx::PxPiDivTwo);
            pxSetQuatExt(scale.rotation, quat);
            break;
        case EAxisDirection::Y_AXIS:
        default:
            scale.rotation = physx::PxQuat{physx::PxIdentity};
            break;
        case EAxisDirection::Z_AXIS:
            quat.set(Vec3::UNIT_X, physx::PxPiDivTwo);
            pxSetQuatExt(scale.rotation, quat);
            break;
    }
    physx::PxConvexMeshGeometry geom;
    if (getShape().getConvexMeshGeometry(geom)) {
        geom.scale = scale;
        getShape().setGeometry(geom);
    }
    pxSetQuatExt(_mRotation, quat);
}

void PhysXCylinder::updateScale() {
    updateGeometry();
    updateCenter();
}

} // namespace physics
} // namespace cc
