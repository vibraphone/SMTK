/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/
// .NAME qtBaseView - a class that encapsulates the UI of an Attribute
// .SECTION Description

#ifndef __smtk_attribute_qtBaseView_h
#define __smtk_attribute_qtBaseView_h

#include <QObject>
#include "smtk/extension/qt/QtSMTKExports.h"
#include "smtk/PublicPointerDefs.h"
#include <QList>

class qtBaseViewInternals;

namespace smtk
{
  namespace attribute
  {
    class qtUIManager;
    class qtItem;

    class QTSMTK_EXPORT qtBaseView : public QObject
    {
      Q_OBJECT

    public:
      qtBaseView(smtk::view::BasePtr, QWidget* parent, qtUIManager* uiman);
      virtual ~qtBaseView();

      smtk::view::BasePtr getObject();
      QWidget* widget()
      {return this->Widget;}
      QWidget* parentWidget();
      qtUIManager* uiManager();
      virtual void getDefinitions(smtk::attribute::DefinitionPtr attDef,
        QList<smtk::attribute::DefinitionPtr>& defs);
      int fixedLabelWidth();
      bool setFixedLabelWidth(int w);
      bool advanceLevelVisible()
        { return m_advOverlayVisible; }

    signals:
      void modified(smtk::attribute::ItemPtr);

    public slots:
      virtual void updateUI()
      {
      this->updateAttributeData();
      this->updateModelAssociation();
      this->showAdvanceLevelOverlay(m_advOverlayVisible);
      }
      virtual void updateModelAssociation() {;}
      virtual void valueChanged(smtk::attribute::ItemPtr);
      virtual void childrenResized(){;}
      virtual void showAdvanceLevelOverlay(bool val)
      { m_advOverlayVisible = val;}

    protected slots:
      virtual void updateAttributeData() {;}

    protected:
      virtual void createWidget(){;}

      QWidget* Widget;
    private:

      qtBaseViewInternals *Internals;
      bool m_advOverlayVisible;

    }; // class
  }; // namespace attribute
}; // namespace smtk

#endif