#ifndef RecoVertex_AdaptiveVertexFinder_InclusiveVertexFinder_h
#define RecoVertex_AdaptiveVertexFinder_InclusiveVertexFinder_h


#include <memory>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/ESGetToken.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "RecoVertex/ConfigurableVertexReco/interface/ConfigurableVertexReconstructor.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "RecoVertex/AdaptiveVertexFinder/interface/TracksClusteringFromDisplacedSeed.h"

#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexUpdator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexTrackCompatibilityEstimator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexSmoother.h"
#include "RecoVertex/MultiVertexFit/interface/MultiVertexFitter.h"

#include "RecoVertex/AdaptiveVertexFinder/interface/TTHelpers.h"
#include "RecoVertex/AdaptiveVertexFinder/interface/SVTimeHelpers.h"
#include "FWCore/Utilities/interface/isFinite.h"

#include <type_traits>


// #define VTXDEBUG 1


template <class InputContainer, class VTX>
class TemplatedInclusiveVertexFinder : public edm::stream::EDProducer<> {
public:
  typedef std::vector<VTX> Product;
  typedef typename InputContainer::value_type TRK;
  TemplatedInclusiveVertexFinder(const edm::ParameterSet &params);

  static void fillDescriptions(edm::ConfigurationDescriptions &cdesc) {
    edm::ParameterSetDescription pdesc;
    pdesc.add<edm::InputTag>("beamSpot", edm::InputTag("offlineBeamSpot"));
    pdesc.add<edm::InputTag>("primaryVertices", edm::InputTag("offlinePrimaryVertices"));
    if (std::is_same<VTX, reco::Vertex>::value) {
      pdesc.add<edm::InputTag>("tracks", edm::InputTag("generalTracks"));
      pdesc.add<unsigned int>("minHits", 8);
    } else if (std::is_same<VTX, reco::VertexCompositePtrCandidate>::value) {
      pdesc.add<edm::InputTag>("tracks", edm::InputTag("particleFlow"));
      pdesc.add<unsigned int>("minHits", 0);
    } else {
      pdesc.add<edm::InputTag>("tracks", edm::InputTag("generalTracks"));
    }
    // pdesc.add<bool>("useMTDTiming", false);
    pdesc.add<std::string>("timeReference", "beamSpot"); // or "primaryVertex"
    pdesc.add<edm::InputTag>("timeValueMap", edm::InputTag("tofPID:t0"));
    pdesc.add<edm::InputTag>("timeErrorMap", edm::InputTag("tofPID:sigmat0"));
    pdesc.add<edm::InputTag>("timeQualityMap", edm::InputTag("mtdTrackQualityMVA:mtdQualMVA"));
    // pdesc.add<double>("timeQualityThreshold", 0.5);
    // pdesc.add<bool>("cutTimeRange", false);
    // pdesc.add<double>("maxTimeRange", 0.4);

    pdesc.add<double>("maximumLongitudinalImpactParameter", 0.3);
    pdesc.add<double>("maximumTimeSignificance", 3.0);
    pdesc.add<double>("minPt", 0.8);
    pdesc.add<unsigned int>("maxNTracks", 30);
    // Clusterizer ParameterSet
    edm::ParameterSetDescription clusterizer;
    clusterizer.add<bool>("useMTDTiming", false);
    // clusterizer.add<std::string>("timeReference", "beamSpot"); // or "primaryVertex"
    // clusterizer.add<edm::InputTag>("timeValueMap", edm::InputTag("tofPID:t0"));
    // clusterizer.add<edm::InputTag>("timeErrorMap", edm::InputTag("tofPID:sigmat0"));
    // clusterizer.add<edm::InputTag>("timeQualityMap", edm::InputTag("mtdTrackQualityMVA:mtdQualMVA"));
    clusterizer.add<double>("timeQualityThreshold", 0.5);
    clusterizer.add<double>("seedMax3DIPSignificance", 9999.0);
    clusterizer.add<double>("seedMax3DIPValue", 9999.0);
    clusterizer.add<double>("seedMin3DIPSignificance", 1.2);
    clusterizer.add<double>("seedMin3DIPValue", 0.005);
    clusterizer.add<bool>("cutTimeRange", false);
    clusterizer.add<double>("maxTimeRange", 0.4);
    clusterizer.add<double>("clusterMaxDistance", 0.05);
    clusterizer.add<double>("clusterMaxSignificance", 4.5);
    clusterizer.add<double>("distanceRatio", 20.0);
    clusterizer.add<double>("clusterMinAngleCosine", 0.5);
    clusterizer.add<double>("maxTimeSignificance", 3.5);
    pdesc.add<edm::ParameterSetDescription>("clusterizer", clusterizer);
    // Vertex and fitter config
    pdesc.add<double>("vertexMinAngleCosine", 0.95);
    pdesc.add<double>("vertexMinDLen2DSig", 2.5);
    pdesc.add<double>("vertexMinDLenSig", 0.5);
    pdesc.add<double>("fitterSigmacut", 3.0);
    pdesc.add<double>("fitterTini", 256.0);
    pdesc.add<double>("fitterRatio", 0.25);
    pdesc.add<bool>("useDirectVertexFitter", true);
    pdesc.add<bool>("useVertexReco", true);
    // VertexReco ParameterSet
    edm::ParameterSetDescription vertexReco;
    vertexReco.add<std::string>("finder", std::string("avr"));
    vertexReco.add<double>("primcut", 1.0);
    vertexReco.add<double>("seccut", 3.0);
    vertexReco.add<bool>("smoothing", true);
    pdesc.add<edm::ParameterSetDescription>("vertexReco", vertexReco);
    if (std::is_same<VTX, reco::Vertex>::value) {
      cdesc.add("inclusiveVertexFinderDefault", pdesc);
    } else if (std::is_same<VTX, reco::VertexCompositePtrCandidate>::value) {
      cdesc.add("inclusiveCandidateVertexFinderDefault", pdesc);
    } else {
      cdesc.addDefault(pdesc);
    }
  }

  void produce(edm::Event &event, const edm::EventSetup &es) override;

private:
  bool trackFilter(const reco::Track &track) const;
  std::pair<std::vector<reco::TransientTrack>, GlobalPoint> nearTracks(const reco::TransientTrack &seed,
                                                                       const std::vector<reco::TransientTrack> &tracks,
                                                                       const reco::Vertex &primaryVertex) const;

  edm::EDGetTokenT<reco::BeamSpot> token_beamSpot;
  edm::EDGetTokenT<reco::VertexCollection> token_primaryVertex;
  edm::EDGetTokenT<InputContainer> token_tracks;
  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> token_trackBuilder;

  // edm::EDGetTokenT<edm::ValueMap<float>> token_timeValueMap;
  // edm::EDGetTokenT<edm::ValueMap<float>> token_timeErrorMap;
  // edm::EDGetTokenT<edm::ValueMap<float>> token_timeQualityMap;

  unsigned int minHits;
  unsigned int maxNTracks;
  double maxLIP;
  double maxTimeSig;
  // bool useMTDTiming;
  std::string timeReference;
  // double timeQualityThreshold;
  // bool cutTimeRange;
  // double maxTimeRange;
  double minPt;
  double vertexMinAngleCosine;
  double vertexMinDLen2DSig;
  double vertexMinDLenSig;
  double fitterSigmacut;
  double fitterTini;
  double fitterRatio;
  bool useVertexFitter;
  bool useVertexReco;
  std::unique_ptr<VertexReconstructor> vtxReco;
  std::unique_ptr<TracksClusteringFromDisplacedSeed> clusterizer;
};


template <class InputContainer, class VTX>
TemplatedInclusiveVertexFinder<InputContainer, VTX>::TemplatedInclusiveVertexFinder(const edm::ParameterSet &params)
    : minHits(params.getParameter<unsigned int>("minHits")),
      maxNTracks(params.getParameter<unsigned int>("maxNTracks")),
      maxLIP(params.getParameter<double>("maximumLongitudinalImpactParameter")),
      maxTimeSig(params.getParameter<double>("maximumTimeSignificance")),
      // useMTDTiming(params.getParameter<bool>("useMTDTiming")),
      timeReference(params.getParameter<std::string>("timeReference")),
      // timeQualityThreshold(params.getParameter<double>("timeQualityThreshold")),  // 0.5
      // cutTimeRange(params.getParameter<bool>("cutTimeRange")),
      // maxTimeRange(params.getParameter<double>("maxTimeRange")),                  // 0.4
      minPt(params.getParameter<double>("minPt")),                                // 0.8
      vertexMinAngleCosine(params.getParameter<double>("vertexMinAngleCosine")),  // 0.98
      vertexMinDLen2DSig(params.getParameter<double>("vertexMinDLen2DSig")),      // 2.5
      vertexMinDLenSig(params.getParameter<double>("vertexMinDLenSig")),          // 0.5
      fitterSigmacut(params.getParameter<double>("fitterSigmacut")),
      fitterTini(params.getParameter<double>("fitterTini")),
      fitterRatio(params.getParameter<double>("fitterRatio")),
      useVertexFitter(params.getParameter<bool>("useDirectVertexFitter")),
      useVertexReco(params.getParameter<bool>("useVertexReco")),
      vtxReco(new ConfigurableVertexReconstructor(params.getParameter<edm::ParameterSet>("vertexReco"))),
      clusterizer(new TracksClusteringFromDisplacedSeed(params.getParameter<edm::ParameterSet>("clusterizer"))) {

  token_beamSpot = consumes<reco::BeamSpot>(params.getParameter<edm::InputTag>("beamSpot"));
  token_primaryVertex = consumes<reco::VertexCollection>(params.getParameter<edm::InputTag>("primaryVertices"));
  token_tracks = consumes<InputContainer>(params.getParameter<edm::InputTag>("tracks"));
  token_trackBuilder = esConsumes<TransientTrackBuilder, TransientTrackRecord>(edm::ESInputTag("", "TransientTrackBuilder"));

  if (timeReference == "beamSpot") {
    std::cout << "chose beam spot as time reference" << std::endl;
    clusterizer->setTimeValueMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeValueMap")));
    clusterizer->setTimeErrorMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeErrorMap")));
    clusterizer->setTimeQualityMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeQualityMap")));
  }
  // else if (timeReference == "primaryVertex") {
  //   clusterizer->setTimeValueMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeValueMap")));
  //   clusterizer->setTimeErrorMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeErrorMap")));
  //   clusterizer->setTimeQualityMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeQualityMap")));
  // }
  else {
    std::cout << "no chosen time reference, using beam spot as default" << std::endl;
    clusterizer->setTimeValueMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeValueMap")));
    clusterizer->setTimeErrorMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeErrorMap")));
    clusterizer->setTimeQualityMapToken(consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeQualityMap")));
  }

  // token_timeValueMap = consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeValueMap"));
  // token_timeErrorMap = consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeErrorMap"));
  // token_timeQualityMap = consumes<edm::ValueMap<float>>(params.getParameter<edm::InputTag>("timeQualityMap"));

  produces<unsigned int>("nClusters");
  produces<Product>();
  // produces<reco::VertexCollection>("multi");
}


template <class InputContainer, class VTX>
bool TemplatedInclusiveVertexFinder<InputContainer, VTX>::trackFilter(const reco::Track &track) const {
  if (track.hitPattern().numberOfValidHits() < (int)minHits)
    return false;
  if (track.pt() < minPt)
    return false;

  return true;
}


template <class InputContainer, class VTX>
void TemplatedInclusiveVertexFinder<InputContainer, VTX>::produce(edm::Event &event, const edm::EventSetup &es) {
  using namespace reco;

  VertexDistance3D vdist;
  VertexDistanceXY vdist2d;
  MultiVertexFitter theMultiVertexFitter;
  AdaptiveVertexFitter theAdaptiveFitter(GeometricAnnealing(fitterSigmacut, fitterTini, fitterRatio),
                                         DefaultLinearizationPointFinder(),
                                         KalmanVertexUpdator<5>(),
                                         KalmanVertexTrackCompatibilityEstimator<5>(),
                                         KalmanVertexSmoother());

  edm::Handle<BeamSpot> beamSpot;
  event.getByToken(token_beamSpot, beamSpot);

  edm::Handle<VertexCollection> primaryVertices;
  event.getByToken(token_primaryVertex, primaryVertices);

  edm::Handle<InputContainer> tracks;
  event.getByToken(token_tracks, tracks);

  // edm::Handle<edm::ValueMap<float>> edm_timeValueMap;
  // event.getByToken(token_timeValueMap, edm_timeValueMap);
  // const edm::ValueMap<float>& timeValueMap = *(edm_timeValueMap.product());

  // edm::Handle<edm::ValueMap<float>> edm_timeErrorMap;
  // event.getByToken(token_timeErrorMap, edm_timeErrorMap);
  // const edm::ValueMap<float>& timeErrorMap = *(edm_timeErrorMap.product());

  // edm::Handle<edm::ValueMap<float>> edm_timeQualityMap;
  // event.getByToken(token_timeQualityMap, edm_timeQualityMap);
  // const edm::ValueMap<float>& timeQualityMap = *(edm_timeQualityMap.product());

  edm::ESHandle<TransientTrackBuilder> trackBuilder = es.getHandle(token_trackBuilder);

  auto recoVertices = std::make_unique<Product>();
  unsigned nClusters = 0;
  if (!primaryVertices->empty()) {
    const reco::Vertex &pv = (*primaryVertices)[0];
    GlobalPoint ppv(pv.position().x(), pv.position().y(), pv.position().z());

    std::vector<TransientTrack> tts;
    // Fill transient track vector
    for (typename InputContainer::const_iterator track = tracks->begin(); track != tracks->end(); ++track) {
      // TransientTrack tt = trackBuilder->build(ref);
      // TrackRef ref(tracks, track - tracks->begin());
      TransientTrack tt(tthelpers::buildTT(tracks, trackBuilder, track - tracks->begin()));
      if (!tt.isValid())
        continue;
      if (!trackFilter(tt.track()))
        continue;
      if (std::abs(tt.track().dz(pv.position())) > maxLIP)
        continue;
      if (edm::isFinite(tt.timeExt()) && pv.covariance(3, 3) > 0.) {  // only apply if time available
        auto tError = std::sqrt(std::pow(tt.dtErrorExt(), 2) + pv.covariance(3, 3));
        auto dtSig = std::abs(tt.timeExt() - pv.t()) / tError;
        if (dtSig > maxTimeSig)
          continue;
      }
      tt.setBeamSpot(*beamSpot);
      tts.push_back(tt);
    }
    std::vector<TracksClusteringFromDisplacedSeed::Cluster> clusters = clusterizer->clusters(event, pv, tts);
        // useMTDTiming, timeValueMap, timeErrorMap, timeQualityMap, timeQualityThreshold, maxTimeRange);

    // Create BS object from PV to feed in the AVR
    BeamSpot::CovarianceMatrix cov;
    for (unsigned int i = 0; i < 7; i++) {
      for (unsigned int j = 0; j < 7; j++) {
        if (i < 3 && j < 3)
          cov(i, j) = pv.covariance(i, j);
        else
          cov(i, j) = 0.0;
      }
    }
    BeamSpot bs(pv.position(), 0.0, 0.0, 0.0, 0.0, cov, BeamSpot::Unknown);

    int i = 0;

#ifdef VTXDEBUG
    std::cout << "CLUSTERS " << clusters.size() << std::endl;
#endif

    nClusters = clusters.size();

    for (std::vector<TracksClusteringFromDisplacedSeed::Cluster>::iterator cluster = clusters.begin();
         cluster != clusters.end();
         ++cluster, ++i) {
      if (cluster->tracks.size() < 2 || cluster->tracks.size() > maxNTracks)
        continue;
      std::vector<TransientVertex> vertices;
      if (useVertexReco) {
        vertices = vtxReco->vertices(cluster->tracks, bs); // Attempt with config given reconstructor
      }
      TransientVertex singleFitVertex;
      if (useVertexFitter) {
        singleFitVertex = theAdaptiveFitter.vertex(cluster->tracks, cluster->seedPoint); // Attempt with direct fitting
        if (singleFitVertex.isValid())
          vertices.push_back(singleFitVertex);
      }

      // For each transient vertex state determine if a time can be measured and fill covariance
      if (pv.covariance(3, 3) > 0.) {
        for (auto &vtx : vertices) {
          svhelper::updateVertexTime(vtx);
        }
      }

      for (std::vector<TransientVertex>::const_iterator v = vertices.begin(); v != vertices.end(); ++v) {
        Measurement1D dlen = vdist.distance(pv, *v);
        Measurement1D dlen2 = vdist2d.distance(pv, *v);

#ifdef VTXDEBUG
        VTX vv(*v);
        std::cout << "V chi2/n: " << v->normalisedChiSquared() << " ndof: " << v->degreesOfFreedom();
        std::cout << " dlen: " << dlen.value() << " error: " << dlen.error() << " signif: " << dlen.significance();
        std::cout << " dlen2: " << dlen2.value() << " error2: " << dlen2.error()
                  << " signif2: " << dlen2.significance();
        std::cout << " pos: " << vv.position() << std::endl;
        // std::cout << " pos: " << vv.position() << " error: " << vv.xError() << " " << vv.yError() << " " << vv.zError()
        //           << std::endl;
        // std::cout << " time: " << vv.time() << " error: " << vv.tError() << std::endl;
#endif

        GlobalVector dir;
        std::vector<reco::TransientTrack> ts = v->originalTracks();
        for (std::vector<reco::TransientTrack>::const_iterator i = ts.begin(); i != ts.end(); ++i) {
          float w = v->trackWeight(*i);
          if (w > 0.5)
            dir += i->impactPointState().globalDirection();

#ifdef VTXDEBUG
          std::cout << "\t[" << (*i).track().pt() << ": " << (*i).track().eta() << ", " << (*i).track().phi() << "], "
                    << w << std::endl;
#endif

        }
        GlobalPoint sv((*v).position().x(), (*v).position().y(), (*v).position().z());
        float vscal = dir.unit().dot((sv - ppv).unit());
        if (dlen.significance() > vertexMinDLenSig &&
            ((vertexMinAngleCosine > 0) ? (vscal > vertexMinAngleCosine) : (vscal < vertexMinAngleCosine)) &&
            v->normalisedChiSquared() < 10 && dlen2.significance() > vertexMinDLen2DSig) {
          recoVertices->push_back(*v);

#ifdef VTXDEBUG
          std::cout << "ADDED" << std::endl;
#endif

        }
      }
    }

#ifdef VTXDEBUG
    std::cout << "Final put  " << recoVertices->size() << std::endl;
#endif

  }

  event.put(std::make_unique<unsigned int>(nClusters), "nClusters");
  event.put(std::move(recoVertices));
}

#endif
