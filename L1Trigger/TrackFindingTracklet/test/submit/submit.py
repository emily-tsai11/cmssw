import os
import sys
from itertools import islice

submit_name = 'ZMM_PU200_D76'

# Number of sample file(s) processed per condor job
n_file = 1

# Number of events per condor job
n_events = -1

# Name of the txt file used to help locate samples
file_list = 'list/RelValZMM.list'

with open(file_list, 'r') as f:

	# The nth job
    job_counter = 1

    while True:

		# Read in files
		files = list(islice(f, n_file))
		if not files:
			break

		# Make job directory
		os.system('mkdir job' + str(job_counter))

		# Writing to .condor file
		condor = 'universe\t\t= vanilla\n'
		condor += 'arguments\t\t= "$(argument)"\n'
		condor += 'output\t\t\t= job' + str(job_counter) + '/' + submit_name + '.out\n'
		condor += 'log\t\t\t\t= job' + str(job_counter) + '/' + submit_name + '.log\n'
		condor += 'error\t\t\t= job' + str(job_counter) + '/' + submit_name + '.err\n'
		condor += '+JobFlavour\t\t= "tomorrow"\n'
		condor += 'queue\n'
		f1_name = 'job' + str(job_counter) + '/' + submit_name + '.condor'
		f1 = open(f1_name, 'w')
		f1.write(condor)
		f1.close()

		# Create input file list for this job
		input = ''
		for file in files:
			input += '\'' + file.strip() + '\','
		input = input[:-1]

		# Writing .sh file
		executable = '#!/bin/bash\n\n'
		executable += 'export CMSSW_PROJECT_SRC=/afs/cern.ch/user/e/etsai/workspace/CMSSW_11_3_0_pre3/src\n'
		executable += 'cd $CMSSW_PROJECT_SRC\n' # Go to the src directly
		executable += 'eval `scramv1 runtime -sh`\n' # This is effectively "cmsenv"
		executable += 'export X509_USER_PROXY=/afs/cern.ch/user/e/etsai/.globus/x509up_u125665\n' # Exporting the proxy
		executable += 'cd /afs/cern.ch/user/e/etsai/workspace/CMSSW_11_3_0_pre3/src/L1Trigger/TrackFindingTracklet/test\n' # Go the directory where you have the producer and python config file
		executable += 'cmsRun L1TrackNtupleMaker_cfg.py inputFiles=' + input + ' outputFile=output/' + submit_name + '_job' + str(job_counter) + '.root maxEvents=' + str(n_events) + '\n'
		f2_name = 'job' + str(job_counter) + '/' + submit_name + '.sh'
		f2 = open(f2_name, 'w')
		f2.write(executable)
		f2.close()
		os.system('chmod 755 ' + f2_name) # Make .sh file executable

		# Submit the job using condor_submit command
		os.system('condor_submit ' + f1_name + ' executable=' + f2_name)

		# Increment the nth job
		job_counter += 1
