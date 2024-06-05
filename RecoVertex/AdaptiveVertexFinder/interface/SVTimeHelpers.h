#ifndef RecoVertex_AdaptiveVertexFinder_SVTimeHelpers_h
#define RecoVertex_AdaptiveVertexFinder_SVTimeHelpers_h


#include "FWCore/Utilities/interface/isFinite.h"

#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"


namespace svhelper {
  inline void updateVertexTime(TransientVertex& vtx) {
    const auto& trks = vtx.originalTracks();
    double meantime = 0., expv_x2 = 0., normw = 0., timecov = 0.;
    for (const auto& trk : trks) {
      if (edm::isFinite(trk.timeExt())) {
        const double time = trk.timeExt();
        const double inverr = 1.0 / trk.dtErrorExt();
        const double w = inverr * inverr;
        meantime += time * w;
        expv_x2 += time * time * w;
        normw += w;
      }
    }
    if (normw > 0.) {
      meantime = meantime / normw;
      expv_x2 = expv_x2 / normw;
      timecov = expv_x2 - meantime * meantime;
      auto err = vtx.positionError().matrix4D();
      err(3, 3) = timecov / (double)trks.size();
      vtx = TransientVertex(vtx.position(), meantime, err, vtx.originalTracks(), vtx.totalChiSquared());
    }
  }
} // namespace svhelper


#endif
