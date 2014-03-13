#include "smtk/model/Parameter.h"

namespace smtk {
  namespace model {

Parameter::Parameter()
  : m_state(PARAMETER_UNKNOWN), m_name("unnamed")
{
}

Parameter::Parameter(const std::string& name)
  : m_state(PARAMETER_UNKNOWN), m_name(name)
{
}

Parameter::Parameter(const std::string& name, const Float& val)
  : m_state(PARAMETER_UNKNOWN), m_name(name), m_floatVals(1, val)
{
}

Parameter::Parameter(const std::string& name, const FloatList& val)
  : m_state(PARAMETER_UNKNOWN), m_name(name), m_floatVals(val)
{
}

Parameter::Parameter(const std::string& name, const String& val)
  : m_state(PARAMETER_UNKNOWN), m_name(name), m_stringVals(1, val)
{
}

Parameter::Parameter(const std::string& name, const StringList& val)
  : m_state(PARAMETER_UNKNOWN), m_name(name), m_stringVals(val)
{
}

Parameter::Parameter(const std::string& name, const Integer& val)
  : m_state(PARAMETER_UNKNOWN), m_name(name), m_integerVals(1, val)
{
}

Parameter::Parameter(const std::string& name, const IntegerList& val)
  : m_state(PARAMETER_UNKNOWN), m_name(name), m_integerVals(val)
{
}

/// Return the name of the parameter.
std::string Parameter::name() const
{
  return this->m_name;
}

/**\brief Returns whether an Operator has validated the parameter value.
  *
  * The default value is PARAMETER_UNKNOWN.
  * The state is reset to the default whenever the parameter value is changed.
  */
ParameterValidState Parameter::validState() const
{
  return this->m_state;
}

void Parameter::setFloatValue(const Float& fval)
{
  this->m_floatVals.clear();
  this->m_floatVals.push_back(fval);
  this->setValidState(PARAMETER_UNKNOWN);
}

void Parameter::setFloatValue(const FloatList& fval)
{
  this->m_floatVals = fval;
  this->setValidState(PARAMETER_UNKNOWN);
}

void Parameter::setStringValue(const String& sval)
{
  this->m_stringVals.clear();
  this->m_stringVals.push_back(sval);
  this->setValidState(PARAMETER_UNKNOWN);
}

void Parameter::setStringValue(const StringList& sval)
{
  this->m_stringVals = sval;
  this->setValidState(PARAMETER_UNKNOWN);
}

void Parameter::setIntegerValue(const Integer& ival)
{
  this->m_integerVals.clear();
  this->m_integerVals.push_back(ival);
  this->setValidState(PARAMETER_UNKNOWN);
}

void Parameter::setIntegerValue(const IntegerList& ival)
{
  this->m_integerVals = ival;
  this->setValidState(PARAMETER_UNKNOWN);
}

bool Parameter::operator < (const Parameter& other) const
{
  return this->name() < other.name();
}

void Parameter::setValidState(ParameterValidState s)
{
  this->m_state = s;
}

  } // model namespace
} // smtk namespace
