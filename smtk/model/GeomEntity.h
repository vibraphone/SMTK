//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_model_GeomEntity_h
#define __smtk_model_GeomEntity_h

#include "smtk/model/EntityRef.h"
#include "smtk/model/EntityRefArrangementOps.h" // For appendAllRelations

namespace smtk {
  namespace model {

class GeomEntity;

/**\brief A entityref subclass with methods specific to geometric point-locus entities.
  *
  * This class represents a point-locus that is algebraically specified.
  * The enumeration may be implicit (i.e., f(x,y,z) = 0) or explicit
  * (i.e., x = x(t), y = y(t), z = z(t)).
  *
  * A point locus by itself has no orientation,
  * however a cell that references one may have an sense relative
  * to the algebraic specification of the locus.
  * For instance, the point locus of a unit sphere may be expressed
  * as the implicit function f(x,y,z) = x*x + y*y + z*z - 1 = 0.
  * The set itself only provides a membership test (e.g., is
  * f(x,y,z) zero?). However, the implicit function value classifies
  * points as inside (f < 0), outside (f > 0), or on (f == 0) the
  * pointset.
  * This is a peculiarity of the algebraic specification used to
  * perform membership tests, not a property we can ascribe to the
  * point locus itself.
  * Not all specifications provide this same test.
  * For instance, a parametrically-specified plane could be
  * written f(u,v) = p + t * u + b * v where p is a point on
  * the plane; t and b are unit-length tangent and binormal
  * vectors; and u and v are real numbers.
  * This definition enumerates points on the plane and only
  * points on the plane; the function returns vectors on the
  * plane not a signed distance.
  * Thus the "inside" and "outside" tests that
  * divide world coordinates into regions to either side of
  * the plane are not of the same form.
  *
  * Note that the implicit sphere example could also have been written
  * g(x,y,z) = -f = 1 - x*x - y*y - z*z = 0.
  * Similarly, the parametric plane could also have been written
  * g(u,v) = p - t * u - b * v = p + t' * u + b' * v.
  * Because neither f nor g are canonical, we cannot assign a positive
  * or negative orientation to either.
  * But a sense can be defined relating the two.
  *
  * So, given a GeomEntity, you must find the corresponding CellEntity
  * of interest and ask it for its sense relative to the point locus'
  * algebraic specification. Use records also have a **geometric** sense
  * that is the composition of their sense with respect to their parent cell
  * and the cell's sense with respect to the point-locus parameterization.
  */
class SMTKCORE_EXPORT GeomEntity : public EntityRef
{
public:
  SMTK_ENTITYREF_CLASS(GeomEntity,EntityRef,isGeomEntity);

  template<typename T> T cells() const;
};

template<typename T>
T GeomEntity::cells() const
{
  T result;
  EntityRefArrangementOps::appendAllRelations(*this, HAS_CELL, result);
  return result;
}

  } // namespace model
} // namespace smtk

#endif // __smtk_model_GeomEntity_h
