# ADD_SUBDIRECTORY("common")
SET(LIBRARY_COMMON "common-lib")
SET(SOURCE_COMMON
    "common/utils.cpp"
    "common/utils.h"
    "common/camera.cpp"
    "common/camera.h"
    "common/light.cpp"
    "common/light.h"
    "common/shader.cpp"
    "common/shader.h"
    "common/mesh.cpp"
    "common/mesh.h"
    "common/model.cpp"
    "common/model.h"
    "common/wrapper_glfw.cpp"
    "common/wrapper_glfw.h"
    )
SOURCE_GROUP(TREE ${CMAKE_SOURCE_DIR} FILES ${SOURCE_COMMON})
ADD_LIBRARY(${LIBRARY_COMMON} OBJECT ${SOURCE_COMMON})
TARGET_LINK_LIBRARIES("${PROJECT_NAME}" ${LIBRARY_COMMON})
