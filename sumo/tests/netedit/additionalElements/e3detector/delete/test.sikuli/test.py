# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, False)

# apply zoom
netedit.zoomIn(match.getTarget().offset(325, 200), 10)

# select E3
netedit.changeAdditional("e3Detector")

# create E3 1
netedit.leftClick(match, 100, 100)

# create E3 2
netedit.leftClick(match, 300, 100)

# select entry detector
netedit.changeAdditional("detEntry")

# Create Entry detectors for E3 2
netedit.selectAdditionalChild(4, 3)
netedit.leftClick(match, 150, 250)

# Change to delete
netedit.deleteMode()

# delete created E3 1
netedit.leftClick(match, 100, 100)

# delete created E3 2
netedit.leftClick(match, 300, 100)

# delete loaded E3 1
netedit.leftClick(match, 400, 100)

# delete loaded E3 2
netedit.leftClick(match, 500, 100)

# Check undo redo
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess, False, False)