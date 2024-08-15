// -*- C++ -*-
//
// Package:    PhysicsTools/TopLeptonMVAIDProducer
// Class:      TopLeptonMVAIDProducer
//
/**\class TopLeptonMVAIDProducer TopLeptonMVAIDProducer.cc PhysicsTools/TopLeptonMVAIDProducer/plugins/TopLeptonMVAIDProducer.cc

 Description: Producer for Top Lepton MVA ID in Run 2 UL data/MC

 Implementation:
     Based on this implementation for miniAOD:
     https://github.com/GhentAnalysis/heavyNeutrino/blob/UL_master/multilep/src/LeptonMvaHelper.cc
*/
//
// Original Author:  Emily Minyun Tsai
//         Created:  Thu, 15 Aug 2024 19:35:07 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

//
// class declaration
//
template <class T>
class TopLeptonMVAIDProducer : public edm::stream::EDProducer<> {
public:
  explicit TopLeptonMVAIDProducer(const edm::ParameterSet& iConfig) :
      src_(consumes<edm::View<T>>(iConfig.getParameter<edm::InputTag>("src"))) {
      // weights_v1_,
      // weights_v2_ {
    produces<edm::ValueMap<float>>("v1");
    produces<edm::ValueMap<float>>("v2");
  }
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
  void getMVAEle(edm::Event&) const;
  void getMVAMu(edm::Event&) const;

  // ----------member data ---------------------------
  edm::EDGetTokenT<edm::View<T>> src_;
  // weight file v1
  // weight file v2
};

// ------------ method called to produce the data  ------------
template <typename T>
void TopLeptonMVAIDProducer<T>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  if ((typeid(T) == typeid(pat::Electron))) getMVAEle(iEvent);
  else if ((typeid(T) == typeid(pat::Muon))) getMVAMu(iEvent);
}

template <>
void TopLeptonMVAIDProducer<pat::Electron>::getMVAEle(edm::Event& iEvent) const {
  edm::Handle<edm::View<pat::Electron>> src;
  iEvent.getByToken(src_, src);

  std::vector<float> MVAv1;
  std::vector<float> MVAv2;
  MVAv1.reserve(src->size());
  MVAv2.reserve(src->size());
  for (unsigned int i = 0; i < src->size(); i++) MVAv1.push_back(0.1*(float)i);
  for (unsigned int i = 0; i < src->size(); i++) MVAv2.push_back(0.2*(float)i);

  auto MVAv1_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler filler_v1(*MVAv1_out);
  filler_v1.insert(src, MVAv1.begin(), MVAv1.end());
  filler_v1.fill();
  auto MVAv2_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler filler_v2(*MVAv2_out);
  filler_v2.insert(src, MVAv2.begin(), MVAv2.end());
  filler_v2.fill();

  iEvent.put(std::move(MVAv1_out), "v1");
  iEvent.put(std::move(MVAv2_out), "v2");
}

// avoid linking errors, this function is not actually used
template <>
void TopLeptonMVAIDProducer<pat::Muon>::getMVAEle(edm::Event& iEvent) const {}

template <>
void TopLeptonMVAIDProducer<pat::Muon>::getMVAMu(edm::Event& iEvent) const {
  edm::Handle<edm::View<pat::Muon>> src;
  iEvent.getByToken(src_, src);

  std::vector<float> MVAv1;
  std::vector<float> MVAv2;
  MVAv1.reserve(src->size());
  MVAv2.reserve(src->size());
  for (unsigned int i = 0; i < src->size(); i++) MVAv1.push_back(0.3*(float)i);
  for (unsigned int i = 0; i < src->size(); i++) MVAv2.push_back(0.4*(float)i);

  auto MVAv1_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler filler_v1(*MVAv1_out);
  filler_v1.insert(src, MVAv1.begin(), MVAv1.end());
  filler_v1.fill();
  auto MVAv2_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler filler_v2(*MVAv2_out);
  filler_v2.insert(src, MVAv2.begin(), MVAv2.end());
  filler_v2.fill();

  iEvent.put(std::move(MVAv1_out), "v1");
  iEvent.put(std::move(MVAv2_out), "v2");
}

// avoid linking errors, this function is not actually used
template <>
void TopLeptonMVAIDProducer<pat::Electron>::getMVAMu(edm::Event& iEvent) const {}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
template <typename T>
void TopLeptonMVAIDProducer<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src")->setComment("input physics object (lepton) collection");
  // desc.add<edm::FileInPath>("weights_v1")->setComment("xgboost file containing weights for v1");
  // desc.add<edm::FileInPath>("weights_v2")->setComment("xgboost file containing weights for v2");

  std::string prodname;
  if (typeid(T) == typeid(pat::Muon))
    prodname += "Muon";
  else if (typeid(T) == typeid(pat::Electron))
    prodname += "Ele";
  prodname += "TopLeptonMVAIDProducer";

  descriptions.add(prodname, desc);
}

typedef TopLeptonMVAIDProducer<pat::Electron> EleTopLeptonMVAIDProducer;
typedef TopLeptonMVAIDProducer<pat::Muon> MuTopLeptonMVAIDProducer;

DEFINE_FWK_MODULE(EleTopLeptonMVAIDProducer);
DEFINE_FWK_MODULE(MuTopLeptonMVAIDProducer);
