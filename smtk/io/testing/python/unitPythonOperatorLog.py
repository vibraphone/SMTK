#!/usr/bin/python
#=============================================================================
#
#  Copyright (c) Kitware, Inc.
#  All rights reserved.
#  See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notice for more information.
#
#=============================================================================
import sys
import os
import smtk
import smtk.testing
import smtk.log
from smtk.simple import *

class TestPythonOperatorLog(smtk.testing.TestCase):

    def setUp(self):
        StartSMTK(session='cgm')
        if not GetActiveSession():
            self.skipTest('CGM session unavailable')
        GetActiveSession().assignDefaultName()
        import tempfile
        self.tmpdir = tempfile.mkdtemp()

    def recordHint(self, result, group, entry, item, index, target):
        """Create a "previous-result" hint and associate it with an item's value(s).

        where
          result    is an integer index specifying which result contains the value of interest.
          group     names the item of the result containing the value of interest.
          entry     is an integer index into the result group containing the value of interest.
          item      is an smtk attribute item to hold the value of interest
          target    specifies where in the item's vector of values the prior result should be placed.
        """
        recorder = GetActiveOperatorLog()
        hint = recorder.createHint('previous-result', 'context')
        SetVectorValue(hint.findString('item name'), '@assoc')
        SetVectorValue(hint.findInt('item index'), [target,])
        SetVectorValue(hint.findInt('result index'), [result,])
        SetVectorValue(hint.findString('result group'), [group,])
        SetVectorValue(hint.findInt('entries'), [entry,])
        recorder.addHintForItem('@assoc', hint, '/')

    def testSolidModelingOps(self):
        # Create a recorder.
        # It will immediately start logging all operations on the manager
        # (and only those on the specified model manager).
        self.logFileName = os.path.join(self.tmpdir, 'unitPythonOperatorLog.py')
        StartOperatorLogging(filename=self.logFileName)

        # Create a thick spherical shell and a sphere:
        sph = CreateSphere(radius=1.0, center=[0.2, 0.2, 0.2])
        sph2 = CreateSphere(radius=0.5, center=[0.9, 0., 0.])

        # Note that su should have same UUID as sph2:
        self.recordHint(result=-2, group='created', entry=0, item=-1, index=0, target=0)
        self.recordHint(result=-1, group='created', entry=0, item=-1, index=1, target=1)
        su = Union([sph, sph2])

        # Create a cylinder:
        cyl = CreateCylinder(top_radius=1.0)

        # Create a brick:
        b = CreateBrick(width=2, height=3, depth=5)

        # Check that the log is capturing operations:
        self.assertEqual(GetActiveOperatorLog().numberOfRecords(), 10,
            'Expected 10 log records, got {N}'.format(
                N=GetActiveOperatorLog().numberOfRecords()))

        # Stop logging at a place where we can verify models exist:
        StopOperatorLogging()
        self.assertIsNone(GetActiveOperatorLog(), 'Expected log to be closed.')

        # Now close the session, kill the manager and attempt to replay
        GetActiveModelManager().closeSession(GetActiveSession())
        self.assertEqual(
            GetActiveSession().name(),
            'invalid id {uid}'.format(uid=str(GetActiveSession().entity())),
            'Expected invalid session name after closing, got "{s}"'.format(
                s=GetActiveSession().name())
        )

        # Grab the log contents:
        logFile = open(self.logFileName, 'r')
        logLines = logFile.read().split('\n')
        #print logLines

        # Kill the previous model manager:
        StopSMTK()
        self.assertIsNone(GetActiveModelManager(), 'StopSMTK() failed to... stop uhhh... SMTK.')

        # We will test that the log recreates these variables, so
        # set them to some invalid values before replaying the log:
        op5 = None
        res5 = None

        # Start a new model manager **before** replaying the script so
        # that we can start logging. Note that calling StartSMTK() twice
        # will have no effect the second time, so when the script calls it,
        # there are no errors:
        StartSMTK() # Start up the model
        self.log2FileName = os.path.join(self.tmpdir, 'unitPythonOperatorLog2.py')
        StartOperatorLogging(filename=self.log2FileName)
        print 'Executing log:'
        for line in logLines:
          print '    %s' % line
          exec line
        print 'Done replaying log'

        # Now verify that at least the last line (creating a brick)
        # was executed properly.
        self.assertIsNotNone(res5, 'Expected an operator result, got None.')
        created = res5.findModelEntity('created')
        self.assertIsNotNone(created, 'Expected a list of created model entities')
        self.assertEqual(
            created.numberOfValues(), 1,
            'Expected 1 model entity, got {N}'.format(N=created.numberOfValues()))
        brick = smtk.model.Model(created.value(0))
        self.assertTrue(brick.isValid(), 'Expected a valid model entity')
        bvols = brick.cells()
        self.assertEqual(
            len(bvols), 1,
            'Expected a single volume in brick, got {N}.'.format(N=len(bvols)))
        bfacs = smtk.model.Volume(bvols[0]).faces()
        self.assertEqual(
            len(bfacs), 6,
            'Expected a 6-faced brick, got {N} faces.'.format(N=len(bfacs)))

        ## Once we get hints working, we could compare all non-comment lines
        ## in the 2 log files to verify that a "round-trip" is possible (i.e.,
        ## that replaying a log while logging will generate the same log).
        ## In fact, right now the Union operation fails because the UUIDs
        ## do not match and we are not using hints properly:
        # log2File = open(self.log2FileName, 'r')
        # log2Lines = log2File.read().split('\n')
        # self.assertEqual(
        #     len(log2Lines), len(logLines),
        #     'Expected logs to have same number of lines, but {N1} != {N2}'.format(
        #         N1=len(logLines), N2=len(log2Lines)))

    def tearDown(self):
        try:
            import shutil
            shutil.rmtree(self.tmpdir)
        except:
            pass

if __name__ == '__main__':
    smtk.testing.process_arguments()
    smtk.testing.main()
