Scripts & card files providing easy access to MC samples.
==========================================================

HOW TO GET THE SCRIPTS & CARD FILES
-----------------------------------------

a) Download the scripts by going to your top-level CMSSW_xyz/src/ directory, then:

mkdir MCsamples
cd MCsamples
curl -k -o MC.tar https://cernbox.cern.ch/index.php/s/h3ycr4nrJT4JKPM/download
tar -xvf MC.tar

(The scripts are hosted at lxplus:/eos/user/t/tomalin/L1TrackAlgo/MCsamples/)

b) Create the _cfi.py card files by:

cmsenv
(Ensure you have a valid grid proxy: voms-proxy-init -rfc --voms cms --valid 192:00)
cd Scripts/python/
# Create cards in MCsamples/RelVal_*/ for CMSSW_11_*_0 
python getMC_11_2_0.py 
python getMC_11_1_0.py 

(If you want other datasets, just copy getMC_11_2_0.py & edit its dataset list).

HOW TO ACCESS MC FROM YOUR CMSSW JOB _cfg.py FILE
-------------------------------------------------------

Use one of the following three options in your _cfg.py file:

a) Read MC dataset from a specified card file:

from MCsamples.RelVal_1120.PU200_TTbar_14TeV_cfi import *
inputMC = getCMSdataFromCards()

b) Read all .root files from a directory on your local computer:

from MCsamples.Scripts.getCMSlocaldata_cfi import *
dirName = "${HOME}/myDirectory/" 
inputMC=getCMSlocaldata(dirName)

c) Read MC dataset using CMS DB instead of card file.

N.B. Only use this method occassionally, as it overworks the DB.

from MCsamples.Scripts.getCMSdata_cfi import *
dataName="/RelValTTbar_14TeV/CMSSW_11_2_0_pre5-PU25ns_110X_mcRun4_realistic_v3_2026D49PU200-v1/GEN-SIM-DIGI-RAW"
inputMC=getCMSdata(dataName)
