# SPDX-License-Identifier: GPL-2.0-or-later

if(UNIX)
  set(OPENCOLLADA_EXTRA_ARGS
    -DLIBXML2_INCLUDE_DIR=${LIBDIR}/xml2/include/libxml2
    -DLIBXML2_LIBRARIES=${LIBDIR}/xml2/lib/libxml2.a)
endif()

ExternalProject_Add(external_opencollada
  URL file://${PACKAGE_DIR}/${OPENCOLLADA_FILE}
  DOWNLOAD_DIR ${DOWNLOAD_DIR}
  URL_HASH ${OPENCOLLADA_HASH_TYPE}=${OPENCOLLADA_HASH}
  PREFIX ${BUILD_DIR}/opencollada
  PATCH_COMMAND ${PATCH_CMD} -p 1 -N -d ${BUILD_DIR}/opencollada/src/external_opencollada < ${PATCH_DIR}/opencollada.diff
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${LIBDIR}/opencollada ${DEFAULT_CMAKE_FLAGS} ${OPENCOLLADA_EXTRA_ARGS}
  INSTALL_DIR ${LIBDIR}/opencollada
)

if(UNIX)
  add_dependencies(
    external_opencollada
    external_xml2
  )
endif()

if(WIN32)
  if(BUILD_MODE STREQUAL Release)
    ExternalProject_Add_Step(external_opencollada after_install
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${LIBDIR}/opencollada/ ${HARVEST_TARGET}/opencollada/
      DEPENDEES install
    )
  endif()
  if(BUILD_MODE STREQUAL Debug)
    ExternalProject_Add_Step(external_opencollada after_install
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/buffer.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/buffer_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/ftoa.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/ftoa_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/GeneratedSaxParser.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/GeneratedSaxParser_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/MathMLSolver.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/MathMLSolver_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/OpenCOLLADABaseUtils.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/OpenCOLLADABaseUtils_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/OpenCOLLADAFramework.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/OpenCOLLADAFramework_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/OpenCOLLADASaxFrameworkLoader.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/OpenCOLLADASaxFrameworkLoader_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/OpenCOLLADAStreamWriter.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/OpenCOLLADAStreamWriter_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/pcre.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/pcre_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/UTF.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/UTF_d.lib
      COMMAND ${CMAKE_COMMAND} -E copy ${LIBDIR}/opencollada/lib/opencollada/xml.lib ${HARVEST_TARGET}/opencollada/lib/opencollada/xml_d.lib
      DEPENDEES install
    )
  endif()
endif()
