INCLUDE(FindOpenCascade.cmake)

INCLUDE_DIRECTORIES(${OPENCASCADE_INCLUDE_DIR})

ADD_LIBRARY(TexGenExport STATIC
Exporter.cpp
Exporter.h
)

ADD_DEFINITIONS(-DWNT)

TARGET_LINK_LIBRARIES(TexGenExport TexGenCore
${OPENCASCADE_LIBRARY_DIR}/TKBO.lib
${OPENCASCADE_LIBRARY_DIR}/TKBOOL.lib
${OPENCASCADE_LIBRARY_DIR}/TKBREP.lib
${OPENCASCADE_LIBRARY_DIR}/TKERNEL.lib
${OPENCASCADE_LIBRARY_DIR}/TKG2D.lib
${OPENCASCADE_LIBRARY_DIR}/TKG3D.lib
${OPENCASCADE_LIBRARY_DIR}/TKGEOMALGO.lib
${OPENCASCADE_LIBRARY_DIR}/TKGEOMBASE.lib
${OPENCASCADE_LIBRARY_DIR}/TKIGES.lib
${OPENCASCADE_LIBRARY_DIR}/TKMATH.lib
${OPENCASCADE_LIBRARY_DIR}/TKOFFSET.lib
${OPENCASCADE_LIBRARY_DIR}/TKPRIM.lib
${OPENCASCADE_LIBRARY_DIR}/TKSHHEALING.lib
${OPENCASCADE_LIBRARY_DIR}/TKSTEP.lib
${OPENCASCADE_LIBRARY_DIR}/TKSTEP209.lib
${OPENCASCADE_LIBRARY_DIR}/TKSTEPATTR.lib
${OPENCASCADE_LIBRARY_DIR}/TKSTEPBASE.lib
${OPENCASCADE_LIBRARY_DIR}/TKTOPALGO.lib
${OPENCASCADE_LIBRARY_DIR}/TKXSBASE.lib
)


