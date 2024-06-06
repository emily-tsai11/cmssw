#ifndef RecoVertex_AdaptiveVertexFinder_TracksClusteringFromDisplacedSeed_h
#define RecoVertex_AdaptiveVertexFinder_TracksClusteringFromDisplacedSeed_h


#include <memory>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/isFinite.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/IPTools/interface/IPTools.h"


// #define VTXDEBUG


class TracksClusteringFromDisplacedSeed {
public:
  struct Cluster {
    GlobalPoint seedPoint;
    reco::TransientTrack seedingTrack;
    std::vector<reco::TransientTrack> tracks;
  };

  TracksClusteringFromDisplacedSeed(const edm::ParameterSet &params);
  std::vector<Cluster> clusters(const edm::Event &event, const reco::Vertex &pv,
                                const std::vector<reco::TransientTrack> &selectedTracks);

  void setTrackTimeValueMapToken(const edm::EDGetTokenT<edm::ValueMap<float>> tokenTrackTimeValueMap) {
    token_trackTimeValueMap = tokenTrackTimeValueMap;
  }

  void setTrackTimeErrorMapToken(const edm::EDGetTokenT<edm::ValueMap<float>> tokenTrackTimeErrorMap) {
    token_trackTimeErrorMap = tokenTrackTimeErrorMap;
  }

  // void setTrackTimeQualityMapToken(const edm::EDGetTokenT<edm::ValueMap<float>> tokenTrackTimeQualityMap) {
  //   token_trackTimeQualityMap = tokenTrackTimeQualityMap;
  // }

private:
  bool trackFilter(const reco::TrackRef &track) const;
  std::pair<std::vector<reco::TransientTrack>, GlobalPoint> nearTracks(const reco::TransientTrack &seed,
                const std::vector<reco::TransientTrack> &tracks, const reco::Vertex &primaryVertex) const;

  edm::EDGetTokenT<edm::ValueMap<float>> token_trackTimeValueMap;
  edm::EDGetTokenT<edm::ValueMap<float>> token_trackTimeErrorMap;
  // edm::EDGetTokenT<edm::ValueMap<float>> token_trackTimeQualityMap;

  edm::ValueMap<float> trackTimeValueMap;
  edm::ValueMap<float> trackTimeErrorMap;
  // edm::ValueMap<float> trackTimeQualityMap;

  // unsigned int maxNTracks;
  bool useMTD;
  // double trackTimeQualityThreshold;
  double max3DIPSignificance;
  double max3DIPValue;
  double min3DIPSignificance;
  double min3DIPValue;
  bool cutTimeRange;
  double maxTimeRange;
  double clusterMaxDistance;
  double clusterMaxSignificance;
  double distanceRatio;
  double clusterMinAngleCosine;
  double maxTimeSignificance;
};


#endif
