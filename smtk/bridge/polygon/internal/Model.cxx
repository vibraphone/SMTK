#include "smtk/bridge/polygon/internal/Model.h"

#include "smtk/model/Session.h"
#include "smtk/model/Manager.h"
#include "smtk/io/Logger.h"

using namespace smtk::model;

namespace smtk {
  namespace bridge {
    namespace polygon {
      namespace internal {

model::model()
  : m_parent(NULL), m_featureSize(1e-8)
{
  for (int i = 0; i < 3; ++i)
    {
    this->m_origin[i] = 0.; // Base point of plane for model
    this->m_xAxis[i] = 0.; // Vector whose length should be equal to one "unit" (e.g., m_scale integers long)
    this->m_yAxis[i] = 0.; // In-plane vector orthogonal to m_xAxis with the same length.
    this->m_zAxis[i] = 0.; // Normal vector orthogonal to m_xAxis and m_yAxis with the same length.
    }
  this->m_xAxis[0] = this->m_yAxis[1] = this->m_zAxis[2] = 1.;
}

model::~model()
{
  this->m_parent = NULL; // better to crash sooner than later.
}

bool model::computeModelScaleAndNormal(
  std::vector<double>& origin,
  std::vector<double>& x_axis,
  std::vector<double>& y_axis,
  double featureSize,
  smtk::io::Logger& log)
{
  if (featureSize <= 0.)
    {
    smtkErrorMacro(log,
      "Feature size must be positive (not " << featureSize << ").");
    return false;
    }
  this->m_featureSize = featureSize;

  if (origin.size() != 3 || x_axis.size() != 3 || y_axis.size() != 3)
    {
    smtkErrorMacro(log,
      "Vector of length 3 expected for"
      << " origin (" << origin.size() << "),"
      << " x axis (" << x_axis.size() << "), and"
      << " y axis (" << y_axis.size() << ").");
    return false;
    }
  double xl2 = 0., yl2 = 0., zl2 = 0.;
  for (int i = 0; i < 3; ++i)
    {
    this->m_origin[i] = origin[i];
    this->m_xAxis[i] = x_axis[i];
    this->m_yAxis[i] = y_axis[i];
    xl2 += x_axis[i] * x_axis[i];
    yl2 += y_axis[i] * y_axis[i];
    this->m_zAxis[i] =
      x_axis[(i+1) % 3] * y_axis[(i+2) % 3] -
      x_axis[(i+2) % 3] * y_axis[(i+1) % 3];
    zl2 += this->m_zAxis[i] * this->m_zAxis[i];
    }
  if (xl2 < 1e-16 || yl2 < 1e-16 || zl2 < 1e-16)
    {
    smtkErrorMacro(log,
      "Vectors of non-zero L2 norm required for "
      << " x (" << xl2 << "),"
      << " y (" << yl2 << "), and"
      << " z (" << zl2 << ") axes.");
    return false;
    }
  xl2 = sqrt(xl2) * featureSize;
  yl2 = sqrt(yl2) * featureSize;
  zl2 = sqrt(zl2) * featureSize;
  // Make the axes one feature-size in length:
  for (int i = 0; i < 3; ++i)
    {
    this->m_xAxis[i] /= xl2;
    this->m_yAxis[i] /= yl2;
    this->m_zAxis[i] /= zl2;
    }
  // Scale each feature size to be 231000 integer quanta.
  this->m_scale = 231000;
  return true;
}

bool model::computeModelScaleAndYAxis(
  std::vector<double>& origin,
  std::vector<double>& x_axis,
  std::vector<double>& z_axis,
  double featureSize,
  smtk::io::Logger& log)
{
  if (featureSize <= 0.)
    {
    smtkErrorMacro(log,
      "Feature size must be positive (not " << featureSize << ").");
    return false;
    }
  this->m_featureSize = featureSize;

  if (origin.size() != 3 || x_axis.size() != 3 || z_axis.size() != 3)
    {
    smtkErrorMacro(log,
      "Vector of length 3 expected for"
      << " origin (" << origin.size() << "),"
      << " x axis (" << x_axis.size() << "), and"
      << " z axis (" << z_axis.size() << ").");
    return false;
    }
  double xl2 = 0., yl2 = 0., zl2 = 0.;
  for (int i = 0; i < 3; ++i)
    {
    this->m_origin[i] = origin[i];
    this->m_xAxis[i] = x_axis[i];
    this->m_zAxis[i] = z_axis[i];
    xl2 += x_axis[i] * x_axis[i];
    zl2 += z_axis[i] * z_axis[i];
    this->m_yAxis[i] =
      z_axis[(i+1) % 3] * x_axis[(i+2) % 3] -
      z_axis[(i+2) % 3] * x_axis[(i+1) % 3];
    yl2 += this->m_yAxis[i] * this->m_yAxis[i];
    }
  if (xl2 < 1e-16 || yl2 < 1e-16 || zl2 < 1e-16)
    {
    smtkErrorMacro(log,
      "Vectors of non-zero L2 norm required for "
      << " x (" << xl2 << "),"
      << " y (" << yl2 << "), and"
      << " z (" << zl2 << ") axes.");
    return false;
    }
  xl2 = sqrt(xl2) * featureSize;
  yl2 = sqrt(yl2) * featureSize;
  zl2 = sqrt(zl2) * featureSize;
  // Make the axes one feature-size in length:
  for (int i = 0; i < 3; ++i)
    {
    this->m_xAxis[i] /= xl2;
    this->m_yAxis[i] /= yl2;
    this->m_zAxis[i] /= zl2;
    }
  // Scale each feature size to be 231000 integer quanta.
  this->m_scale = 231000;
  return true;
}

bool model::computeFeatureSizeAndNormal(
  std::vector<double>& origin,
  std::vector<double>& x_axis,
  std::vector<double>& y_axis,
  long long modelScale,
  smtk::io::Logger& log)
{
  if (origin.size() != 3 || x_axis.size() != 3 || y_axis.size() != 3)
    {
    smtkErrorMacro(log,
      "Vector of length 3 expected for"
      << " origin (" << origin.size() << "),"
      << " x axis (" << x_axis.size() << "), and"
      << " y axis (" << y_axis.size() << ").");
    return false;
    }
  double xl2 = 0., yl2 = 0., zl2 = 0.;
  for (int i = 0; i < 3; ++i)
    {
    this->m_origin[i] = origin[i];
    this->m_xAxis[i] = x_axis[i];
    this->m_yAxis[i] = y_axis[i];
    xl2 += x_axis[i] * x_axis[i];
    yl2 += y_axis[i] * y_axis[i];
    this->m_zAxis[i] =
      x_axis[(i+1) % 3] * y_axis[(i+2) % 3] -
      x_axis[(i+2) % 3] * y_axis[(i+1) % 3];
    zl2 += this->m_zAxis[i] * this->m_zAxis[i];
    }
  if (xl2 < 1e-16 || yl2 < 1e-16 || zl2 < 1e-16)
    {
    smtkErrorMacro(log,
      "Vectors of non-zero L2 norm required for "
      << " x (" << xl2 << "),"
      << " y (" << yl2 << "), and"
      << " z (" << zl2 << ") axes.");
    return false;
    }
  xl2 = sqrt(xl2);
  yl2 = sqrt(yl2);
  zl2 = sqrt(zl2);
  for (int i = 0; i < 3; ++i)
    {
    this->m_xAxis[i] /= xl2;
    this->m_yAxis[i] /= yl2;
    this->m_zAxis[i] /= zl2;
    }
  this->m_scale = modelScale;
  this->m_featureSize = 1.0;
  return true;
}

      } // namespace internal
    } // namespace polygon
  } // namespace bridge
} // namespace smtk