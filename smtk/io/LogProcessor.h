#ifndef __smtk_io_LogProcessor_h
#define __smtk_io_LogProcessor_h

#include "smtk/SharedFromThis.h"
#include "smtk/PublicPointerDefs.h"

#include <cstddef> // for std::size_t

namespace smtk {
  namespace io {

class LogProcessor : smtkEnableSharedPtr(LogProcessor)
{
public:
  smtkTypeMacro(LogProcessor);

  // Empty constructor/destructor prevent warning in shiboken-generated code:
  LogProcessor() { }
  virtual ~LogProcessor() { }

  virtual void initialize(OperatorLog* log) = 0;
  virtual void addRecords(OperatorLog* log, std::size_t finalRecordId) = 0;
  virtual void finalize(OperatorLog* log) = 0;
};

  } // namespace io
} // namespace smtk

#endif // __smtk_io_LogProcessor_h
