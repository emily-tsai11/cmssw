import matplotlib.pyplot as plt
import matplotlib.patches as phs
import matplotlib.lines as lin
import sys
sys.path.insert(0, "/eos/user/e/etsai/workspace/VertexNtuples_CMSSW_14_1_0_pre3/src/VertexNtuples/VertexNtuplizer/test/cmsstyle/src/cmsstyle")
import cmsstyle as CMS


# taken from MTD TDR
btlRadius = 117.45 # cm
otstz = 265.0 # cm
etl1 = 303.8 # cm
etl2 = 305.7 # cm
# taken from tracker geometry:
otRadius = 112.651 # cm
otz = 267.325 # cm

class Trajectory:
  def __init__(self, refPoint, refPointErr, refWidth, hitsX, hitsXerr, hitsY, hitsYerr, hitsZ, hitsZerr, trkRef, trkTime, trkTimeErr):
    self.refPoint = refPoint # [x, y, z]
    self.refPointErr = refPointErr # [xerr, yer, zerr]
    self.refWidth = refWidth # for beam spot only, 0 for primary vertex
    self.hitsX = hitsX
    self.hitsXerr = hitsXerr
    self.hitsY = hitsY
    self.hitsYerr = hitsYerr
    self.hitsZ = hitsZ
    self.hitsZerr = hitsZerr
    self.trkRef = trkRef
    self.trkTime = trkTime
    self.trkTimeErr = trkTimeErr


def plotTrajectories(trajBS, trajPV, iTrk):
  timeBS = "t=%.4f$\pm$%.4fps" % (trajBS.trkTime*1000, trajBS.trkTimeErr*1000)
  timePV = "t=%.4f$\pm$%.4fps" % (trajPV.trkTime*1000, trajPV.trkTimeErr*1000)

  fmtBS = "v"
  fillBS = "none"
  fmtPV = "^"
  fillPV = "none"
  saveDir = "/eos/home-b/btvweb/www/Offline/Phase2/etsai/Phase2/Trajectory/"

  figXY = plt.figure()
  axXY = figXY.add_subplot()
  btl = phs.Ellipse((0, 0), btlRadius*2, btlRadius*2, fill=False, lw=1.5, ls="dashed", color="k")
  axXY.add_patch(btl)
  ot = phs.Ellipse((0, 0), otRadius*2, otRadius*2, fill=False, lw=1.5, ls="solid", color="k")
  axXY.add_patch(ot)
  bsXY = phs.Ellipse((trajBS.refPoint[0], trajBS.refPoint[1]), trajBS.refWidth[0], trajBS.refWidth[1], label="Beam Spot", fill=False, lw=1.5, color="k")
  axXY.add_patch(bsXY)
  plt.errorbar(trajBS.hitsX, trajBS.hitsY, trajBS.hitsYerr, trajBS.hitsXerr, fmt=fmtBS, fillstyle=fillBS, label="BS extrapolation, "+timeBS, color=CMS.petroff_8[1]) # orange
  plt.errorbar(trajPV.refPoint[0], trajPV.refPoint[1], trajPV.refPointErr[1], trajPV.refPointErr[0], ms=5, fmt="o", label="Primary Vertex", color=CMS.petroff_8[2]) # red
  plt.errorbar(trajPV.hitsX, trajPV.hitsY, trajPV.hitsYerr, trajPV.hitsXerr, fmt=fmtPV, fillstyle=fillPV, label="PV extrapolation, "+timePV, color=CMS.petroff_8[0]) # blue
  plt.arrow(-90, 0, -btlRadius+98, 0, width=1.5)
  plt.text(-88, -2.5, "MTD barrel")
  plt.text(-100, -50, "Outer Tracker barrel")
  plt.legend()
  plt.xlabel("x [cm]")
  plt.ylabel("y [cm]")
  plt.savefig(saveDir + "traj_" + str(iTrk) + "_xy.png")
  plt.savefig(saveDir + "traj_" + str(iTrk) + "_xy.pdf")
  plt.close()

  figXZ = plt.figure()
  axXZ = figXZ.add_subplot()
  bsXZ = phs.Ellipse((trajBS.refPoint[0], trajBS.refPoint[2]), trajBS.refWidth[0], trajBS.refWidth[2], label="Beam Spot", fill=False, lw=1.5, color="k")
  axXZ.add_patch(bsXZ)
  # BTL
  plt.plot([btlRadius, btlRadius], [-otstz, otstz], ls="dashed", lw=1.5, color="k")
  plt.plot([-btlRadius, -btlRadius], [-otstz, otstz], ls="dashed", lw=1.5, color="k")
  # ETL
  plt.plot([btlRadius, -btlRadius], [etl2, etl2], ls="dotted", lw=1.5, color="k")
  plt.plot([btlRadius, -btlRadius], [etl1, etl1], ls="dotted", lw=1.5, color="k")
  plt.plot([btlRadius, -btlRadius], [-etl2, -etl2], ls="dotted", lw=1.5, color="k")
  plt.plot([btlRadius, -btlRadius], [-etl1, -etl1], ls="dotted", lw=1.5, color="k")
  # OT
  plt.plot([otRadius, otRadius], [-otz, otz], ls="solid", lw=1.5, color="k")
  plt.plot([-otRadius, -otRadius], [-otz, otz], ls="solid", lw=1.5, color="k")
  plt.plot([otRadius, -otRadius], [otz, otz], ls="solid", lw=1.5, color="k")
  plt.plot([otRadius, -otRadius], [-otz, -otz], ls="solid", lw=1.5, color="k")
  # TRAJ
  plt.errorbar(trajBS.hitsX, trajBS.hitsZ, trajBS.hitsZerr, trajBS.hitsXerr, fmt=fmtBS, fillstyle=fillBS, label="BS extrapolation, "+timeBS, color=CMS.petroff_8[1])
  plt.errorbar(trajPV.refPoint[0], trajPV.refPoint[2], trajPV.refPointErr[2], trajPV.refPointErr[0], ms=5, fmt="o", label="Primary Vertex", color=CMS.petroff_8[2])
  plt.errorbar(trajPV.hitsX, trajPV.hitsZ, trajPV.hitsZerr, trajPV.hitsXerr, fmt=fmtPV, fillstyle=fillPV, label="PV extrapolation, "+timePV, color=CMS.petroff_8[0])
  plt.arrow(-102, 107, -btlRadius+110, 0, width=1.5)
  plt.text(-100, 100, "outer edge of MTD barrel")
  plt.text(-10, 280, "MTD endcap disks")
  plt.text(-10, -295, "MTD endcap disks")
  plt.text(-10, 243, "outer edge of Outer Tracker")
  plt.legend()
  plt.xlabel("x [cm]")
  plt.ylabel("z [cm]")
  plt.savefig(saveDir + "traj_" + str(iTrk) + "_xz.png")
  plt.savefig(saveDir + "traj_" + str(iTrk) + "_xz.pdf")
  plt.close()

  figYZ = plt.figure()
  axYZ = figYZ.add_subplot()
  bsYZ = phs.Ellipse((trajBS.refPoint[1], trajBS.refPoint[2]), trajBS.refWidth[1], trajBS.refWidth[2], label="Beam Spot", fill=False, lw=1.5, color="k")
  axYZ.add_patch(bsYZ)
  # BTL
  plt.plot([btlRadius, btlRadius], [-otstz, otstz], ls="dashed", lw=1.5, color="k")
  plt.plot([-btlRadius, -btlRadius], [-otstz, otstz], ls="dashed", lw=1.5, color="k")
  # ETL
  plt.plot([btlRadius, -btlRadius], [etl2, etl2], ls="dotted", lw=1.5, color="k")
  plt.plot([btlRadius, -btlRadius], [etl1, etl1], ls="dotted", lw=1.5, color="k")
  plt.plot([btlRadius, -btlRadius], [-etl2, -etl2], ls="dotted", lw=1.5, color="k")
  plt.plot([btlRadius, -btlRadius], [-etl1, -etl1], ls="dotted", lw=1.5, color="k")
  # OT
  plt.plot([otRadius, otRadius], [-otz, otz], ls="solid", lw=1.5, color="k")
  plt.plot([-otRadius, -otRadius], [-otz, otz], ls="solid", lw=1.5, color="k")
  plt.plot([otRadius, -otRadius], [otz, otz], ls="solid", lw=1.5, color="k")
  plt.plot([otRadius, -otRadius], [-otz, -otz], ls="solid", lw=1.5, color="k")
  # TRAJ
  plt.errorbar(trajBS.hitsY, trajBS.hitsZ, trajBS.hitsZerr, trajBS.hitsYerr, fmt=fmtBS, fillstyle=fillBS, label="BS extrapolation, "+timeBS, color=CMS.petroff_8[1])
  plt.errorbar(trajPV.refPoint[1], trajPV.refPoint[2], trajPV.refPointErr[2], trajPV.refPointErr[1], ms=5, fmt="o", label="Primary Vertex", color=CMS.petroff_8[2])
  plt.errorbar(trajPV.hitsY, trajPV.hitsZ, trajPV.hitsZerr, trajPV.hitsYerr, fmt=fmtPV, fillstyle=fillPV, label="PV extrapolation, "+timePV, color=CMS.petroff_8[0])
  plt.arrow(-102, 107, -btlRadius+110, 0, width=1.5)
  plt.text(-100, 100, "outer edge of MTD barrel")
  plt.text(-10, 280, "MTD endcap disks")
  plt.text(-10, -295, "MTD endcap disks")
  plt.text(-10, 243, "outer edge of Outer Tracker")
  plt.legend()
  plt.xlabel("y [cm]")
  plt.ylabel("z [cm]")
  plt.savefig(saveDir + "traj_" + str(iTrk) + "_yz.png")
  plt.savefig(saveDir + "traj_" + str(iTrk) + "_yz.pdf")
  plt.close()


fTraj = open("trajectories.txt", "r")

evtcnt = 0
bsTrajs = {}
pvTrajs = {}
for line in fTraj:
  line = line.strip()

  if line == "TrackExtenderWithMTD":
    evtcnt += 1
    line = fTraj.readline().strip().split()
    bsposx = [float(line[0])]
    bsposxerr = [float(line[1])]
    bsposy = [float(line[2])]
    bsposyerr = [float(line[3])]
    bsposz = [float(line[4])]
    bsposzerr = [float(line[5])]
    line = fTraj.readline().strip().split()
    bswidx = float(line[0])
    # bswidxerr = float(line[1])
    bswidy = float(line[2])
    # bswidyerr = float(line[3])
    bswidz = float(line[4])
    # bswidzerr = float(line[5])
    refPoint = [bsposx, bsposy, bsposz]
    refPointErr = [bsposxerr, bsposyerr, bsposzerr]
    refWidth = [bswidx, bswidy, bswidz]
    continue
  elif line == "TrackExtenderFromPointWithMTD":
    line = fTraj.readline().strip().split()
    pvx = [float(line[0])]
    pvxerr = [float(line[1])]
    pvy = [float(line[2])]
    pvyerr = [float(line[3])]
    pvz = [float(line[4])]
    pvzerr = [float(line[5])]
    refPoint = [pvx, pvy, pvz]
    refPointErr = [pvxerr, pvyerr, pvzerr]
    refWidth = [0.0, 0.0, 0.0]
    continue

  line = line.split()
  if len(line) == 1:
    nHits = int(line[0])
    x = []
    xerr = []
    y = []
    yerr = []
    z = []
    zerr = []
    readAgain = True
    for iHit in range(nHits):
      line = fTraj.readline().strip().split()
      if len(line) == 3:
        readAgain = False
        break
      refType = line[0]
      x.append(float(line[1]))
      xerr.append(float(line[2]))
      y.append(float(line[3]))
      yerr.append(float(line[4]))
      z.append(float(line[5]))
      zerr.append(float(line[6]))
    if readAgain: line = fTraj.readline().strip().split()
    trkRef = line[0]
    trkTime = float(line[1])
    trkTimeErr = float(line[2])
  traj = Trajectory(refPoint, refPointErr, refWidth, x, xerr, y, yerr, z, zerr, trkRef, trkTime, trkTimeErr)
  if refType == "BS": bsTrajs[trkRef+("k%d"%evtcnt)] = traj
  elif refType == "PV": pvTrajs[trkRef+("k%d"%evtcnt)] = traj

numTrks = 25
iTrk = 1
nBadTracks = 0
maxNBadTracks = 10
for bskey in bsTrajs.keys():
  if iTrk > numTrks: break
  if bskey not in pvTrajs.keys():
    # print("No matching track extrapolated to PV!")
    continue
  if bsTrajs[bskey].trkTime == 0.0 and bsTrajs[bskey].trkTimeErr == -1.0:
    # print("No BS extrapolated track time!")
    continue
  if pvTrajs[bskey].trkTime == 0.0 and pvTrajs[bskey].trkTimeErr == -1.0:
    # print("No PV extrapolated track time!")
    continue
  if bsTrajs[bskey].trkTime == pvTrajs[bskey].trkTime:
    print("Times are too similar!")
    continue
  if bsTrajs[bskey].trkTimeErr*1000 > 60.0 or pvTrajs[bskey].trkTimeErr*1000 > 60.0:
    if nBadTracks >= maxNBadTracks:
      # print("Errors are too large!")
      continue
    nBadTracks += 1

  print("Plotting track %d: %s" % (iTrk, bsTrajs[bskey].trkRef))
  plotTrajectories(bsTrajs[bskey], pvTrajs[bskey], iTrk)
  iTrk += 1
