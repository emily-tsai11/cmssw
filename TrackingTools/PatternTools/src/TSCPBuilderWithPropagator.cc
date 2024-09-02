#include "TrackingTools/PatternTools/interface/TSCPBuilderWithPropagator.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackingTools/PatternTools/interface/TrajectoryExtrapolatorToLine.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;

TSCPBuilderWithPropagator::TSCPBuilderWithPropagator(const Propagator& u) : thePropagator(u.clone()) {
  thePropagator->setPropagationDirection(anyDirection);
}

TrajectoryStateClosestToPoint TSCPBuilderWithPropagator::operator()(const FreeTrajectoryState& originalFTS,
                                                                    const GlobalPoint& referencePoint) const {
  GlobalPoint pointpos(referencePoint.x(), referencePoint.y(), referencePoint.z());
  GlobalVector pointvec(referencePoint.x() / referencePoint.z(),
                        referencePoint.y() / referencePoint.z(),
                        1.0);
  Line pointline(pointpos, pointvec);

  TrajectoryExtrapolatorToLine tetl;

  TrajectoryStateOnSurface tsosfinal = tetl.extrapolate(originalFTS, pointline, *thePropagator);

  if (!tsosfinal.isValid())
    return TrajectoryStateClosestToPoint();

  // Compute point on beamline of closest approach
  GlobalPoint tp = tsosfinal.globalPosition(); // position of trajectory closest approach
  GlobalVector hyp(
      tp.x() - pointpos.x(), tp.y() - pointpos.y(), tp.z() - pointpos.z()); // difference between traj and reference point
  double l = pointline.direction().dot(hyp);                                // length along beamline away from reference point
  GlobalPoint closepoint = pointpos + l * pointvec;

  // Get the free state and return the TSCP
  const FreeTrajectoryState theFTS = *tsosfinal.freeState();
  return TrajectoryStateClosestToPoint(theFTS, closepoint);
}

TrajectoryStateClosestToPoint TSCPBuilderWithPropagator::operator()(const TrajectoryStateOnSurface& originalTSOS,
                                                                    const GlobalPoint& referencePoint) const {
  // TODO: this method is not correctly implemented
  return TrajectoryStateClosestToPoint(*(originalTSOS.freeState()), referencePoint);
}
