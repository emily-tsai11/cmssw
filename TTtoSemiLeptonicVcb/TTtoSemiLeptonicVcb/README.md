# For generating Run 3 TTtoSemiLeptonic V_cb gridpacks

CMSSW is so far only needed for the environment, according to the [CMS generator Wiki](https://cms-gen.gitbook.io/cms-generator-central-place/how-to-produce-gridpacks/powheg-box). The folders are structured in such a way to comply with CMSSW package requirements.

### Setup

Lxplus8 is required.

### Procedure (so far)

Generally, we want to find the current Run 3 Powheg gridpack to get the powheg.input file, 

1. Get the name of the dataset you want to use, e.g. through DAS. Here we will use [this](https://cmsweb.cern.ch/das/request?input=dataset%3D%2FTTtoLNu2Q_TuneCP5_13p6TeV_powheg-pythia8%2FRun3Summer23MiniAODv4-130X_mcRun3_2023_realistic_v14-v2%2FMINIAODSIM&instance=prod/global) sample.
2. Go to McM. Find the output dataset, then click on "View chains containing \<your dataset\>". Click on the "LHEGS" or "LHEGEN" step. Then get the setup command. Download the fragment file from McM. For the example file, the download command is shown.
```
# Download fragment from McM
curl -s -k https://cms-pdmv-prod.web.cern.ch/mcm/public/restapi/requests/get_fragment/GEN-Run3Summer23wmLHEGS-00009 --retry 3 --create-dirs -o Configuration/GenProduction/python/GEN-Run3Summer23wmLHEGS-00009-fragment.py
[ -s Configuration/GenProduction/python/GEN-Run3Summer23wmLHEGS-00009-fragment.py ] || exit $?;
```
3. Open the fragment file to get the name of the gridpack. Then you can copy the gridpack to your local directory.

### Procedure to get the Run 2 gridpack


### Description of files in this directory

| File | Description |
| ---- | ----------- |
| `Configuration/GenProduction/python/GEN-Run3Summer23wmLHEGS-00009-fragment` | |
