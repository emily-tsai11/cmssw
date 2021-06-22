###################################################################
# Get python list containing CMS data files stored in a local     #
# directory. (Assumes no other .root files in that directory).    #
#                                                                 #
#                                                                 #
# From linux (inside CMSSW project area) run with:                #
#                                                                 #
# python -c 'from getCMSlocaldata_cfi import *;                   # 
#            print getCMSlocaldata("dataDirectoryNameXYZ")'       #
#                                                                 #
# where optional second argument defaults to "prod/global"        #
#                                                                 #
# Or from CMSSW myJob_cfg.py use with:                            #
#                                                                 #
# from xyz.abc.getCMSlocaldata_cfi import *                       #
# flist = getCMSdata("datDirectoryNameXYZ")'                      #
#                                                                 #
###################################################################

import os
def getCMSlocaldata( dirName ):
  cmd = '\ls ' + dirName + '/*.root'
  files = os.popen(cmd).read()
  # Create python list containing file names.
  flist = files.split('\n')
  del flist[-1]
  flist2 = ['file:' + f for f in flist]
  return flist2
