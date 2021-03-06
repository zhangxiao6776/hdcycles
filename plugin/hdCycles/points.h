//  Copyright 2020 Tangent Animation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
//  including without limitation, as related to merchantability and fitness
//  for a particular purpose.
//
//  In no event shall any copyright holder be liable for any damages of any kind
//  arising from the use of this software, whether in contract, tort or otherwise.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef HD_CYCLES_POINTS_H
#define HD_CYCLES_POINTS_H

#include "api.h"

#include "hdcycles.h"
#include "renderDelegate.h"

#include <util/util_transform.h>

#include <pxr/imaging/hd/points.h>
#include <pxr/pxr.h>

namespace ccl {
class Object;
class Mesh;
class Scene;
}  // namespace ccl

PXR_NAMESPACE_OPEN_SCOPE

class HdSceneDelegate;
class HdCyclesRenderDelegate;

enum HdCyclesPointStyle {
    POINT_DISCS,
    POINT_SPHERES,
};

/**
 * @brief An intermediate solution for HdPoints as Cycles doesn't
 * natively support point clouds.
 * 
 */
class HdCyclesPoints final : public HdPoints {
public:
    /**
     * @brief Construct a new HdCycles Point object
     * 
     * @param id Path to the Point Primitive
     * @param instancerId If specified the HdInstancer at this id uses this curve
     * as a prototype
     */
    HdCyclesPoints(SdfPath const& id, SdfPath const& instancerId,
                   HdCyclesRenderDelegate* a_renderDelegate);
    /**
     * @brief Destroy the HdCycles Points object
     * 
     */
    virtual ~HdCyclesPoints();

    /**
     * @brief Pull invalidated material data and prepare/update the core Cycles 
     * representation.
     * 
     * This must be thread safe.
     * 
     * @param sceneDelegate The data source for the Point
     * @param renderParam State
     * @param dirtyBits Which bits of scene data has changed
     */
    void Sync(HdSceneDelegate* sceneDelegate, HdRenderParam* renderParam,
              HdDirtyBits* dirtyBits, TfToken const& reprSelector) override;

    /**
     * @brief Inform the scene graph which state needs to be downloaded in
     * the first Sync() call
     * 
     * @return The initial dirty state this Point wants to query
     */
    HdDirtyBits GetInitialDirtyBitsMask() const override;

    /**
     * @return Return true if this light is valid.
     */
    bool IsValid() const;

    /**
     * @brief Not Implemented
     */
    void Finalize(HdRenderParam* renderParam) override;

protected:
    /**
     * @brief Initialize the given representation of this Rprim.
     * This is called prior to syncing the prim.
     * 
     * @param reprToken The name of the repr to initialize
     * @param dirtyBits In/Out dirty values
     */
    void _InitRepr(TfToken const& reprToken, HdDirtyBits* dirtyBits) override;

    /**
     * @brief Set additional dirty bits
     * 
     * @param bits 
     * @return New value of dirty bits
     */
    HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;

private:
    /**
     * @brief Create the cycles points as discs mesh and object representation
     * 
     * @param resolution Resolution of the disc geometry
     * @return New allocated pointer to ccl::Mesh
     */
    ccl::Mesh* _CreateDiscMesh();

    /**
     * @brief Create the cycles points as spheres mesh and object representation
     * 
     * @param scene Cycles scene to add mesh to
     * @param transform Initial transform for object
     * @return New allocated pointer to ccl::Mesh
     */
    ccl::Mesh* _CreateSphereMesh();

    /**
     * @brief Create the cycles object for an individual point
     * 
     * @param transform Transform of the point
     * @param mesh Mesh to populate the point with
     * @return ccl::Object* 
     */
    ccl::Object* _CreatePointsObject(const ccl::Transform& transform,
                                     ccl::Mesh* mesh);

    ccl::Mesh* m_cyclesMesh;

    std::vector<ccl::Object*> m_cyclesObjects;

    HdCyclesRenderDelegate* m_renderDelegate;

    ccl::Transform m_transform;

    int m_pointStyle;
    int m_pointResolution;

    // -- Currently unused

    bool m_useMotionBlur;
    int m_motionSteps;

    HdTimeSampleArray<GfMatrix4d, HD_CYCLES_MOTION_STEPS> m_transformSamples;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif  // HD_CYCLES_POINTS_H
