// -*- C++ -*-
//
// Package:    PhysicsTools/TopLeptonMVAProducer
// Class:      TopLeptonMVAProducer
//
/**\class TopLeptonMVAProducer TopLeptonMVAProducer.cc PhysicsTools/TopLeptonMVAProducer/plugins/TopLeptonMVAProducer.cc

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
#include "DataFormats/PatCandidates/interface/Jet.h"

#include <xgboost/c_api.h>

//
// class declaration
//
template <class T>
class TopLeptonMVAProducer : public edm::stream::EDProducer<> {
public:
  explicit TopLeptonMVAProducer(const edm::ParameterSet& iConfig) :
      leptons_(consumes<edm::View<T>>(iConfig.getParameter<edm::InputTag>("leptons"))),
      jetNDauCharged_(consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("jetNDauCharged"))),
      miniIsoChg_(consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("miniIsoChg"))),
      miniIsoAll_(consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("miniIsoAll"))),
      ptRel_(consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("ptRel"))),
      ptRatio_(consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("ptRatio"))),
      PFIsoAll_(consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("PFIsoAll"))),
      jetForLepJetVar_(consumes<edm::View<reco::CandidatePtr>>(iConfig.getParameter<edm::InputTag>("jetForLepJetVar"))),
      jets_(consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jets"))),
      primaryVertices_(consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("primaryVertices"))) {

    if (typeid(T) == typeid(pat::Electron)) {
      mvaFall17V2noIso_ = consumes<edm::View<float>>(iConfig.getParameter<edm::InputTag>("mvaFall17V2noIso"));
    }

    produces<edm::ValueMap<float>>("RAWv1");
    produces<edm::ValueMap<float>>("RAWv2");
    produces<edm::ValueMap<int>>("WPv1");
    produces<edm::ValueMap<int>>("WPv2");

    weights_v1_ = iConfig.getParameter<edm::FileInPath>("weights_v1");
    weights_v2_ = iConfig.getParameter<edm::FileInPath>("weights_v2");
    // features_v1_ = iConfig.getParameter<std::vector<std::string>>("features_v1");
    // features_v2_ = iConfig.getParameter<std::vector<std::string>>("features_v2");

    XGBoosterCreate(NULL, 0, &booster_v1_);
    XGBoosterCreate(NULL, 0, &booster_v2_);
    XGBoosterLoadModel(booster_v1_, weights_v1_.fullPath().c_str());
    XGBoosterLoadModel(booster_v2_, weights_v2_.fullPath().c_str());
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
  void getMVAEle(edm::Event&) const;
  void getMVAMu(edm::Event&) const;

  // ----------member data ---------------------------
  edm::EDGetTokenT<edm::View<T>> leptons_;
  edm::EDGetTokenT<edm::View<float>> jetNDauCharged_;
  edm::EDGetTokenT<edm::View<float>> miniIsoChg_;
  edm::EDGetTokenT<edm::View<float>> miniIsoAll_;
  edm::EDGetTokenT<edm::View<float>> ptRel_;
  edm::EDGetTokenT<edm::View<float>> ptRatio_;
  edm::EDGetTokenT<edm::View<float>> PFIsoAll_;
  edm::EDGetTokenT<edm::View<reco::CandidatePtr>> jetForLepJetVar_;
  edm::EDGetTokenT<edm::View<pat::Jet>> jets_;
  edm::EDGetTokenT<std::vector<reco::Vertex>> primaryVertices_;
  edm::EDGetTokenT<edm::View<float>> mvaFall17V2noIso_;

  edm::FileInPath weights_v1_;
  edm::FileInPath weights_v2_;
  // std::vector<std::string> features_v1_;
  // std::vector<std::string> features_v2_;
  BoosterHandle booster_v1_;
  BoosterHandle booster_v2_;

};

// ------------ method called to produce the data  ------------
template <typename T>
void TopLeptonMVAProducer<T>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  if (typeid(T) == typeid(pat::Electron)) getMVAEle(iEvent);
  else if (typeid(T) == typeid(pat::Muon)) getMVAMu(iEvent);
}

template <>
void TopLeptonMVAProducer<pat::Electron>::getMVAEle(edm::Event& iEvent) const {
  edm::Handle<edm::View<pat::Electron>> electrons;
  edm::Handle<edm::View<pat::Jet>> jets;
  iEvent.getByToken(leptons_, electrons);
  iEvent.getByToken(jets_, jets);

  std::vector<float> MVAv1;
  std::vector<float> MVAv2;
  std::vector<int> WPv1;
  std::vector<int> WPv2;
  MVAv1.reserve(electrons->size());
  MVAv2.reserve(electrons->size());
  WPv1.reserve(electrons->size());
  WPv2.reserve(electrons->size());

for (const pat::Electron& electron : *electrons) {
  float pt = electron.pt();
  float absEta = std::abs(electron.eta());
  //
}

  for (unsigned int i = 0; i < electrons->size(); i++) {
    MVAv1.push_back(0.1*(float)i);
    MVAv2.push_back(0.2*(float)i);
    WPv1.push_back(i);
    WPv2.push_back(i*2);
  }

  auto MVAv1_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler MVAv1_filler(*MVAv1_out);
  MVAv1_filler.insert(electrons, MVAv1.begin(), MVAv1.end());
  MVAv1_filler.fill();
  auto MVAv2_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler MVAv2_filler(*MVAv2_out);
  MVAv2_filler.insert(electrons, MVAv2.begin(), MVAv2.end());
  MVAv2_filler.fill();
  auto WPv1_out = std::make_unique<edm::ValueMap<int>>();
  edm::ValueMap<int>::Filler WPv1_filler(*WPv1_out);
  WPv1_filler.insert(electrons, WPv1.begin(), WPv1.end());
  WPv1_filler.fill();
  auto WPv2_out = std::make_unique<edm::ValueMap<int>>();
  edm::ValueMap<int>::Filler WPv2_filler(*WPv2_out);
  WPv2_filler.insert(electrons, WPv2.begin(), WPv2.end());
  WPv2_filler.fill();

  iEvent.put(std::move(MVAv1_out), "RAWv1");
  iEvent.put(std::move(MVAv2_out), "RAWv2");
  iEvent.put(std::move(WPv1_out), "WPv1");
  iEvent.put(std::move(WPv2_out), "WPv2");
}

template <>
void TopLeptonMVAProducer<pat::Muon>::getMVAMu(edm::Event& iEvent) const {
  edm::Handle<edm::View<pat::Muon>> muons;
  edm::Handle<edm::View<pat::Jet>> jets;
  iEvent.getByToken(leptons_, muons);
  iEvent.getByToken(jets_, jets);

  std::vector<float> MVAv1;
  std::vector<float> MVAv2;
  std::vector<int> WPv1;
  std::vector<int> WPv2;
  MVAv1.reserve(muons->size());
  MVAv2.reserve(muons->size());
  WPv1.reserve(muons->size());
  WPv2.reserve(muons->size());

  for (unsigned int i = 0; i < muons->size(); i++) {
    MVAv1.push_back(0.3*(float)i);
    MVAv2.push_back(0.4*(float)i);
    WPv1.push_back(3*i);
    WPv2.push_back(4*i);
  }

  auto MVAv1_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler MVAv1_filler(*MVAv1_out);
  MVAv1_filler.insert(muons, MVAv1.begin(), MVAv1.end());
  MVAv1_filler.fill();
  auto MVAv2_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler MVAv2_filler(*MVAv2_out);
  MVAv2_filler.insert(muons, MVAv2.begin(), MVAv2.end());
  MVAv2_filler.fill();
  auto WPv1_out = std::make_unique<edm::ValueMap<int>>();
  edm::ValueMap<int>::Filler WPv1_filler(*WPv1_out);
  WPv1_filler.insert(muons, WPv1.begin(), WPv1.end());
  WPv1_filler.fill();
  auto WPv2_out = std::make_unique<edm::ValueMap<int>>();
  edm::ValueMap<int>::Filler WPv2_filler(*WPv2_out);
  WPv2_filler.insert(muons, WPv2.begin(), WPv2.end());
  WPv2_filler.fill();

  iEvent.put(std::move(MVAv1_out), "RAWv1");
  iEvent.put(std::move(MVAv2_out), "RAWv2");
  iEvent.put(std::move(WPv1_out), "WPv1");
  iEvent.put(std::move(WPv2_out), "WPv2");
}

// avoid linking errors, this function is not actually used
template <>
void TopLeptonMVAProducer<pat::Muon>::getMVAEle(edm::Event& iEvent) const {}

// avoid linking errors, this function is not actually used
template <>
void TopLeptonMVAProducer<pat::Electron>::getMVAMu(edm::Event& iEvent) const {}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
template <typename T>
void TopLeptonMVAProducer<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::FileInPath>("weights_v1")->setComment("json file containing weights for v1");
  desc.add<edm::FileInPath>("weights_v2")->setComment("json file containing weights for v2");
  // desc.add<std::vector<std::string>>("features_v1")->setComment("features for v1");
  // desc.add<std::vector<std::string>>("features_v2")->setComment("features for v2");

  desc.add<edm::InputTag>("leptons")->setComment("lepton collection");
  desc.add<edm::InputTag>("jetNDauCharged")->setComment("number of charged daughters of the closest jet");
  desc.add<edm::InputTag>("miniIsoChg")->setComment("charged component of mini PF isolation");
  desc.add<edm::InputTag>("miniIsoAll")->setComment("total mini PF isolation");
  desc.add<edm::InputTag>("ptRel")->setComment("relative momentum of the lepton with respect to the closest jet after subtracting the lepton");
  desc.add<edm::InputTag>("ptRatio")->setComment("min(1 / (jetRelIso + 1), 1.5)");
  desc.add<edm::InputTag>("PFIsoAll")->setComment("PF relative isolation dR=0.3");
  desc.add<edm::InputTag>("jetForLepJetVar")->setComment("closest jet");
  desc.add<edm::InputTag>("jets")->setComment("input jet collection");
  desc.add<edm::InputTag>("primaryVertices")->setComment("primary vertex collection");
  desc.addOptional<edm::InputTag>("mvaFall17V2noIso")->setComment("MVA noIso ID V2 score");

  std::string prodname;
  if (typeid(T) == typeid(pat::Muon))
    prodname += "Muon";
  else if (typeid(T) == typeid(pat::Electron))
    prodname += "Ele";
  prodname += "TopLeptonMVAProducer";

  descriptions.add(prodname, desc);
}

typedef TopLeptonMVAProducer<pat::Electron> EleTopLeptonMVAProducer;
typedef TopLeptonMVAProducer<pat::Muon> MuTopLeptonMVAProducer;

DEFINE_FWK_MODULE(EleTopLeptonMVAProducer);
DEFINE_FWK_MODULE(MuTopLeptonMVAProducer);
