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

#include <xgboost/c_api.h>

//
// class declaration
//
template <class T>
class TopLeptonMVAProducer : public edm::stream::EDProducer<> {
public:
  explicit TopLeptonMVAProducer(const edm::ParameterSet& iConfig) :
      leptons_(consumes<std::vector<T>>(iConfig.getParameter<edm::InputTag>("leptons"))),
      jetNDauCharged_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("jetNDauCharged"))),
      miniIsoChg_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("miniIsoChg"))),
      miniIsoAll_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("miniIsoAll"))),
      ptRel_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("ptRel"))),
      ptRatio_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("ptRatio"))),
      jetBTag_(consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("jetBTag"))),
      jetForLepJetVar_(consumes<edm::ValueMap<reco::CandidatePtr>>(iConfig.getParameter<edm::InputTag>("jetForLepJetVar"))) {

    if (typeid(T) == typeid(pat::Electron)) {
      PFIsoAll04_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("PFIsoAll04"));
      PFIsoAll_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("PFIsoAll"));
      mvaFall17V2noIso_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("mvaFall17V2noIso"));
    }

    produces<edm::ValueMap<float>>("jetPtRatio");
    produces<edm::ValueMap<float>>("RAWv1");
    produces<edm::ValueMap<float>>("RAWv2");
    produces<edm::ValueMap<int>>("WPv1");
    produces<edm::ValueMap<int>>("WPv2");

    weights_v1_ = iConfig.getParameter<edm::FileInPath>("weights_v1");
    weights_v2_ = iConfig.getParameter<edm::FileInPath>("weights_v2");
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
  edm::EDGetTokenT<std::vector<T>> leptons_;
  edm::EDGetTokenT<edm::ValueMap<float>> jetNDauCharged_;
  edm::EDGetTokenT<edm::ValueMap<float>> miniIsoChg_;
  edm::EDGetTokenT<edm::ValueMap<float>> miniIsoAll_;
  edm::EDGetTokenT<edm::ValueMap<float>> ptRel_;
  edm::EDGetTokenT<edm::ValueMap<float>> ptRatio_;
  edm::EDGetTokenT<edm::ValueMap<float>> PFIsoAll04_;
  edm::EDGetTokenT<edm::ValueMap<float>> PFIsoAll_;
  edm::EDGetTokenT<edm::ValueMap<float>> jetBTag_;
  edm::EDGetTokenT<edm::ValueMap<float>> mvaFall17V2noIso_;
  edm::EDGetTokenT<edm::ValueMap<reco::CandidatePtr>> jetForLepJetVar_;

  edm::FileInPath weights_v1_;
  edm::FileInPath weights_v2_;
  BoosterHandle booster_v1_;
  BoosterHandle booster_v2_;

  const float workingPoints_v1[4] = {0.20, 0.41, 0.64, 0.81};
  const float workingPoints_v2[4] = {0.59, 0.81, 0.90, 0.94};
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
  edm::Handle<std::vector<pat::Electron>> electrons;
  edm::Handle<edm::ValueMap<float>> jetNDauChargedVM;
  edm::Handle<edm::ValueMap<float>> miniIsoChgVM;
  edm::Handle<edm::ValueMap<float>> miniIsoAllVM;
  edm::Handle<edm::ValueMap<float>> ptRelVM;
  edm::Handle<edm::ValueMap<float>> ptRatioVM;
  edm::Handle<edm::ValueMap<float>> PFIsoAll04VM;
  edm::Handle<edm::ValueMap<float>> PFIsoAllVM;
  edm::Handle<edm::ValueMap<float>> jetBTagVM;
  edm::Handle<edm::ValueMap<float>> mvaFall17V2noIsoVM;
  edm::Handle<edm::ValueMap<reco::CandidatePtr>> jetForLepJetVarVM;
  iEvent.getByToken(leptons_, electrons);
  iEvent.getByToken(jetNDauCharged_, jetNDauChargedVM);
  iEvent.getByToken(miniIsoChg_, miniIsoChgVM);
  iEvent.getByToken(miniIsoAll_, miniIsoAllVM);
  iEvent.getByToken(ptRel_, ptRelVM);
  iEvent.getByToken(ptRatio_, ptRatioVM);
  iEvent.getByToken(PFIsoAll04_, PFIsoAll04VM);
  iEvent.getByToken(PFIsoAll_, PFIsoAllVM);
  iEvent.getByToken(jetBTag_, jetBTagVM);
  iEvent.getByToken(mvaFall17V2noIso_, mvaFall17V2noIsoVM);
  iEvent.getByToken(jetForLepJetVar_, jetForLepJetVarVM);

  std::vector<float> eleJetPtRatio;
  std::vector<float> MVAv1;
  std::vector<float> MVAv2;
  std::vector<int> WPv1;
  std::vector<int> WPv2;
  eleJetPtRatio.reserve(electrons->size());
  MVAv1.reserve(electrons->size());
  MVAv2.reserve(electrons->size());
  WPv1.reserve(electrons->size());
  WPv2.reserve(electrons->size());

  const int nFeatures_v1 = 13;
  const int nFeatures_v2 = 14;
  for (unsigned int iEle = 0; iEle < electrons->size(); iEle++) {
    const pat::ElectronRef eleRef(electrons, iEle);

    float pt = eleRef->pt();
    float absEta = std::abs(eleRef->eta());
    float jetNDauCharged = (*jetNDauChargedVM)[eleRef];
    float miniPFRelIso_chg = (*miniIsoChgVM)[eleRef] / pt;
    float miniPFRelIso_neu = (*miniIsoAllVM)[eleRef] / pt - miniPFRelIso_chg;
    float jetPtRelv2 = (*jetForLepJetVarVM)[eleRef].isNonnull() ? (*ptRelVM)[eleRef] : 0.0; 
    float jetPtRatio = (*jetForLepJetVarVM)[eleRef].isNonnull()
        ? std::min((double)(*ptRatioVM)[eleRef], 1.5)
        : 1.0 / (1.0 + (*PFIsoAll04VM)[eleRef] / pt);
    float pfRelIso03_all = (*PFIsoAllVM)[eleRef] / pt;
    float jetBTag = (*jetForLepJetVarVM)[eleRef].isNonnull() ? std::max((double)(*jetBTagVM)[eleRef], 0.0) : 0.0;
    float sip3d = eleRef->dB(pat::Electron::PV3D) / eleRef->edB(pat::Electron::PV3D);
    float logAbsDxy = std::log(std::abs(eleRef->dB(pat::Electron::PV2D)));
    float logAbsDz = std::log(std::abs(eleRef->dB(pat::Electron::PVDZ)));
    float mvaFall17V2noIso = (*mvaFall17V2noIsoVM)[eleRef];
    float lostHits = eleRef->gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS);

    eleJetPtRatio.push_back(jetPtRatio);

    float features_v1[1][nFeatures_v1];
    features_v1[0][0] = pt;
    features_v1[0][1] = absEta;
    features_v1[0][2] = jetNDauCharged;
    features_v1[0][3] = miniPFRelIso_chg;
    features_v1[0][4] = miniPFRelIso_neu;
    features_v1[0][5] = jetPtRelv2;
    features_v1[0][6] = jetPtRatio;
    features_v1[0][7] = pfRelIso03_all;
    features_v1[0][8] = jetBTag;
    features_v1[0][9] = sip3d;
    features_v1[0][10] = logAbsDxy;
    features_v1[0][11] = logAbsDz;
    features_v1[0][12] = mvaFall17V2noIso;
    DMatrixHandle dtest_v1;
    XGDMatrixCreateFromMat((float*)features_v1, 1, nFeatures_v1, NAN, &dtest_v1);
    unsigned long out_len_v1;
    const float *raw_v1;
    XGBoosterPredict(booster_v1_, dtest_v1, 0, 0, 0, &out_len_v1, &raw_v1);
    XGDMatrixFree(dtest_v1);
    int wp_v1 = 0;
    for (const float &wp : workingPoints_v1) {
      if (*raw_v1 > wp) wp_v1 += 1;
    }
    MVAv1.push_back(*raw_v1);
    WPv1.push_back(wp_v1);

    float features_v2[1][nFeatures_v2];
    features_v2[0][0] = pt;
    features_v2[0][1] = absEta;
    features_v2[0][2] = jetNDauCharged;
    features_v2[0][3] = miniPFRelIso_chg;
    features_v2[0][4] = miniPFRelIso_neu;
    features_v2[0][5] = jetPtRelv2;
    features_v2[0][6] = jetPtRatio;
    features_v2[0][7] = pfRelIso03_all;
    features_v2[0][8] = jetBTag;
    features_v2[0][9] = sip3d;
    features_v2[0][10] = logAbsDxy;
    features_v2[0][11] = logAbsDz;
    features_v2[0][12] = mvaFall17V2noIso;
    features_v2[0][13] = lostHits;
    DMatrixHandle dtest_v2;
    XGDMatrixCreateFromMat((float*)features_v2, 1, nFeatures_v2, NAN, &dtest_v2);
    unsigned long out_len_v2;
    const float *raw_v2;
    XGBoosterPredict(booster_v2_, dtest_v2, 0, 0, 0, &out_len_v2, &raw_v2);
    XGDMatrixFree(dtest_v2);
    int wp_v2 = 0;
    for (const float &wp : workingPoints_v2) {
      if (*raw_v2 > wp) wp_v2 += 1;
    }
    MVAv2.push_back(*raw_v2);
    WPv2.push_back(wp_v2);
  }

  auto eleJetPtRatio_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler eleJetPtRatio_filler(*eleJetPtRatio_out);
  eleJetPtRatio_filler.insert(electrons, eleJetPtRatio.begin(), eleJetPtRatio.end());
  eleJetPtRatio_filler.fill();
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

  iEvent.put(std::move(eleJetPtRatio_out), "jetPtRatio");
  iEvent.put(std::move(MVAv1_out), "RAWv1");
  iEvent.put(std::move(MVAv2_out), "RAWv2");
  iEvent.put(std::move(WPv1_out), "WPv1");
  iEvent.put(std::move(WPv2_out), "WPv2");
}

template <>
void TopLeptonMVAProducer<pat::Muon>::getMVAMu(edm::Event& iEvent) const {
  edm::Handle<std::vector<pat::Muon>> muons;
  edm::Handle<edm::ValueMap<float>> jetNDauChargedVM;
  edm::Handle<edm::ValueMap<float>> miniIsoChgVM;
  edm::Handle<edm::ValueMap<float>> miniIsoAllVM;
  edm::Handle<edm::ValueMap<float>> ptRelVM;
  edm::Handle<edm::ValueMap<float>> ptRatioVM;
  edm::Handle<edm::ValueMap<float>> jetBTagVM;
  edm::Handle<edm::ValueMap<reco::CandidatePtr>> jetForLepJetVarVM;
  iEvent.getByToken(leptons_, muons);
  iEvent.getByToken(jetNDauCharged_, jetNDauChargedVM);
  iEvent.getByToken(miniIsoChg_, miniIsoChgVM);
  iEvent.getByToken(miniIsoAll_, miniIsoAllVM);
  iEvent.getByToken(ptRel_, ptRelVM);
  iEvent.getByToken(ptRatio_, ptRatioVM);
  iEvent.getByToken(jetBTag_, jetBTagVM);
  iEvent.getByToken(jetForLepJetVar_, jetForLepJetVarVM);

  std::vector<float> muJetPtRatio;
  std::vector<float> MVAv1;
  std::vector<float> MVAv2;
  std::vector<int> WPv1;
  std::vector<int> WPv2;
  muJetPtRatio.reserve(muons->size());
  MVAv1.reserve(muons->size());
  MVAv2.reserve(muons->size());
  WPv1.reserve(muons->size());
  WPv2.reserve(muons->size());

  const int nFeatures = 13;
  for (unsigned int iMu = 0; iMu < muons->size(); iMu++) {
    const pat::MuonRef muRef(muons, iMu);

    float pt = muRef->pt();
    float absEta = std::abs(muRef->eta());
    float jetNDauCharged = (*jetNDauChargedVM)[muRef];
    float miniPFRelIso_chg = (*miniIsoChgVM)[muRef] / pt;
    float miniPFRelIso_neu = (*miniIsoAllVM)[muRef] / pt - miniPFRelIso_chg;
    float jetPtRelv2 = (*jetForLepJetVarVM)[muRef].isNonnull() ? (*ptRelVM)[muRef] : 0.0;
    float jetPtRatio = (*jetForLepJetVarVM)[muRef].isNonnull()
        ? std::min((double)(*ptRatioVM)[muRef], 1.5)
        : 1.0 / (1.0 + (muRef->pfIsolationR04().sumChargedHadronPt + std::max(muRef->pfIsolationR04().sumNeutralHadronEt
            + muRef->pfIsolationR04().sumPhotonEt - muRef->pfIsolationR04().sumPUPt / 2.0, 0.0)) / pt);
    float pfRelIso03_all = (muRef->pfIsolationR03().sumChargedHadronPt + std::max(muRef->pfIsolationR03().sumNeutralHadronEt
        + muRef->pfIsolationR03().sumPhotonEt - muRef->pfIsolationR03().sumPUPt / 2.0, 0.0)) / pt;
    float jetBTag = (*jetForLepJetVarVM)[muRef].isNonnull() ? std::max((double)(*jetBTagVM)[muRef], 0.0) : 0.0;
    float sip3d = muRef->dB(pat::Muon::PV3D) / muRef->edB(pat::Muon::PV3D);
    float logAbsDxy = std::log(std::abs(muRef->dB(pat::Muon::PV2D)));
    float logAbsDz = std::log(std::abs(muRef->dB(pat::Muon::PVDZ)));
    float segmentComp = muRef->segmentCompatibility();

    muJetPtRatio.push_back(jetPtRatio);

    float features[1][nFeatures];
    features[0][0] = pt;
    features[0][1] = absEta;
    features[0][2] = jetNDauCharged;
    features[0][3] = miniPFRelIso_chg;
    features[0][4] = miniPFRelIso_neu;
    features[0][5] = jetPtRelv2;
    features[0][6] = jetPtRatio;
    features[0][7] = pfRelIso03_all;
    features[0][8] = jetBTag;
    features[0][9] = sip3d;
    features[0][10] = logAbsDxy;
    features[0][11] = logAbsDz;
    features[0][12] = segmentComp;

    DMatrixHandle dtest_v1;
    XGDMatrixCreateFromMat((float*)features, 1, nFeatures, NAN, &dtest_v1);
    unsigned long out_len_v1;
    const float *raw_v1;
    XGBoosterPredict(booster_v1_, dtest_v1, 0, 0, 0, &out_len_v1, &raw_v1);
    XGDMatrixFree(dtest_v1);
    int wp_v1 = 0;
    for (const float &wp : workingPoints_v1) {
      if (*raw_v1 > wp) wp_v1 += 1;
    }
    MVAv1.push_back(*raw_v1);
    WPv1.push_back(wp_v1);

    DMatrixHandle dtest_v2;
    XGDMatrixCreateFromMat((float*)features, 1, nFeatures, NAN, &dtest_v2);
    unsigned long out_len_v2;
    const float *raw_v2;
    XGBoosterPredict(booster_v2_, dtest_v2, 0, 0, 0, &out_len_v2, &raw_v2);
    XGDMatrixFree(dtest_v2);
    int wp_v2 = 0;
    for (const float &wp : workingPoints_v2) {
      if (*raw_v2 > wp) wp_v2 += 1;
    }
    MVAv2.push_back(*raw_v2);
    WPv2.push_back(wp_v2);
  }

  auto muJetPtRatio_out = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler muJetPtRatio_filler(*muJetPtRatio_out);
  muJetPtRatio_filler.insert(muons, muJetPtRatio.begin(), muJetPtRatio.end());
  muJetPtRatio_filler.fill();
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

  iEvent.put(std::move(muJetPtRatio_out), "jetPtRatio");
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
  desc.add<edm::InputTag>("leptons")->setComment("lepton collection");
  desc.add<edm::InputTag>("jetNDauCharged")->setComment("number of charged daughters of the closest jet");
  desc.add<edm::InputTag>("miniIsoChg")->setComment("charged component of mini PF isolation");
  desc.add<edm::InputTag>("miniIsoAll")->setComment("total mini PF isolation");
  desc.add<edm::InputTag>("ptRel")->setComment("relative momentum of the lepton with respect to the closest jet after subtracting the lepton");
  desc.add<edm::InputTag>("ptRatio")->setComment("min(1 / (jetRelIso + 1), 1.5)");
  desc.add<edm::InputTag>("jetBTag")->setComment("b-tag of the closest jet");
  desc.add<edm::InputTag>("jetForLepJetVar")->setComment("closest jet");
  // For electrons only
  desc.addOptional<edm::InputTag>("PFIsoAll")->setComment("PF relative isolation dR=0.3");
  desc.addOptional<edm::InputTag>("PFIsoAll04")->setComment("PF relative isolation dR=0.4");
  desc.addOptional<edm::InputTag>("mvaFall17V2noIso")->setComment("MVA noIso ID V2 score");

  desc.add<edm::FileInPath>("weights_v1")->setComment("json file containing weights for v1");
  desc.add<edm::FileInPath>("weights_v2")->setComment("json file containing weights for v2");

  std::string prodname;
  if (typeid(T) == typeid(pat::Muon))
    prodname += "Mu";
  else if (typeid(T) == typeid(pat::Electron))
    prodname += "Ele";
  prodname += "TopLeptonMVAProducer";

  descriptions.add(prodname, desc);
}

typedef TopLeptonMVAProducer<pat::Electron> EleTopLeptonMVAProducer;
typedef TopLeptonMVAProducer<pat::Muon> MuTopLeptonMVAProducer;

DEFINE_FWK_MODULE(EleTopLeptonMVAProducer);
DEFINE_FWK_MODULE(MuTopLeptonMVAProducer);
