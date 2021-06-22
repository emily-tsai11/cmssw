#######################################################################
# Make _cfi.py files, each corresponding to one of the datasets       #
# provided in the python 'list' dataList.                             #
# They are written to directory dirName (which need not exist)     	  #
#                                                                     #
# This script should be called from another python script, such as    #
#                                                                     #
# N.B. You must have a valid GRID proxy to use this script!           #
#######################################################################

from getCMSdata_cfi import *
import os
import sys
import shutil

def makeDataCfiFiles(dirName, dataList):

  # Delete output directory, if already existed from previous run.
  if (os.path.exists(dirName)):
    shutil.rmtree(dirName)
  if (not os.path.exists(dirName)):
    os.makedirs(dirName)

  for data in dataList:

      # Get list of .root files in this dataset.
      flist = getCMSdata(data)

      # Create subdirectory according to pile-up
      words = data.split('/')
      if (words[2].find("noPU") >= 0):
          PU = "PU0"
      elif (words[2].find("PU140") >= 0):
          PU = "PU140"
      elif (words[2].find("PU200") >= 0):
          PU = "PU200"
      else:
          print "ERROR: Unknown pileup " + data
          exit(1)

      # Produce output _cfi.py file
      typeMC = words[1].replace('RelVal','')
      fileName = dirName + PU + "_" + typeMC + '_cfi.py'

      # Python can't print list directly to file, so do this ...
      stdout_store = sys.stdout
      sys.stdout = open(fileName, "w")
      # Create function returning output list
      print "def getCMSdataFromCards():"
      print "   return ",flist
      sys.stdout = stdout_store
      print fileName + " created"
