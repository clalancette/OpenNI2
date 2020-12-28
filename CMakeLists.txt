cmake_minimum_required(VERSION 3.5)

project(OpenNI2)

include(CheckCXXCompilerFlag)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -fPIC -fvisibility=hidden -Woverloaded-virtual -Wnon-virtual-dtor -Wformat=2)
  check_cxx_compiler_flag(-msse3 HAS_SSE3)
  if (HAS_SSE3)
    add_compile_options(-msse3)
  endif()
endif()

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -DNDEBUG -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -DNDEBUG -O2")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(GLUT REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(Libusb1 REQUIRED)
find_package(Threads REQUIRED)
find_package(udev REQUIRED)

set(XNLIB_LINUX_SOURCES
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxCriticalSections.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxDebug.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxEvents.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxFiles.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxKeyboard.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxINI.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxMemory.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxMutex.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxPosixEvents.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxPosixNamedEvents.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxProcesses.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxSharedLibs.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxSharedMemory.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxStrings.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxSysVNamedEvents.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxThreads.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxTime.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxUSB.cpp
  ThirdParty/PSCommon/XnLib/Source/Linux/XnLinuxUSBDevice.cpp
)

add_library(XnLib STATIC
  ThirdParty/PSCommon/XnLib/Source/XnDump.cpp
  ThirdParty/PSCommon/XnLib/Source/XnDumpFileWriter.cpp
  ThirdParty/PSCommon/XnLib/Source/XnErrorLogger.cpp
  ThirdParty/PSCommon/XnLib/Source/XnFiles.cpp
  ThirdParty/PSCommon/XnLib/Source/XnFPSCalculator.cpp
  ThirdParty/PSCommon/XnLib/Source/XnJpeg.cpp
  ThirdParty/PSCommon/XnLib/Source/XnLog.cpp
  ThirdParty/PSCommon/XnLib/Source/XnLogConsoleWriter.cpp
  ThirdParty/PSCommon/XnLib/Source/XnLogFileWriter.cpp
  ThirdParty/PSCommon/XnLib/Source/XnOSMemoryProfiling.cpp
  ThirdParty/PSCommon/XnLib/Source/XnProfiling.cpp
  ThirdParty/PSCommon/XnLib/Source/XnScheduler.cpp
  ThirdParty/PSCommon/XnLib/Source/XnStatus.cpp
  ThirdParty/PSCommon/XnLib/Source/XnStrings.cpp
  ThirdParty/PSCommon/XnLib/Source/XnThreads.cpp
  ThirdParty/PSCommon/XnLib/Source/XnOS.cpp
  ThirdParty/PSCommon/XnLib/Source/XnUSB.cpp
  ThirdParty/PSCommon/XnLib/Source/XnVector3D.cpp
  ${XNLIB_LINUX_SOURCES}
)
target_include_directories(XnLib PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(XnLib PUBLIC
  ${CMAKE_DL_LIBS}
  ${UDEV_LIBRARIES}
  ${LIBUSB1_LIBRARIES}
  JPEG::JPEG
  Threads::Threads
  -Wl,--no-undefined
)
# Since we are building and linking XnLib statically, no need to install it

add_library(DepthUtils STATIC
  Source/DepthUtils/DepthUtils.cpp
  Source/DepthUtils/DepthUtilsImpl.cpp
)
target_include_directories(DepthUtils PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(XnLib PUBLIC
  -Wl,--no-undefined
)
# Since we are building and linking DepthUtils statically, no need to install it

add_library(OpenNI2 SHARED
  Source/Core/OniContext.cpp
  Source/Core/OniDataRecords.cpp
  Source/Core/OniDeviceDriver.cpp
  Source/Core/OniDevice.cpp
  Source/Core/OniDriverHandler.cpp
  Source/Core/OniFileRecorder.cpp
  Source/Core/OniFrameManager.cpp
  Source/Core/OniRecorder.cpp
  Source/Core/OniSensor.cpp
  Source/Core/OniStream.cpp
  Source/Core/OniStreamFrameHolder.cpp
  Source/Core/OniSyncedStreamsFrameHolder.cpp
  Source/Core/OpenNI.cpp
  Source/Drivers/OniFile/Formats/XnCodec.cpp
  Source/Drivers/OniFile/Formats/XnStreamCompression.cpp
)
target_compile_definitions(OpenNI2 PRIVATE
  OPENNI2_EXPORT
)
target_include_directories(OpenNI2 PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/OniFile>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/OniFile/Formats>"
)
target_link_libraries(OpenNI2
  XnLib
  -Wl,--no-undefined
)
install(TARGETS OpenNI2
  DESTINATION .
)

add_library(DummyDevice SHARED
  Source/Drivers/DummyDevice/DummyDevice.cpp
)
target_include_directories(DummyDevice PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(DummyDevice
  XnLib
  -Wl,--no-undefined
)
install(TARGETS DummyDevice
  DESTINATION OpenNI2/Drivers
)

add_library(DriverCommon STATIC
  Source/Drivers/DriverCommon/Core/XnBuffer.cpp
  Source/Drivers/DriverCommon/Core/XnCore.cpp
  Source/Drivers/DriverCommon/Core/XnIOFileStream.cpp

  Source/Drivers/DriverCommon/DDK/XnActualGeneralProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnActualIntProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnActualPropertiesHash.cpp
  Source/Drivers/DriverCommon/DDK/XnActualRealProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnActualStringProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnDDK.cpp
  Source/Drivers/DriverCommon/DDK/XnDepthStream.cpp
  Source/Drivers/DriverCommon/DDK/XnDeviceBase.cpp
  Source/Drivers/DriverCommon/DDK/XnDeviceModule.cpp
  Source/Drivers/DriverCommon/DDK/XnDeviceModuleHolder.cpp
  Source/Drivers/DriverCommon/DDK/XnDeviceStream.cpp
  Source/Drivers/DriverCommon/DDK/XnGeneralProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnFrameBufferManager.cpp
  Source/Drivers/DriverCommon/DDK/XnFrameStream.cpp
  Source/Drivers/DriverCommon/DDK/XnImageStream.cpp
  Source/Drivers/DriverCommon/DDK/XnIntProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnIntPropertySynchronizer.cpp
  Source/Drivers/DriverCommon/DDK/XnIRStream.cpp
  Source/Drivers/DriverCommon/DDK/XnPixelStream.cpp
  Source/Drivers/DriverCommon/DDK/XnProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnPropertySet.cpp
  Source/Drivers/DriverCommon/DDK/XnRealProperty.cpp
  Source/Drivers/DriverCommon/DDK/XnShiftToDepthStreamHelper.cpp
  Source/Drivers/DriverCommon/DDK/XnStreamingStream.cpp
  Source/Drivers/DriverCommon/DDK/XnStringProperty.cpp

  Source/Drivers/DriverCommon/DriverImpl/XnOniColorStream.cpp
  Source/Drivers/DriverCommon/DriverImpl/XnOniDepthStream.cpp
  Source/Drivers/DriverCommon/DriverImpl/XnOniDevice.cpp
  Source/Drivers/DriverCommon/DriverImpl/XnOniDriver.cpp
  Source/Drivers/DriverCommon/DriverImpl/XnOniIRStream.cpp
  Source/Drivers/DriverCommon/DriverImpl/XnOniMapStream.cpp
  Source/Drivers/DriverCommon/DriverImpl/XnOniStream.cpp

  Source/Drivers/DriverCommon/Formats/XnFormats.cpp
  Source/Drivers/DriverCommon/Formats/XnFormatsMirror.cpp
  Source/Drivers/DriverCommon/Formats/XnFormatsStatus.cpp

  Source/Drivers/DriverCommon/Sensor/Bayer.cpp
  Source/Drivers/DriverCommon/Sensor/Uncomp.cpp
  Source/Drivers/DriverCommon/Sensor/XnBayerImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnCmosInfo.cpp
  Source/Drivers/DriverCommon/Sensor/XnDataProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnDataProcessorHolder.cpp
  Source/Drivers/DriverCommon/Sensor/XnDepthProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnDeviceSensorInit.cpp
  Source/Drivers/DriverCommon/Sensor/XnDeviceSensorIO.cpp
  Source/Drivers/DriverCommon/Sensor/XnDeviceSensorProtocol.cpp
  Source/Drivers/DriverCommon/Sensor/XnFirmwareCommands.cpp
  Source/Drivers/DriverCommon/Sensor/XnFirmwareStreams.cpp
  Source/Drivers/DriverCommon/Sensor/XnFrameStreamProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnGeneralDebugProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnGMCDebugProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnJpegToRGBImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnNesaDebugProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnIRProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnJpegImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnPacked11DepthProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnPacked12DepthProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnPassThroughImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnPSCompressedDepthProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnPSCompressedImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorDepthStream.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorFirmware.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorFixedParams.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorFPS.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorImageStream.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorIRStream.cpp
  Source/Drivers/DriverCommon/Sensor/XnSensorStreamHelper.cpp
  Source/Drivers/DriverCommon/Sensor/XnStreamProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnTecDebugProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnUncompressedBayerProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnUncompressedDepthProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnUncompressedYUV422toRGBImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnUncompressedYUYVtoRGBImageProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnWavelengthCorrectionDebugProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/XnWholePacketProcessor.cpp
  Source/Drivers/DriverCommon/Sensor/YUV.cpp
)
target_include_directories(DriverCommon PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/DepthUtils>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(DriverCommon
  DepthUtils
  XnLib
  -Wl,--no-undefined
)

add_library(PS1080 SHARED
  Source/Drivers/DriverCommon/DriverImpl/XnExportedOniDriver.cpp

  Source/Drivers/PS1080/DDK/XnShiftToDepth.cpp

  Source/Drivers/PS1080/Sensor/XnDeviceEnumeration.cpp
  Source/Drivers/PS1080/Sensor/XnHostProtocol.cpp
  Source/Drivers/PS1080/Sensor/XnSensor.cpp
  Source/Drivers/PS1080/Sensor/XnSensorFirmwareParams.cpp
)
target_include_directories(PS1080 PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/DepthUtils>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/DDK>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Formats>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Sensor>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/PS1080>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(PS1080
  DepthUtils
  DriverCommon
  XnLib
  -Wl,--no-undefined
)
install(TARGETS PS1080
  DESTINATION OpenNI2/Drivers
)

add_library(orbbec SHARED
  Source/Drivers/DriverCommon/DriverImpl/XnExportedOniDriver.cpp

  Source/Drivers/orbbec/DDK/XnShiftToDepth.cpp

  Source/Drivers/orbbec/Sensor/XnDeviceEnumeration.cpp
  Source/Drivers/orbbec/Sensor/XnHostProtocol.cpp
  Source/Drivers/orbbec/Sensor/XnSensor.cpp
  Source/Drivers/orbbec/Sensor/XnSensorFirmwareParams.cpp
)
target_include_directories(orbbec PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/DepthUtils>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/DDK>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Formats>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Sensor>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/orbbec>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(orbbec
  DepthUtils
  DriverCommon
  XnLib
  -Wl,--no-undefined
)
install(TARGETS orbbec
  DESTINATION OpenNI2/Drivers
)

add_library(TestDevice SHARED
  Source/Drivers/TestDevice/TestDevice.cpp
  Source/Drivers/TestDevice/TestDriver.cpp
  Source/Drivers/TestDevice/TestStream.cpp
)
target_include_directories(TestDevice PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/TestDevice>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(TestDevice
  XnLib
  -Wl,--no-undefined
)
install(TARGETS TestDevice
  DESTINATION OpenNI2/Drivers
)

add_library(PSLink SHARED
  Source/Drivers/PSLink/LinkDeviceEnumeration.cpp
  Source/Drivers/PSLink/PrimeClient.cpp
  Source/Drivers/PSLink/PS1200Device.cpp
  Source/Drivers/PSLink/DriverImpl/LinkExportedOniDriver.cpp
  Source/Drivers/PSLink/DriverImpl/LinkOniDepthStream.cpp
  Source/Drivers/PSLink/DriverImpl/LinkOniDevice.cpp
  Source/Drivers/PSLink/DriverImpl/LinkOniDriver.cpp
  Source/Drivers/PSLink/DriverImpl/LinkOniIRStream.cpp
  Source/Drivers/PSLink/DriverImpl/LinkOniMapStream.cpp
  Source/Drivers/PSLink/DriverImpl/LinkOniStream.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnClientUSBConnectionFactory.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnClientUSBControlEndpoint.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnClientUSBInDataEndpoint.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnClientUSBOutDataEndpoint.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLink11BitS2DParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLink12BitS2DParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLink16zParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLink24zYuv422Parser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLink6BitParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkContInputStream.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkControlEndpoint.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkFrameInputStream.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkInputDataEndpoint.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkInputStream.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkInputStreamsMgr.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkLogParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkMsgEncoder.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkMsgParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkOutputDataEndpoint.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkOutputStream.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkOutputStreamsMgr.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkPacked10BitParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkProtoUtils.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkResponseMsgParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnShiftToDepth.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkUnpackedDataReductionParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkUnpackedS2DParser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkYuv422ToRgb888Parser.cpp
  Source/Drivers/PSLink/LinkProtoLib/XnLinkYuvToRgb.cpp
)
target_include_directories(PSLink PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/PSLink>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/PSLink/LinkProtoLib>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/PSLink/Protocols/XnLinkProto>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(PSLink
  XnLib
  -Wl,--no-undefined
)
install(TARGETS PSLink
  DESTINATION OpenNI2/Drivers
)

add_library(OniFile SHARED
  Source/Drivers/OniFile/DataRecords.cpp
  Source/Drivers/OniFile/PlayerCodecFactory.cpp
  Source/Drivers/OniFile/PlayerDevice.cpp
  Source/Drivers/OniFile/PlayerDriver.cpp
  Source/Drivers/OniFile/PlayerNode.cpp
  Source/Drivers/OniFile/PlayerSource.cpp
  Source/Drivers/OniFile/PlayerStream.cpp
  Source/Drivers/OniFile/Formats/XnCodec.cpp
  Source/Drivers/OniFile/Formats/XnStreamCompression.cpp
)
target_include_directories(OniFile PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
)
target_link_libraries(OniFile
  XnLib
  -Wl,--no-undefined
)
install(TARGETS OniFile
  DESTINATION OpenNI2/Drivers
)

add_executable(PS1080Console
  Source/Drivers/PS1080/PS1080Console/PS1080Console.cpp
)
target_include_directories(PS1080Console PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/DriverCommon/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/PS1080/Include>"
)
target_link_libraries(PS1080Console
  OpenNI2
  XnLib
  -Wl,--no-undefined
)
install(TARGETS PS1080Console
  DESTINATION .
)

add_executable(PSLinkConsole
  Source/Drivers/PSLink/PSLinkConsole/PSLinkConsole.cpp
)
target_include_directories(PSLinkConsole PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PSCommon/XnLib/Include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/Drivers/PSLink/Protocols/XnLinkProto>"
)
target_link_libraries(PSLinkConsole
  PSLink
  OpenNI2
  XnLib
  -Wl,--no-undefined
)
install(TARGETS PSLinkConsole
  DESTINATION .
)

add_executable(NiViewer
  Source/Tools/NiViewer/Capture.cpp
  Source/Tools/NiViewer/Device.cpp
  Source/Tools/NiViewer/Draw.cpp
  Source/Tools/NiViewer/Keyboard.cpp
  Source/Tools/NiViewer/Menu.cpp
  Source/Tools/NiViewer/MouseInput.cpp
  Source/Tools/NiViewer/NiViewer.cpp
)
target_link_libraries(NiViewer
  GLUT::GLUT
  OpenGL::GLU
  OpenNI2
  XnLib
  -Wl,--no-undefined
)
install(TARGETS NiViewer
  DESTINATION .
)

add_executable(SimpleRead
  Samples/SimpleRead/main.cpp
)
target_include_directories(SimpleRead PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
)
target_link_libraries(SimpleRead
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS SimpleRead
  DESTINATION .
)

add_executable(EventBasedRead
  Samples/EventBasedRead/main.cpp
)
target_include_directories(EventBasedRead PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
)
target_link_libraries(EventBasedRead
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS EventBasedRead
  DESTINATION .
)

add_executable(MultipleStreamRead
  Samples/MultipleStreamRead/main.cpp
)
target_include_directories(MultipleStreamRead PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
)
target_link_libraries(MultipleStreamRead
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS MultipleStreamRead
  DESTINATION .
)

add_library(MWClosestPoint STATIC
  Samples/MWClosestPoint/MWClosestPoint.cpp
)
target_include_directories(MWClosestPoint PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/MWClosestPoint>"
)
target_compile_definitions(MWClosestPoint PRIVATE
  _CLOSEST_POINT
)
target_link_libraries(MWClosestPoint
  OpenNI2
  -Wl,--no-undefined
)

add_executable(MWClosestPointApp
  Samples/MWClosestPointApp/main.cpp
)
target_include_directories(MWClosestPointApp PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/MWClosestPoint>"
)
target_link_libraries(MWClosestPointApp
  MWClosestPoint
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS MWClosestPointApp
  DESTINATION .
)

add_executable(SimpleViewer
  Samples/SimpleViewer/Viewer.cpp
  Samples/SimpleViewer/main.cpp
)
target_include_directories(SimpleViewer PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
)
target_link_libraries(SimpleViewer
  GLUT::GLUT
  OpenGL::GLU
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS SimpleViewer
  DESTINATION .
)

add_executable(MultiDepthViewer
  Samples/MultiDepthViewer/Viewer.cpp
  Samples/MultiDepthViewer/main.cpp
)
target_include_directories(MultiDepthViewer PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
)
target_link_libraries(MultiDepthViewer
  GLUT::GLUT
  OpenGL::GLU
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS MultiDepthViewer
  DESTINATION .
)

add_executable(ClosestPointViewer
  Samples/ClosestPointViewer/Viewer.cpp
  Samples/ClosestPointViewer/main.cpp
)
target_include_directories(ClosestPointViewer PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples/Common>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Samples>"
)
target_link_libraries(ClosestPointViewer
  GLUT::GLUT
  MWClosestPoint
  OpenGL::GLU
  OpenNI2
  -Wl,--no-undefined
)
install(TARGETS ClosestPointViewer
  DESTINATION .
)

install(DIRECTORY Include DESTINATION .)
