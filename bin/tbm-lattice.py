#!/usr/bin/env python
#-------------------------------------------------------------|
#| Copyright (C) 2016 Yuan-Yen Tai, Hongchul Choi,            |
#|                    Jian-Xin Zhu                            |
#|                                                            |
#| This file is distributed under the terms of the BSD        |
#| Berkeley Software Distribution. See the file `LICENSE' in  |
#| the root directory of the present distribution, or         |
#| https://en.wikipedia.org/wiki/BSD_licenses.                |
#|                                                            |
#|-------------------------------------------------------------
import os
import sys
import scipy
import numpy as np

from scipy import spatial

def fmt(inStr, length=15):
	while len(inStr) < length:
		inStr+=" "
	return inStr

def MatToStr(valMat):
	valStr = ""
	for val in np.array(valMat)[0]:
		valStr += str(val)+","
	return valStr[0:-1]

class	Atom:
	def __init__(self, atomStringList):
		self.orbitalProfileIndex = int(atomStringList[0])
		self.atomPos = map(float, atomStringList[1:])

	def atomString(self):
		valStr =  fmt(str(self.orbitalProfileIndex),5)+" "\
				+ fmt(str(self.atomPos[0]))+" "\
				+ fmt(str(self.atomPos[1]))+" "\
				+ fmt(str(self.atomPos[2]))
		return valStr

class   Lattice:
	def __init__(self, _filename):
		self.filename = _filename
		self.basisVector = []
		self.orbitalProfile = []
		self.atomList = []
		self.openLattice(_filename)
		print
		print	"Starting Query ..."
		print

	def openLattice(self, _filename):
		fileReader = open(self.filename)

		header = ""
		flag = ""
		atomPosList = []

		for line in fileReader.readlines():
			spline = line.split()
			if len(spline) > 0 :
				header = spline[0]

				# --------------------
				if		header == "#BasisVector":
					flag = header
					continue
				elif	header == "#OrbitalProfile":
					flag = header
					continue
				elif	header == "#Atoms":
					flag = header
					continue

				# --------------------
				if		flag == "#BasisVector":
					self.basisVector.append(line)
					continue
				elif	flag == "#OrbitalProfile":
					self.orbitalProfile.append(line)
					continue
				elif	flag == "#Atoms":
					atom = Atom(spline)
					self.atomList.append(atom)
					atomPosList.append(atom.atomPos)
					continue

		fileReader.close()

		self.tree3D = spatial.KDTree( np.array(atomPosList) )

	def setOrbitalProfile(self, fromKey, toKey, pos):
		result = self.tree3D.query(np.array([pos]))

		if result[0] < 0.01 :
			if self.atomList[result[1]].orbitalProfileIndex == fromKey:
				print "Query: ",self.atomList[result[1]].atomString(), "changedIndex:", fromKey, "->", toKey
				self.atomList[result[1]].orbitalProfileIndex = toKey


	def save(self):

		f = open(self.filename+".new", 'w')

		f.write("#BasisVector\n")
		for basisVec in self.basisVector:
			f.write(basisVec)
		f.write("\n")

		f.write("#OrbitalProfile\n")
		for orbitalProf in self.orbitalProfile:
			f.write(orbitalProf)
		f.write("\n")

		f.write("#Atoms\n")
		for atom in self.atomList:
			f.write(atom.atomString()+"\n")

		print
		print "New file saved in:\n"+self.filename+".new"
		print
		f.close()


if __name__ == "__main__":

	inputLatticeFileName = "sys.argv[1]"

	content = """#!/usr/bin/env python
#|---------------------------------------------------------------
#| Modify this file to manipulate the orders in following ways:
#| 1. Access the order:
#|         Lat.getOrder( key, pos)
#| 2. Set new value to the order:
#|         Lat.setOrder( key, pos, order)
#| 3. Save the new order:
#|         Lat.save( filename = "" )
#|---------------------------------------------------------------
import imp
import sys
import numpy as np

### Loading Lattice class from the TBM3 bin/ path.
foo = imp.load_source('LO', '"""+sys.argv[0]+"""')
Lat = foo.Lattice("""+inputLatticeFileName+""")

#### Query the correspond order in the given atom position.
#Lat.setOrbitalProfile(2,3,[x,y,z])

#### Save the order in the original input file.
#Lat.save()
"""
	filename = "setLattice.py"
	if len(sys.argv) == 2:
		filename = sys.argv[1]+".py"

	f = open(filename,'w')
	f.write(content)
	f.close()

	os.system("chmod +x "+filename)

