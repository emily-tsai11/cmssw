# For generating Run 3 TTtoSemiLeptonic V_cb gridpacks

CMSSW is so far only needed for the environment, according to the [CMS generator Wiki](https://cms-gen.gitbook.io/cms-generator-central-place/how-to-produce-gridpacks/powheg-box). The folders are structured in such a way to comply with CMSSW package requirements.

### Setup

Lxplus8 is required.

### Procedure to get the old gridpack

Generally, we want to find the current Run 3 Powheg gridpack to get the `powheg.input` file. Then, we use genproductions and a new patch file from [here](https://indico.cern.ch/event/1354206/contributions/5704525/attachments/2774287/4834467/Top_MP_INSEOK_231218.pdf) to generate a new (patched) gridpack.

1. Get the name of the dataset you want to use, e.g. through DAS. Here we will use [this](https://cmsweb.cern.ch/das/request?input=dataset%3D%2FTTtoLNu2Q_TuneCP5_13p6TeV_powheg-pythia8%2FRun3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v14-v2%2FMINIAODSIM&instance=prod/global) sample.
2. Go to McM. Find the output dataset, then click on "View chains containing \<the sample\>". Click on the first step (should be `LHEGEN` or `LHEGS`). Then get the setup command. Download the fragment file from McM. For the example file, the download command is shown.
```
# Download fragment from McM
curl -s -k https://cms-pdmv-prod.web.cern.ch/mcm/public/restapi/requests/get_fragment/GEN-Run3Summer23wmLHEGS-00009 --retry 3 --create-dirs -o Configuration/GenProduction/python/GEN-Run3Summer23wmLHEGS-00009-fragment.py
[ -s Configuration/GenProduction/python/GEN-Run3Summer23wmLHEGS-00009-fragment.py ] || exit $?;
```
3. Get the name of the gridpack from the fragment file.
```
/cvmfs/cms.cern.ch/phys_generator/gridpacks/PdmV/Run3Summer22/Powheg/TT/hvq_slc7_amd64_gcc10_CMSSW_12_4_8_TTtoLNu2Q_powheg-pythia8.tgz
```
4. Untar the gridpack to get the `powheg.input` file.

### Procedure to generate a new gridpack (so far)

1. Set up POWHEG-BOX via genproductions as outlined in the CMS Generator central place [here](https://cms-gen.gitbook.io/cms-generator-central-place/how-to-produce-gridpacks/powheg-box). The CMSSW version should be `CMSSW_13_0_13` (if in doubt, check "Get setup command" for the sample). Use the `genproductions` repo here.
```
git clone --single-branch --depth=1 --no-checkout https://github.com/emily-tsai11/genproductions.git -b TTToLNu2Q_Vcb
```
2. Go to `genproductions/bin/Powheg/`.
3. Do the gridpack generation step. To run locally use the following.
```
python3 ./run_pwg_condor.py -p f -i /afs/cern.ch/user/e/etsai/projects/VcbMeasurement/Run3Gridpack_CMSSW_13_0_13/src/TTtoSemiLeptonicVcb/TTtoSemiLeptonicVcb/test/powheg.input -m hvq -f TTtoSemileptonicVcb -n 100
```
To run on condor use the following.
```
python3 ./run_pwg_condor.py -p f -i /afs/cern.ch/user/e/etsai/projects/VcbMeasurement/Run3Gridpack_CMSSW_13_0_13/src/TTtoSemiLeptonicVcb/TTtoSemiLeptonicVcb/test/powheg.input -m hvq -f TTtoSemileptonicVcb -q longlunch -n 100
```
Right now I would suggest running locally, as it is much faster than condor and condor has the following errors.
```
/srv//run_full_TTtoSemileptonicVcb.sh: line 161: patch: command not found
...
/srv//run_full_TTtoSemileptonicVcb.sh: line 390: patch: command not found
```

### Procedure to verify the contents of a gridpack

To generate the config file used to make NANOGEN files from the gridpack, first change the gridpack in the fragment file to the gridpack you want to use. Then do the following `cmsDriver` command.
```
cmsDriver.py Configuration/GenProduction/python/GEN-Run3Summer23wmLHEGS-00009-fragment.py --python_filename GEN-Run3Summer23MiniAODv4-00015_1_cfg.py --eventcontent NANOAODGEN --customise Configuration/DataProcessing/Utils.addMonitoring --datatier NANOAOD --fileout file:GEN-Run3Summer23MiniAODv4-00015.root --conditions 130X_mcRun3_2023_realistic_v14 --step LHE,GEN,NANOGEN --geometry DB:Extended --era Run3_2023 --no_exec --mc -n 100
```
This command was put together from the `cmsDriver` command in the `LHEGS` "Get setup command" and from the NANOGEN [Twiki](https://twiki.cern.ch/twiki/bin/viewauth/CMS/NanoGen) (see "Generation starting with a gridpack").

Then generate events with `cmsRun`.
```
cmsRun GEN-Run3Summer23MiniAODv4-00015_1_cfg.py
```
