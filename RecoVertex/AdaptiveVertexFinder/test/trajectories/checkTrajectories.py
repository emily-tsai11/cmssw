import matplotlib.pyplot as plt
import matplotlib.patches as phs


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

  figXY = plt.figure()
  axXY = figXY.add_subplot()
  bsXY = phs.Ellipse((trajBS.refPoint[0], trajBS.refPoint[1]), trajBS.refWidth[0], trajBS.refWidth[1], label="Beam Spot", fill=False, lw=1.5)
  axXY.add_patch(bsXY)
  plt.errorbar(trajBS.hitsX, trajBS.hitsY, trajBS.hitsYerr, trajBS.hitsXerr, fmt=fmtBS, fillstyle=fillBS, label="BS extrapolation, "+timeBS)
  plt.errorbar(trajPV.refPoint[0], trajPV.refPoint[1], trajPV.refPointErr[1], trajPV.refPointErr[0], ms=5, fmt="o", label="Primary Vertex")
  plt.errorbar(trajPV.hitsX, trajPV.hitsY, trajPV.hitsYerr, trajPV.hitsXerr, fmt=fmtPV, fillstyle=fillPV, label="PV extrapolation, "+timePV)
  plt.legend()
  plt.xlabel("x [cm]")
  plt.ylabel("y [cm]")
  plt.savefig("traj_xy_" + str(iTrk) + ".pdf")
  plt.close()

  figXZ = plt.figure()
  axXZ = figXZ.add_subplot()
  bsXZ = phs.Ellipse((trajBS.refPoint[0], trajBS.refPoint[2]), trajBS.refWidth[0], trajBS.refWidth[2], label="Beam Spot", fill=False, lw=1.5)
  axXZ.add_patch(bsXZ)
  plt.errorbar(trajBS.hitsX, trajBS.hitsZ, trajBS.hitsZerr, trajBS.hitsXerr, fmt=fmtBS, fillstyle=fillBS, label="BS extrapolation, "+timeBS)
  plt.errorbar(trajPV.refPoint[0], trajPV.refPoint[2], trajPV.refPointErr[2], trajPV.refPointErr[0], ms=5, fmt="o", label="Primary Vertex")
  plt.errorbar(trajPV.hitsX, trajPV.hitsZ, trajPV.hitsZerr, trajPV.hitsXerr, fmt=fmtPV, fillstyle=fillPV, label="PV extrapolation, "+timePV)
  plt.legend()
  plt.xlabel("x [cm]")
  plt.ylabel("z [cm]")
  plt.savefig("traj_xz_" + str(iTrk) + ".pdf")
  plt.close()

  figYZ = plt.figure()
  axYZ = figYZ.add_subplot()
  bsYZ = phs.Ellipse((trajBS.refPoint[1], trajBS.refPoint[2]), trajBS.refWidth[1], trajBS.refWidth[2], label="Beam Spot", fill=False, lw=1.5)
  axYZ.add_patch(bsYZ)
  plt.errorbar(trajBS.hitsY, trajBS.hitsZ, trajBS.hitsZerr, trajBS.hitsYerr, fmt=fmtBS, fillstyle=fillBS, label="BS extrapolation, "+timeBS)
  plt.errorbar(trajPV.refPoint[1], trajPV.refPoint[2], trajPV.refPointErr[2], trajPV.refPointErr[1], ms=5, fmt="o", label="Primary Vertex")
  plt.errorbar(trajPV.hitsY, trajPV.hitsZ, trajPV.hitsZerr, trajPV.hitsYerr, fmt=fmtPV, fillstyle=fillPV, label="PV extrapolation, "+timePV)
  plt.legend()
  plt.xlabel("y [cm]")
  plt.ylabel("z [cm]")
  plt.savefig("traj_yz_" + str(iTrk) + ".pdf")
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
for bskey in bsTrajs.keys():
  if iTrk > numTrks: break
  if bskey not in pvTrajs.keys():
    # print("No matching track extrapolated to PV!")
    continue
  if bsTrajs[bskey].trkTime == 0.0:
    # print("No BS extrapolated track time!")
    continue
  if pvTrajs[bskey].trkTime == 0.0:
    # print("No PV extrapolated track time!")
    continue
  if bsTrajs[bskey].trkTime == pvTrajs[bskey].trkTime:
    print("Times are too similar!")
    continue

  print("Plotting track %d: %s" % (iTrk, bsTrajs[bskey].trkRef))
  plotTrajectories(bsTrajs[bskey], pvTrajs[bskey], iTrk)
  iTrk += 1
