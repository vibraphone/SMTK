/*=========================================================================

Copyright (c) 1998-2012 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================*/

#include "smtk/attribute/Manager.h"
#include "smtk/attribute/Definition.h"
#include "smtk/attribute/Attribute.h"
#include <iostream>

int main()
{
  int status = 0;
    {
    smtk::attribute::Manager manager;
    std::cout << "Manager Created\n";
    smtk::attribute::DefinitionPtr def = manager.createDefinition("testDef");
    if (def)
      {
      std::cout << "Definition testDef created\n";
      }
    else
      {
      std::cout << "ERROR: Definition testDef not created\n";
      status = -1;
      }

    smtk::attribute::AttributePtr att = manager.createAttribute("testDef");
    if (att)
      {
      std::cout << "Attribute " << att->name() << " created\n";
      }
    else
      {
      std::cout << "ERROR: 1st Attribute not created\n";
      status = -1;
      }

    att = manager.createAttribute("testDef");
    if (att)
      {
      std::cout << "Attribute " << att->name() << " created\n";
      }
    else
      {
      std::cout << "ERROR: 2nd Attribute not created\n";
      status = -1;
      }

    att = manager.createAttribute("testDef");
    if (att)
      {
      std::cout << "Attribute " << att->name() << " created\n";
      }
    else
      {
      std::cout << "ERROR: 3rd Attribute not created\n";
      status = -1;
      }
    std::cout << "Manager destroyed\n";
    }
    return status;
}