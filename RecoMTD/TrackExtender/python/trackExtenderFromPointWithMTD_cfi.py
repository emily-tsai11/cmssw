import FWCore.ParameterSet.Config as cms

from RecoMTD.TrackExtender.PropagatorWithMaterialForMTD_cfi import *
from RecoMTD.TrackExtender.trackExtenderFromPointWithMTDBase_cfi import *

trackExtenderFromPointWithMTD = trackExtenderFromPointWithMTDBase.clone()
