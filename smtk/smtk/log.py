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
import smtk
import re

class PythonOperatorLog(smtk.io.OperatorLog):
    """An operator log that generates a Python script to reproduce operations.
    """
    def __init__(self, mgr):
        super(PythonOperatorLog, self).__init__(mgr)
        self.history = []  # Array of operations. Each array contains an array of statements for reproduction.
        self.active = []   # Stack of operators that have been invoked but not provided return values yet.
        self.setup = [] # Array of preamble statements; initially populated with sessions on manager.
        self.opnum = 0
        #self.var_regex = re.compile('((?<=[a-z0-9])[A-Z]|(?!^)[A-Z](?=[a-z]))')
        self.session_map = {}
        self.setup += [
            'from uuid import UUID',
            'import smtk',
            'from smtk.simple import *',
            'mgr = smtk.model.Manager.create()',
            ]
        for sess in mgr.sessions():
          varname = self.varNameFromLabel(sess.name())
          self.setup += [
              '{svar} = mgr.createSession(\'{stype}\')'.format(
                  svar=varname,
                  stype=sess.session().name().encode('string-escape')),
              '{svar}.setName(\'{sname}\')'.format(
                  svar=varname,
                  sname=sess.name().encode('string-escape'))
              ]
          self.session_map[sess] = varname

    def varNameFromLabel(self, label):
        import string
        compacted = string.capwords(label)
        compacted = re.sub('[^0-9a-zA-Z_]', '', compacted)
        compacted = re.sub('^[^a-zA-Z_]+', '', compacted)
        compacted = compacted[0].lower() + compacted[1:]
        #return self.var_regex.sub(r'_\1', compacted).lower()
        return compacted.encode('string-escape')

    def attribItemDiff(self, itm, ivar, depth = 0):
        """Return statements to set an item to match the non-default values of the given item."""
        stmt = []
        if not itm.isUsingDefault():
            stmt.append('SetVectorValue({ivar}, {val})'.format(
                ivar=ivar, val=str([itm.value(i) for i in range(itm.numberOfValues())])))
        if hasattr(itm, 'childrenItems'):
            itmVarName = 'item{dp}'.format(dp=depth)
            cstmt = []
            for citm in [smtk.attribute.to_concrete(y) for x, y in itm.childrenItems().iteritems()]:
                tmp = ['{ivar} = smtk.attribute.to_concrete({parent}.chilrenItems[\'{iname}\'])'.format(
                    ivar=itmVarName, parent=ivar, iname=citm.name())]
                tmp += self.attribItemDiff(citm, itmVarName, depth + 1)
                if len(tmp) > 1:
                  cstmt += tmp
            if len(cstmt) > 1:
              stmt += cstmt
        return stmt

    def attribDiff(self, a1, varName):
        """Return statements to set an attribute to match the non-default values of the given attribute."""
        #sys = a1.system()
        istmt = []
        for itm in [smtk.attribute.to_concrete(a1.item(i)) for i in range(a1.numberOfItems())]:
            tmp = [
                'item0 = smtk.attribute.to_concrete({var}.find(\'{iname}\'))'.format(
                    var=varName, iname=itm.name()),]
            tmp += self.attribItemDiff(itm, 'item0', 1)
            if len(tmp) > 1:
              istmt += tmp
        return istmt

    def opDiff(self, op):
        """Return statements to create a duplicate operator to match the given one."""
        spec = op.specification()
        self.opnum += 1
        specDefType = spec.definition().type()
        opname = 'op{num}'.format(num=self.opnum)
        prestmt = ['{opname} = {sessvar}.op(\'{dtype}\')'.format(
            sessvar=self.session_map[op.session().ref()],
            opname=opname,
            dtype=specDefType),]
        poststmt = ['res = {opname}.operate()'.format(
          opname=opname),]
        return prestmt + self.attribDiff(spec, opname + '.specification()') + poststmt

    def recordInvocation(self, evt, op):
        """Called when an operator is invoked."""
        self.active.append([op.name(),op.session().ref(),self.opDiff(op)])
        print 'Record Invocation of {nm}!'.format(nm=op.name())
        print self.active[-1]
        return 0

    def recordResult(self, evt, op, res):
        """Called when an operator provides its results."""
        print 'Record Result ({n})'.format(n=len(self.history)+1)
        outcome = smtk.model.OperatorOutcome(
            res.findInt('outcome').value(0))
        if len(self.active) and self.active[-1][0] == op.name():
            self.history.append({
                'name':self.active[-1][0],
                'session':self.active[-1][1],
                'statements':self.active[-1][2]})# + [outcome,])
            self.active = self.active[:-1]
        else:
            self.history.append({'name':self.active[-1][0], 'statements':'***ERROR***'})
        return 0

    def preamble(self):
      """Return statements that prepare the interpreter."""
      return self.setup

    def records(self):
        """Return the statements reproducing the history so far."""
        return self.history
