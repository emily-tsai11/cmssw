#ifndef TSCPBuilderWithPropagator_H
#define TSCPBuilderWithPropagator_H

#include "TrackingTools/PatternTools/interface/TrajectoryStateClosestToPointBuilder.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/DeepCopyPointerByClone.h"
#include "DataFormats/Math/interface/Point3D.h"

/**
 * This class builds a TrajectoryStateClosestToPoint given an original
 * FreeTrajectoryState. This new state is then defined at the point of closest
 * approach to a reference point. It is to be used when a specific propagator
 * has to be used.
 */

class TSCPBuilderWithPropagator : public TrajectoryStateClosestToPointBuilder {
public:
  // constructor with user-supplied propagator
  TSCPBuilderWithPropagator(const Propagator& u);

  ~TSCPBuilderWithPropagator() override{};

  TrajectoryStateClosestToPoint operator()(const FTS& originalFTS, const GlobalPoint& referencePoint) const override;
  TrajectoryStateClosestToPoint operator()(const TSOS& originalTSOS, const GlobalPoint& referencePoint) const override;

private:
  DeepCopyPointerByClone<Propagator> thePropagator;
};

#endif
