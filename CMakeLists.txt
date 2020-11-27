project(LidarViewer)
add_definitions(${QT_DEFINITIONS})


set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)

set(PACPUS_CITYVIP_INCLUDE_DIR "${PACPUSDEV_ROOT}/include")

################################################################################
create_export(EXPORT_HDR ${PROJECT_NAME})
create_export(EXPORT_HDR ${PROJECT_NAME})
add_definitions( -DOPENGL3DVIEWER_EXPORTS)
pacpus_plugin(PLUGIN_CPP PLUGIN_HDR ${PROJECT_NAME})

find_package(Qt5OpenGL REQUIRED)
#find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    list(APPEND OpenCV_LIBS ${OpenCV_LIBRARIES})
    list(APPEND OpenCV_INCLUDE_DIR ${OpenCV_INCLUDE_DIRS})

    include_directories(${OpenCV_INCLUDE_DIRS})
    list(APPEND LIBS ${OpenCV_LIBS})
endif(OpenCV_FOUND)
#set(OpenCV_LIB_DIREC  "D:/opencv/build/install/x86/vc10/lib")

add_definitions(
  ${QT_DEFINITIONS}
)
################################################################################
# Include directories
include_directories(

    ${PROJECT_BINARY_DIR}
    ${OpenCV_INCLUDE_DIR}
	${PACPUS_CITYVIP_INCLUDE_DIR}

	
    ${QT_INCLUDE_DIR}
	D:/Qt/5.2.0/5.2.0/msvc2010_opengl/include/QtOpenGL
    ${PACPUS_INCLUDE_DIR}
    ${PACPUS_INCLUDE_DIR}/structure/LDMRS
	${PACPUS_INCLUDE_DIR}/Pacpus/
	${PACPUS_INCLUDE_DIR}/structure
)

################################################################################
# Link directories
link_directories(
    ${PACPUS_LIB_DIR}
	${LDMRS_LIB_DIR}
	D:/Qt/5.2.0/5.2.0/msvc2010_opengl/lib
	
)

################################################################################
# FILES
set(HDRS
    ${EXPORT_HDR}
    LidarViewer.h
   LidarViewerImpl.h


    LidarScene.h
    LidarView.h

)

set(SRCS
    ${PLUGIN_CPP}
    LidarViewer.cpp

    LidarViewerImpl.cpp
 
    LidarScene.cpp
    LidarView.cpp

)

set(MOC_FILES
    ${PLUGIN_HDR}
    LidarViewer.h
   LidarViewerImpl.h


    LidarScene.h
    LidarView.h
)

set(UI_FILES
)

################################################################################
# Call moc, uic
qt_wrap_cpp(MOC_SRCS
    ${MOC_FILES}
)
qt_wrap_ui(UI_SRCS
    ${UI_FILES}
)

################################################################################
# BUILD and LINK
pacpus_add_library(${PROJECT_NAME} SHARED
    ${SRCS}
    ${HDRS}
    ${MOC_SRCS}
    ${UI_SRCS}
)

# LIBS
set(LIBS
    ${PACPUS_LIBRARIES}
    ${PACPUS_DEPENDENCIES_LIB}
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
	${Boost_LIBRARIES}
	${LDMRS_LIBRARIES}
	optimized Qt5Widgets debug Qt5Widgetsd
    optimized Qt5OpenGL debug Qt5OpenGLd
	OpenGL32
	glew32
	${OpenCV_LIBS}
	
)

# LINK
target_link_libraries(${PROJECT_NAME} ${LIBS})

################################################################################
# FOLDERS
pacpus_folder(${PROJECT_NAME} "components")

################################################################################
# INSTALL
pacpus_install(${PROJECT_NAME})

