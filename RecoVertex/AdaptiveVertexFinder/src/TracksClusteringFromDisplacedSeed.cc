#include "RecoVertex/AdaptiveVertexFinder/interface/TracksClusteringFromDisplacedSeed.h"


// #define VTXDEBUG 1


TracksClusteringFromDisplacedSeed::TracksClusteringFromDisplacedSeed(const edm::ParameterSet &params)
    : // maxNTracks(params.getParameter<unsigned int>("maxNTracks")),
      useMTD(params.getParameter<bool>("useMTD")),
      // trackTimeQualityThreshold(params.getParameter<double>("trackTimeQualityThreshold")), // 0.5
      max3DIPSignificance(params.getParameter<double>("seedMax3DIPSignificance")),
      max3DIPValue(params.getParameter<double>("seedMax3DIPValue")),
      min3DIPSignificance(params.getParameter<double>("seedMin3DIPSignificance")),
      min3DIPValue(params.getParameter<double>("seedMin3DIPValue")),
      cutTimeRange(params.getParameter<bool>("cutTimeRange")),
      maxTimeRange(params.getParameter<double>("maxTimeRange")),                           // 0.4
      clusterMaxDistance(params.getParameter<double>("clusterMaxDistance")),
      clusterMaxSignificance(params.getParameter<double>("clusterMaxSignificance")),       // 3
      distanceRatio(params.getParameter<double>("distanceRatio")),                         // was clusterScale/densityFactor
      clusterMinAngleCosine(params.getParameter<double>("clusterMinAngleCosine")),         // 0.0
      maxTimeSignificance(params.getParameter<double>("maxTimeSignificance")) {}


std::pair<std::vector<reco::TransientTrack>, GlobalPoint> TracksClusteringFromDisplacedSeed::nearTracks(
    const reco::TransientTrack &seed, const std::vector<reco::TransientTrack> &tracks,
    const reco::Vertex &primaryVertex) const {

  VertexDistance3D distanceComputer;
  GlobalPoint pv(primaryVertex.position().x(), primaryVertex.position().y(), primaryVertex.position().z());
  std::vector<reco::TransientTrack> result;
  TwoTrackMinimumDistance dist;
  GlobalPoint seedingPoint;
  float sumWeights = 0;
  std::pair<bool, Measurement1D> ipSeed = IPTools::absoluteImpactParameter3D(seed, primaryVertex);
  float pvDistance = ipSeed.second.value();
  for (std::vector<reco::TransientTrack>::const_iterator tt = tracks.begin(); tt != tracks.end(); ++tt) {
    if (*tt == seed)
      continue;

    float seedTime = -999.9; // Dummy value
    float trackTime = -999.9; // Dummy value
    if (useMTD) {
      if (!trackTimeValueMap.contains(tt->trackBaseRef().id())) continue;
      if (trackTimeErrorMap[tt->trackBaseRef()] == -1.0) continue; // Remove fakes
      // if (trackTimeQualityMap[tt->trackBaseRef()] < trackTimeQualityThreshold) continue; // Quality cut
      seedTime = trackTimeValueMap[seed.trackBaseRef()];
      trackTime = trackTimeValueMap[tt->trackBaseRef()];
      if (cutTimeRange) {
        float timeDiff = std::abs(seedTime - trackTime);
        if (timeDiff > maxTimeRange / 2.0) continue;
      }
    }

    if (dist.calculate(tt->impactPointState(), seed.impactPointState())) {
      GlobalPoint ttPoint = dist.points().first;
      GlobalError ttPointErr = tt->impactPointState().cartesianError().position();
      GlobalPoint seedPosition = dist.points().second;
      GlobalError seedPositionErr = seed.impactPointState().cartesianError().position();
      Measurement1D m = distanceComputer.distance(VertexState(seedPosition, seedPositionErr), VertexState(ttPoint, ttPointErr));
      GlobalPoint cp(dist.crossingPoint());

      double timeSig = 0.0;
      if (primaryVertex.covariance(3, 3) > 0.0 && edm::isFinite(seed.timeExt()) && edm::isFinite(tt->timeExt())) {
        // Apply only if time available and being used in vertexing
        const double tError = std::sqrt(std::pow(seed.dtErrorExt(), 2) + std::pow(tt->dtErrorExt(), 2));
        timeSig = std::abs(seed.timeExt() - tt->timeExt()) / tError;
      }

      float distanceFromPV = (dist.points().second - pv).mag();
      float distance = dist.distance();
      GlobalVector trackDir2D(tt->impactPointState().globalDirection().x(), tt->impactPointState().globalDirection().y(), 0.0);
      GlobalVector seedDir2D(seed.impactPointState().globalDirection().x(), seed.impactPointState().globalDirection().y(), 0.0);
      // SK:UNUSED // float dotprodTrackSeed2D = trackDir2D.unit().dot(seedDir2D.unit());

      float dotprodTrack = (dist.points().first - pv).unit().dot(tt->impactPointState().globalDirection().unit());
      float dotprodSeed = (dist.points().second - pv).unit().dot(seed.impactPointState().globalDirection().unit());

      float w = distanceFromPV * distanceFromPV / (pvDistance * distance);
      bool selected =
          (m.significance() < clusterMaxSignificance &&
          ((clusterMinAngleCosine > 0)
              ? (dotprodSeed > clusterMinAngleCosine)
              : (dotprodSeed < clusterMinAngleCosine)) && // Angles between PV-PCAonSeed vectors and seed directions
          ((clusterMinAngleCosine > 0)
              ? (dotprodTrack > clusterMinAngleCosine)
              : (dotprodTrack < clusterMinAngleCosine)) && // Angles between PV-PCAonTrack vectors and track directions
          // dotprodTrackSeed2D > clusterMinAngleCosine && // Angle between track and seed
          // distance*clusterScale * tracks.size() < (distanceFromPV + pvDistance) * (distanceFromPV + pvDistance) / pvDistance && // cut scaling with track density
          distance * distanceRatio < distanceFromPV &&  // Cut scaling with track density
          distance < clusterMaxDistance &&
          timeSig < maxTimeSignificance); // Absolute distance cut

#ifdef VTXDEBUG
      std::cout << tt->trackBaseRef().key() << " :  " << (selected ? "+" : " ") << " " << m.significance() << " < "
                << clusterMaxSignificance << " &&  " << dotprodSeed << " > " << clusterMinAngleCosine << "  && "
                << dotprodTrack << " > " << clusterMinAngleCosine << "  && " << dotprodTrackSeed2D << " > "
                << clusterMinAngleCosine << "  &&  " << distance * distanceRatio << " < " << distanceFromPV
                << "  crossingtoPV: " << distanceFromPV << " dis*scal " << distance * distanceRatio << "  <  "
                << distanceFromPV << " dist: " << distance << " < " << clusterMaxDistance < < < <
                "timeSig: " << timeSig << std::endl; // Cut scaling with track density
#endif

      if (selected) {
        result.push_back(*tt);
        seedingPoint = GlobalPoint(cp.x() * w + seedingPoint.x(), cp.y() * w + seedingPoint.y(), cp.z() * w + seedingPoint.z());
        sumWeights += w;
      }
    }
  }

  seedingPoint = GlobalPoint(seedingPoint.x() / sumWeights, seedingPoint.y() / sumWeights, seedingPoint.z() / sumWeights);
  return std::pair<std::vector<reco::TransientTrack>, GlobalPoint>(result, seedingPoint);
}


std::vector<TracksClusteringFromDisplacedSeed::Cluster> TracksClusteringFromDisplacedSeed::clusters(
    const edm::Event &event, const reco::Vertex &pv, const std::vector<reco::TransientTrack> &selectedTracks) {

  using namespace reco;

  edm::Handle<edm::ValueMap<float>> edm_trackTimeValueMap;
  event.getByToken(token_trackTimeValueMap, edm_trackTimeValueMap);

  edm::Handle<edm::ValueMap<float>> edm_trackTimeErrorMap;
  event.getByToken(token_trackTimeErrorMap, edm_trackTimeErrorMap);

  // edm::Handle<edm::ValueMap<float>> edm_trackTimeQualityMap;
  // event.getByToken(token_trackTimeQualityMap, edm_trackTimeQualityMap);

  trackTimeValueMap = *(edm_trackTimeValueMap.product());
  trackTimeErrorMap = *(edm_trackTimeErrorMap.product());
  // trackTimeQualityMap = *(edm_trackTimeQualityMap.product());

  std::vector<TransientTrack> seeds;
  for (std::vector<TransientTrack>::const_iterator it = selectedTracks.begin(); it != selectedTracks.end(); it++) {
    std::pair<bool, Measurement1D> ip = IPTools::absoluteImpactParameter3D(*it, pv);
    if (ip.first && ip.second.value() >= min3DIPValue && ip.second.significance() >= min3DIPSignificance &&
        ip.second.value() <= max3DIPValue && ip.second.significance() <= max3DIPSignificance) {

      if (useMTD) { // Skip seeds with no/fake MTD timing information
        if (!trackTimeErrorMap.contains(it->trackBaseRef().id())) continue;
        if (trackTimeErrorMap[it->trackBaseRef()] == -1.0) continue;
        // if (trackTimeQualityMap[it->trackBaseRef()] < 0.5) continue;
      }

#ifdef VTXDEBUG
      std::cout << "new seed " << it - selectedTracks.begin() << " ref " << it->trackBaseRef().key() << " "
                << ip.second.value() << " " << ip.second.significance() << " "
                << it->track().hitPattern().trackerLayersWithMeasurement() << " " << it->track().pt() << " "
                << it->track().eta() << std::endl;
#endif

      seeds.push_back(*it);
    }
  }

  std::vector<Cluster> clusters;
  int i = 0;
  for (std::vector<TransientTrack>::const_iterator s = seeds.begin(); s != seeds.end(); ++s, ++i) {

#ifdef VTXDEBUG
    std::cout << "Seed N. " << i << std::endl;
#endif

    std::pair<std::vector<reco::TransientTrack>, GlobalPoint> ntracks = nearTracks(*s, selectedTracks, pv);
    // std::cout << ntracks.first.size() << " " << ntracks.first.size() << std::endl;
    // if (ntracks.first.size() == 0 || ntracks.first.size() > maxNTracks) continue;
    ntracks.first.push_back(*s);
    Cluster aCl;
    aCl.seedingTrack = *s;
    aCl.seedPoint = ntracks.second;
    aCl.tracks = ntracks.first;
    clusters.push_back(aCl);
  }

  return clusters;
}
