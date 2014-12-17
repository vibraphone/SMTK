#Build the actual code
add_subdirectory(common)
smtk_source_group(common)

add_subdirectory(attribute)
smtk_source_group(attribute)

add_subdirectory(model)
smtk_source_group(model)

add_subdirectory(simulation)
smtk_source_group(simulation)

add_subdirectory(io)
smtk_source_group(io)

add_subdirectory(view)
smtk_source_group(view)

set(smtk_headers ${commonHeaders} ${attributeHeaders} ${modelHeaders} ${simulationHeaders} ${ioHeaders} ${viewHeaders})
set(smtk_srcs ${commonSrcs} ${attributeSrcs} ${modelSrcs} ${simulationSrcs} ${ioSrcs} ${viewSrcs})

add_library(SMTKCore ${smtk_srcs})
target_link_libraries(SMTKCore
  LINK_PUBLIC cJSON
  LINK_PRIVATE ${Boost_LIBRARIES})

smtk_export_header(SMTKCore SMTKCoreExports.h)
if (NOT BUILD_SHARED_LIBS AND SMTK_BUILD_PYTHON_WRAPPINGS)
  # Set position-independent flag when other project libs are shared.
  set_target_properties(SMTKCore PROPERTIES POSITION_INDEPENDENT_CODE True)
endif()

# On Mac OS X, link to the Foundation framework
if (APPLE)
  find_library(SMTK_MACOS_FOUNDATION_FMWK
    NAMES Foundation
    PATHS ${CMAKE_OSX_SYSROOT}/System/Library
    PATH_SUFFIXES Frameworks
    NO_DEFAULT_PATH)
  if (SMTK_MACOS_FOUNDATION_FMWK)
    target_link_libraries(SMTKCore
      LINK_PRIVATE ${SMTK_MACOS_FOUNDATION_FMWK})
  endif()
endif()

# On Mac OS X, set the directory included as part of the installed library's path:
if (BUILD_SHARED_LIBS)
  set_target_properties(SMTKCore PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()

#install the library and exports the library when used from a build tree
smtk_install_library(SMTKCore DEPENDS cJSON)

################################################################################
# setup install rules
################################################################################
install(
  FILES
    AutoInit.h
    PublicPointerDefs.h
    SharedFromThis.h
    SystemConfig.h
  DESTINATION
    include/smtk
)

#setup the exports for the library when used from an installed location
install(EXPORT SMTK-targets DESTINATION lib)

#wrap everything
if(SMTK_BUILD_PYTHON_WRAPPINGS AND Shiboken_FOUND)
  sbk_wrap_library(SMTKCore
    GENERATOR_ARGS --avoid-protected-hack
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    LOCAL_INCLUDE_DIRECTORIES
      "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}>"
      "$<BUILD_INTERFACE:${SMTK_BINARY_DIR}>"
      "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}/thirdparty>"
      "$<BUILD_INTERFACE:${SMTK_BINARY_DIR}/thirdparty>"
      "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}/thirdparty/cJSON>"
      ${CMAKE_CURRENT_SOURCE_DIR}/common
      ${CMAKE_CURRENT_SOURCE_DIR}/attribute
      ${CMAKE_CURRENT_SOURCE_DIR}/model
      ${CMAKE_CURRENT_SOURCE_DIR}/simulation
      ${CMAKE_CURRENT_SOURCE_DIR}/io
      ${CMAKE_CURRENT_SOURCE_DIR}/view
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
    TYPESYSTEM typesystem.xml
    HEADERS ${smtk_headers}
  )

  #get the location to copy the shiboken library so developers have an
  #easier time testing the python wrapping

  #first we have to resolve the path since shiboken library most likely
  #uses symlinks and we have to copy that version
  get_filename_component(shiboken_path "${SHIBOKEN_LIBRARY}" REALPATH)
  file(COPY "${shiboken_path}" DESTINATION "${SMTK_BINARY_DIR}")

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/smtk.py"
                 "${SMTK_BINARY_DIR}/smtk.py" @ONLY)

  #todo we need to install this file in such a way that it can resolve
  #the location of shiboken, which is going to mean running fixup bundle
  #on the mac, and than fixing up the smtk.py package script

  get_filename_component(SHIBOKEN_LIBRARY_NAME ${SHIBOKEN_LIBRARY} NAME)
  install(CODE "set(LIBRARY_OUTPUT_PATH \"${CMAKE_INSTALL_PREFIX}/lib\")
                set(SHIBOKEN_LIBRARY \"${CMAKE_INSTALL_PREFIX}/lib/${SHIBOKEN_LIBRARY_NAME}\")
                configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/smtk.py ${CMAKE_INSTALL_PREFIX}/python/smtk.py )" )
endif()

################################################################################
# Build smtk extensions
#add libraries that extend other projects to have support for SMTK
#for example extension/vtk allows you to show smtk models in vtk
################################################################################
add_subdirectory(extension)

################################################################################
# Build model bridges
# Now that the main components of smtk have been built we can build the custom
# model bridges that the user has enabled
################################################################################
add_subdirectory(bridge)

################################################################################
# Target include directories must come after all subdirectories are processed.
foreach (target
    SMTKCore
    TestBuild_smtk_attribute
    TestBuild_smtk_bridge_cgm
    TestBuild_smtk_bridge_discrete
    TestBuild_smtk_bridge_discrete_kernel
    TestBuild_smtk_bridge_exodus
    TestBuild_smtk_bridge_remote
    TestBuild_smtk_common
    TestBuild_smtk_common_testing_cxx
    TestBuild_smtk_extension_qt
    TestBuild_smtk_extension_vtk
    TestBuild_smtk_io
    TestBuild_smtk_model
    TestBuild_smtk_model_testing_cxx
    TestBuild_smtk_simulation
    TestBuild_smtk_view
  )
  if (TARGET ${target})
    message("Adding ${target} inc $<BUILD_INTERFACE:${SMTK_BINARY_DIR}>")
    target_include_directories(${target}
      PUBLIC
        "${Boost_INCLUDE_DIRS}"
        "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${SMTK_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}/thirdparty>"
        "$<BUILD_INTERFACE:${SMTK_BINARY_DIR}/thirdparty>"
        "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}/thirdparty/cJSON>"
        "$<INSTALL_INTERFACE:include>"
        "$<INSTALL_INTERFACE:include/smtk/thirdparty>"
        "$<INSTALL_INTERFACE:include/smtk/thirdparty/cJSON>"
      INTERFACE
        "$<BUILD_INTERFACE:${SMTK_SOURCE_DIR}/thirdparty/pugixml>"
    )
    if (SMTK_HASH_STORAGE AND SMTK_USE_SYSTEM_SPARSEHASH)
      target_include_directories(${target}
        PUBLIC
          "${SPARSEHASH_INCLUDE_DIRS}"
      )
    endif()
  endif()
endforeach()
