//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//=============================================================================


#ifndef __smtk_mesh_moab_Types_h
#define __smtk_mesh_moab_Types_h

#ifndef _MSC_VER
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wc++11-long-long"
#endif
//these require us to install moab headers, so lets fix that
#include "moab/EntityHandle.hpp"
#include "moab/Range.hpp"
#ifndef _MSC_VER
#  pragma GCC diagnostic pop
#endif

namespace smtk {
namespace mesh {
namespace moab {

typedef ::moab::EntityHandle  Handle;
typedef ::moab::Range         HandleRange;

}
}
}

#endif
