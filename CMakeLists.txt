cmake_minimum_required(VERSION 2.8)
PROJECT("TransformWidget")

#SET(ParaView_DIR "/Users/delvistaveras/Desktop/Paraview_Plugin/ParaView/build")

#--------------------------------------------------
# Find and Use ParaView
#--------------------------------------------------
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

#--------------------------------------------------
# Set project include directories 
# pv-common is assumed to be checkout out too
#--------------------------------------------------
INCLUDE_DIRECTORIES(
  "${PROJECT_SOURCE_DIR}"
  "${PROJECT_BINARY_DIR}"
)

#--------------------------------------------------
# make sure all *.h files for ui are wrapped by moc
#--------------------------------------------------
QT4_WRAP_CPP(MOC_SRCS
    pqParaWidget.h
    Leap/VTK/vtkLeapController.h
    Leap/VTK/vtkLeapListener.h
)

#--------------------------------------------------
# invoke macro that adds our grid source 3D widget
#--------------------------------------------------
ADD_3DWIDGET(
  PARA_IFACE_W
  PARA_IFACE_SRCS_W
  CLASS_NAME 
    pqParaWidget
  WIDGET_TYPE 
    # string found in <hints> section of xml
    "Boxing"
)

#--------------------------------------------------
# Define Plugin 
#--------------------------------------------------
SET(PLUGIN_NAME vtkTransformWidget)
ADD_PARAVIEW_PLUGIN(
  ${PLUGIN_NAME}
  "1.1" 

  SERVER_MANAGER_XML 
    ${CMAKE_CURRENT_SOURCE_DIR}/LeapBoxTransformFilter.xml
    ${TIPSY_XML}
  SERVER_MANAGER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vtkNewTransformFilter.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/vtkLeapBoxRepresentation.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/vtkLeapBoxWidget2.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/vtkSMLeapBoxRepresentationProxy.cxx
    ${TIPSY_CXX}
  SOURCES
    ${outsrcs0}
    ${outsrcs1}
    ${outsrcs2}
    ${outsrcs3}
    ${moc_srcs}
  GUI_INTERFACES 
    ${PARA_IFACE_W}
  GUI_SOURCES
    ${MOC_SRCS}
    ${UI_BUILT_SOURCES}
    ${UI_MOC}
    ${PARA_IFACE_SRCS_W}
    ${CMAKE_CURRENT_SOURCE_DIR}/pqParaWidget.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Leap/VTK/vtkLeapController.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Leap/VTK/vtkLeapListener.cxx
  GUI_RESOURCE_FILES
)
