set(smtkPolygonSessionPythonTests
)

# Additional tests that require SMTK_DATA_DIR
set(smtkPolygonSessionPythonDataTests
  polygonCreate
  #polygonReadFile
)

foreach (test ${smtkPolygonSessionPythonTests})
  add_test(${test}Py ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${test}.py)
  set_tests_properties(${test}Py
    PROPERTIES
    ENVIRONMENT "PYTHONPATH=${VTKPY_DIR}${SHIBOKEN_SMTK_PYTHON};${LIB_ENV_VAR}"
  )
endforeach()

if (SMTK_DATA_DIR AND EXISTS ${SMTK_DATA_DIR}/ReadMe.mkd)
  foreach (test ${smtkPolygonSessionPythonDataTests})
    add_test(${test}Py
      ${PYTHON_EXECUTABLE}
      ${CMAKE_CURRENT_SOURCE_DIR}/${test}.py
      -D "${SMTK_DATA_DIR}"
    )
    set_tests_properties(${test}Py
      PROPERTIES
        ENVIRONMENT "PYTHONPATH=${VTKPY_DIR}${SHIBOKEN_SMTK_PYTHON};${LIB_ENV_VAR}"
    )
  endforeach()
endif()
