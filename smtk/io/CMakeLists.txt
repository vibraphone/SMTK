# Encode definitions for operator hints as a string for use in OperatorLog.cxx:
smtk_operator_xml("${CMAKE_CURRENT_SOURCE_DIR}/OperatorLogHints.sbt" ioLogHintXML)

# set up sources to build
set(ioSrcs
  AttributeReader.cxx
  AttributeWriter.cxx
  ExportJSON.cxx
  ImportJSON.cxx
  Logger.cxx
  LogToFile.cxx
  OperatorLog.cxx
  ResourceSetReader.cxx
  ResourceSetWriter.cxx
  XmlDocV1Parser.cxx
  XmlDocV2Parser.cxx
  XmlV2StringWriter.cxx
)

set(ioHeaders
  AttributeReader.h
  AttributeWriter.h
  ExportJSON.h
  ImportJSON.h
  Logger.h
  OperatorLog.h
  LogProcessor.h
  LogToFile.h
  ResourceSetReader.h
  ResourceSetWriter.h
  #XmlDocV1Parser.h
  XmlDocV2Parser.h
  #XmlV1StringWriter.h
  XmlV2StringWriter.h
)

# Install the headers:
smtk_public_headers(${ioHeaders})

if (SMTK_ENABLE_TESTING)
  add_subdirectory(testing)
endif()
