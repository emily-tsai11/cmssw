#ifndef RecoVertex_AdaptiveVertexFinder_TracksClusteringFromDisplacedSeed_h
#define RecoVertex_AdaptiveVertexFinder_TracksClusteringFromDisplacedSeed_h


#include <memory>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
// #include "FWCore/Framework/interface/stream/EDProducer.h"
// #include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/isFinite.h"
// #include "FWCore/ServiceRegistry/interface/Service.h"
// #include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"

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

  void setTimeValueMapToken(const edm::EDGetTokenT<edm::ValueMap<float>> tokenTimeValueMap) {
    token_timeValueMap = tokenTimeValueMap;
  }

  void setTimeErrorMapToken(const edm::EDGetTokenT<edm::ValueMap<float>> tokenTimeErrorMap) {
    token_timeErrorMap = tokenTimeErrorMap;
  }

  void setTimeQualityMapToken(const edm::EDGetTokenT<edm::ValueMap<float>> tokenTimeQualityMap) {
    token_timeQualityMap = tokenTimeQualityMap;
  }

private:
  bool trackFilter(const reco::TrackRef &track) const;
  std::pair<std::vector<reco::TransientTrack>, GlobalPoint> nearTracks(const reco::TransientTrack &seed,
                                const std::vector<reco::TransientTrack> &tracks, const reco::Vertex &primaryVertex) const;

  edm::EDGetTokenT<edm::ValueMap<float>> token_timeValueMap;
  edm::EDGetTokenT<edm::ValueMap<float>> token_timeErrorMap;
  edm::EDGetTokenT<edm::ValueMap<float>> token_timeQualityMap;

  edm::ValueMap<float> timeValueMap;
  edm::ValueMap<float> timeErrorMap;
  edm::ValueMap<float> timeQualityMap;

  // unsigned int maxNTracks;
  bool useMTDTiming;
  std::string timeReference;
  double timeQualityThreshold;
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
