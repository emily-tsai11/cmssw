import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.globalVariablesTableProducer_cfi import globalVariablesTableProducer

from PhysicsTools.NanoAOD.jetsMC_cff import genJetTable

##################### User floats producers, selectors ##########################

from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenBHadron
matchGenBHadron = matchGenBHadron.clone(
    genParticles = cms.InputTag("prunedGenParticles"),
    jetFlavourInfos = cms.InputTag("slimmedGenJetsFlavourInfos"),
)

from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenCHadron
matchGenCHadron = matchGenCHadron.clone(
    genParticles = cms.InputTag("prunedGenParticles"),
    jetFlavourInfos = cms.InputTag("slimmedGenJetsFlavourInfos"),
)

## Producer for ttbar categorisation ID
from TopQuarkAnalysis.TopTools.categorizeGenTtbar_cfi import categorizeGenTtbar
categorizeGenTtbar = categorizeGenTtbar.clone(
    genJetPtMin = 20.,
    genJetAbsEtaMax = 2.4,
    genJets = cms.InputTag("slimmedGenJets"),
)


##################### Tables for final output and docs ##########################
ttbarCategoryTable = globalVariablesTableProducer.clone(
    variables = cms.PSet(
        genTtbarId = ExtVar( cms.InputTag("categorizeGenTtbar:genTtbarId"), "int", doc = "ttbar categorization")
    )
)

ttbarBHadronOriginTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
    src = genJetTable.src,
    cut = genJetTable.cut,
    name = genJetTable.name,
    variables = cms.PSet(),
    singleton = cms.bool(False),
    extension = cms.bool(True),
    externalVariables = cms.PSet(
        nBHadFromT = ExtVar(cms.InputTag("categorizeGenTtbar:nBHadFromT"), int, doc="number of matched B hadrons with a top quark in their ancestry"),
        nBHadFromTbar = ExtVar(cms.InputTag("categorizeGenTtbar:nBHadFromTbar"), int, doc="number of matched B hadrons with an anti-top quark in their ancestry"),
        nBHadFromW = ExtVar(cms.InputTag("categorizeGenTtbar:nBHadFromW"), int, doc="number of matched B hadrons with a W boson in their ancestry"),
        nBHadOther = ExtVar(cms.InputTag("categorizeGenTtbar:nBHadOther"), int, doc="number of matched B hadrons with no W boson or top quark in their ancestry"),
        nCHadFromW = ExtVar(cms.InputTag("categorizeGenTtbar:nCHadFromW"), int, doc="number of matched prompt (no intermediate B) C hadrons with a W boson in their ancestry"),
        nCHadOther = ExtVar(cms.InputTag("categorizeGenTtbar:nCHadOther"), int, doc="number of matched prompt (no intermediate B) C hadrons with no W boson or top quark in their ancestry"),
    ),
)

ttbarCategoryTableTask = cms.Task(ttbarCategoryTable)
ttbarCatMCProducersTask = cms.Task(matchGenBHadron,matchGenCHadron,categorizeGenTtbar,ttbarBHadronOriginTable)
