import os
import sys
from itertools import islice


#################### CHANGE THESE ####################

# sample = 'TTbar_PU200_D88'
# sample = 'TTbar_noPU_D88'
# sample = 'TRK2026D88PU200MB2-v5'
sample = 'TTbar_D88_PU200_v5'
# sample = 'test'

# Number of sample file(s) processed per condor job
n_file = 1

######################################################


# Don't run the script if previous jobs files still exist
if(os.path.exists(sample)):
    print('clean the directory before running! exiting...')
    exit()

# Make job directories
os.system('mkdir ' + sample)
os.system('mkdir ' + sample + '/root_files')
os.system('mkdir ' + sample + '/status')

# Name of the txt file used to help locate samples
file_list = 'lists/' + sample + '.list'

with open(file_list, 'r') as f:

    # The nth job
    job_counter = 1

    while True:

        # Read in files
        files = list(islice(f, n_file))
        if not files:
            break

        # File names
        f1_name = sample + '/' + sample + '-' + str(job_counter) + '.condor'
        f2_name = sample + '/' + sample + '-' + str(job_counter) + '.sh'

        # Writing to .condor file
        condor = 'universe\t\t\t= vanilla\n'
        condor += 'executable\t\t= ' + f2_name + '\n'
        condor += 'output\t\t\t\t= ' + sample + '/status/' + sample + '-' + str(job_counter) + '.out\n'
        condor += 'log\t\t\t\t\t\t= ' + sample + '/status/' + sample + '-' + str(job_counter) + '.log\n'
        condor += 'error\t\t\t\t\t= ' + sample + '/status/' + sample + '-' + str(job_counter) + '.err\n'
        # condor += '+JobFlavour\t\t= "microcentury"\n' # 1 hour
        # condor += '+JobFlavour\t\t= "longlunch"\n'    # 2 hours
        condor += '+JobFlavour\t\t= "workday"\n'      # 8 hours
        # condor += '+JobFlavour\t\t= "tomorrow"\n'     # 1 day
        condor += 'queue\n'
        f1 = open(f1_name, 'w')
        f1.write(condor)
        f1.close()

        # Create input file list for this job
        input = ''
        for file in files:
            input += '\'' + file.strip() + '\','
        input = input[:-1]

        # Writing .sh file
        executable = '#!/bin/bash\n'
        executable += 'export CMSSW_PROJECT_SRC=/afs/cern.ch/user/e/etsai/workspace/L1TrackingStudies_CMSSW_12_6_0_pre5/src\n'
        executable += 'cd $CMSSW_PROJECT_SRC\n' # Go to the src directly
        executable += 'eval `scramv1 runtime -sh`\n' # This is effectively "cmsenv"
        executable += 'export X509_USER_PROXY=/afs/cern.ch/user/e/etsai/.globus/x509up_u125665\n' # Exporting the proxy
        executable += 'cd /afs/cern.ch/user/e/etsai/workspace/L1TrackingStudies_CMSSW_12_6_0_pre5/src/L1Trigger/TrackFindingTracklet/test\n' # Go the directory where you have the producer and python config file
        executable += 'cmsRun L1TrackNtupleMaker_cfg.py inputFiles=' + input + ' outputFile=condor_files/' + sample + '/root_files/' + sample + '-' + str(job_counter) + '.root\n'
        f2 = open(f2_name, 'w')
        f2.write(executable)
        f2.close()
        os.system('chmod 755 ' + f2_name) # Make .sh file executable

        # Submit the job using condor_submit command
        os.system('condor_submit ' + f1_name)

        # Increment the nth job
        job_counter += 1
