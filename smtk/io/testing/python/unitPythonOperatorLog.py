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
        self.mgr = smtk.model.Manager.create()
        sref = self.mgr.createSession('cgm')
        if not sref.isValid():
            self.skipTest('CGM session unavailable')
        sref.assignDefaultName()
        SetActiveSession(sref)
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
        hint = self.recorder.createHint('previous-result', 'context')
        SetVectorValue(hint.findString('item name'), '@assoc')
        SetVectorValue(hint.findInt('item index'), [target,])
        SetVectorValue(hint.findInt('result index'), [result,])
        SetVectorValue(hint.findString('result group'), [group,])
        SetVectorValue(hint.findInt('entries'), [entry,])
        self.recorder.addHintForItem('@assoc', hint, '/')

    def testSolidModelingOps(self):
        # Create a recorder.
        # It will immediately start logging all operations on the manager
        # (and only those on the specified model manager).
        self.recorder = smtk.log.PythonOperatorLog(self.mgr)
        self.logToFile = smtk.io.LogToFile.create()
        self.logToFile.setFilename(os.path.join(self.tmpdir, 'XXX.py'))
        self.recorder.addLogProcessor(self.logToFile)

        # Create a thick spherical shell and a sphere:
        sph = CreateSphere(radius=1.0, center=[0.2, 0.2, 0.2])
        sph2 = CreateSphere(radius=0.5, center=[0.9, 0., 0.])

        # Note that su should have same UUID as sph2:
        self.recordHint(result=-2, group='created', entry=0, item=-1, index=0, target=0)
        self.recordHint(result=-1, group='created', entry=0, item=-1, index=1, target=1)
        su = Union([sph, sph2])

        # Create a cylinder:
        cyl = CreateCylinder(top_radius=1.0)

        # Create a brick
        b = CreateBrick(width=2, height=3, depth=5)

        # Now close the session, kill the manager and attempt to replay
        self.mgr.closeSession(GetActiveSession())
        self.assertEqual(
            GetActiveSession().name(),
            'invalid id {uid}'.format(uid=str(GetActiveSession().entity())),
            'Expected invalid session name after closing, got "{s}"'.format(
                s=GetActiveSession().name()))

        ## Print what we recorded:
        ## First, the preamble.
        #print self.recorder.preamble()
        #print ''

        ## Second, the list of operations.
        #history = self.recorder.records()
        #for op in history:
        #  print '# Operator {op}'.format(op=op['name'])
        #  print '\n'.join(op['statements'])
        #  print '\n# outcome {oc}'.format(oc=op['outcome'])
        #  print '# created ', '\n# created '.join([str(x) for x in op['created']])
        #  print '# expunged ', '\n# expunged '.join([str(x) for x in op['expunged']])
        #  print '# modified ', '\n# modified '.join([str(x) for x in op['modified']])
        #  print ''

        ## Create a new manager
        ##self.mgr = smtk.model.Manager.create()
        ##sref = self.mgr.createSession('cgm')
        ##sref.setName('Replay')
        ##SetActiveSession(sref)

    def tearDown(self):
        try:
            import shutil
            shutil.rmtree(self.tmpdir)
        except:
            pass

if __name__ == '__main__':
    smtk.testing.process_arguments()
    smtk.testing.main()
