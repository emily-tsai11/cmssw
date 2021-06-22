#######################################################################
# Get python list containing CMS files corresponding to a dataset     #
#                                                                     #
# From linux (inside CMSSW project area) run with:                    #
#                                                                     #
# python -c 'from getCMSdata_cfi import *;                            # 
#            print getCMSdata("/NoBPTX/blahblah/USER","prod/phys03")' #
#                                                                     #
# where optional second argument defaults to "prod/global"            #
#                                                                     #
# Or from CMSSW myJob_cfg.py use with:                                #
#                                                                     #
# from xyz.abc.getCMSdata_cfi import *                                # 
# flist = getCMSdata("/NoBPTX/blahblah/USER","prod/phys03")'          #
#                                                                     #
# N.B. You must have a valid GRID proxy to use this script!           #
#######################################################################

import os
def getCMSdata( data, dbs="prod/global" ):
  # Read DAS database.
  cmd = 'dasgoclient --query="file dataset=DATA instance=DBS" | sort'
  cmd2 = cmd.replace('DATA',data).replace('DBS',dbs)
  files = os.popen(cmd2).read()
  # Create python list containing file names.
  flist = files.split('\n')
  del flist[-1]
  return flist
