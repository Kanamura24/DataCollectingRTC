// -*- C++ -*-
/*!
 * @file  DataCollectingRTC.cpp
 * @brief DataCollectingRTC
 * @date $Date$
 *
 * @author Sugar Sweet Robotics (ysuga@ysuga.net)
 *
 * GPL, LGPL
 *
 * $Id$
 */

#include "DataCollectingRTC.h"

#include <iomanip>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

// Module specification
// <rtc-template block="module_spec">
static const char* datacollectingrtc_spec[] =
  {
    "implementation_id", "DataCollectingRTC",
    "type_name",         "DataCollectingRTC",
    "description",       "DataCollectingRTC",
    "version",           "1.0.0",
    "vendor",            "kanamura",
    "category",          "Categor",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
DataCollectingRTC::DataCollectingRTC(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inIn("in", m_in),
    m_gripperOCIn("gripperOC", m_gripperOC),
    m_jointRIn("jointR", m_jointR),
    m_jointLIn("jointL", m_jointL),
    m_cameraIn("camera", m_camera),
    m_manipMiddlePort("manipMiddle"),
    m_manipCommonPort("manipCommon")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
DataCollectingRTC::~DataCollectingRTC()
{
}



RTC::ReturnCode_t DataCollectingRTC::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers

  //in=Photointerrupter
  addInPort("in", m_inIn);

  //gripperOC=gripper open or close
  addInPort("gripperOC", m_gripperOCIn);

  //jointR=joint of migi
  addInPort("jointR", m_jointRIn);

  //jointL=joint of hidari
  addInPort("jointL", m_jointLIn);

  //camera=Webcamera
  addInPort("camera", m_cameraIn);


  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  m_manipMiddlePort.registerConsumer("JARA_ARM_ManipulatorCommonInterface_Middle", "JARA_ARM::ManipulatorCommonInterface_Middle", m_manipMiddle);
  m_manipCommonPort.registerConsumer("JARA_ARM_ManipulatorCommonInterface_Common", "JARA_ARM::ManipulatorCommonInterface_Common", m_manipCommon);
  
  // Set CORBA Service Ports
  addPort(m_manipMiddlePort);
  addPort(m_manipCommonPort);
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DataCollectingRTC::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DataCollectingRTC::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DataCollectingRTC::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t DataCollectingRTC::onActivated(RTC::UniqueId ec_id)
{

  time_t now = std::time(NULL);
 	struct tm* localNow = std::localtime(&now);
 	std::ostringstream ss;
 	ss << "log"
 		<< 1900 + localNow->tm_year
 		<< std::setw(2) << std::setfill('0') << localNow->tm_mon + 1
 		<< std::setw(2) << std::setfill('0') << localNow->tm_mday
 		<< std::setw(2) << std::setfill('0') << localNow->tm_hour
 		<< std::setw(2) << std::setfill('0') << localNow->tm_min
 		<< std::setw(2) << std::setfill('0') << localNow->tm_sec;

 	m_logDir = ss.str();
 #ifdef WIN32
 	_mkdir(m_logDir.c_str());
 #else
 	mkdir(m_logDir.c_str(), 0777);
 #endif

  std::string filename_mtn = m_logDir + "/motion.csv";
  m_JointLog.open(filename_mtn.c_str(), std::ios::out);

  std::string filename_img = m_logDir + "/front_imglist.csv";
  m_ImgLog.open(filename_img.c_str(), std::ios::out);

  
	
  return RTC::RTC_OK;
}


RTC::ReturnCode_t DataCollectingRTC::onDeactivated(RTC::UniqueId ec_id)
{
  m_JointLog.close();
  
  return RTC::RTC_OK;
}


RTC::ReturnCode_t DataCollectingRTC::onExecute(RTC::UniqueId ec_id)
{
   std::cout << "Waiting for CameraImage...." << std::ends;

   time_t now = std::time(NULL);
   struct tm* localNow = std::localtime(&now);
   std::ostringstream ss;
   ss 
     << "image"
     << 1900 + localNow->tm_year
     << std::setw(2) << std::setfill('0') << localNow->tm_mon + 1
     << std::setw(2) << std::setfill('0') << localNow->tm_mday
     << std::setw(2) << std::setfill('0') << localNow->tm_hour
     << std::setw(2) << std::setfill('0') << localNow->tm_min
     << std::setw(2) << std::setfill('0') << localNow->tm_sec
     << ".png";

   coil::sleep(1.0);
   
   std::string filename = ss.str();


 #if 1
   /// Capture Image and Save
   bool imageArrived = false;
   //long counter = 0;

  //Inport data check
  while (m_cameraIn.isNew() && (!imageArrived)) {
    m_cameraIn.read();
    imageArrived = true;
  }
  std::cout << "[ArmImageGenerator] Image Arrived." << std::endl;
  
  long width = m_camera.data.image.width;
  long height = m_camera.data.image.height;
  long channels = (m_camera.data.image.format == Img::CF_GRAY) ? 1 :
    (m_camera.data.image.format == Img::CF_RGB || m_camera.data.image.format == Img::CF_PNG || m_camera.data.image.format == Img::CF_JPEG) ? 3 :
    (m_camera.data.image.raw_data.length() / width / height);
  
  if (channels == 3)
    m_buffer.create(height, width, CV_8UC3);
  else
    m_buffer.create(height, width, CV_8UC1);
  
  long data_length = m_camera.data.image.raw_data.length();

  //long image_size = width * height * channels;
  
  if (m_camera.data.image.format == Img::CF_RGB) {
    for (int i = 0; i<height; ++i)
      memcpy(&m_buffer.data[i*m_buffer.step], &m_camera.data.image.raw_data[i*width*channels], sizeof(unsigned char)*width*channels);
    if (channels == 3)
      cv::cvtColor(m_buffer, m_buffer, CV_RGB2BGR);
  }
  else if (m_camera.data.image.format == Img::CF_JPEG || m_camera.data.image.format == Img::CF_PNG) {
    std::vector<uchar> compressed_image = std::vector<uchar>(data_length);
    memcpy(&compressed_image[0], &m_camera.data.image.raw_data[0], sizeof(unsigned char) * data_length);
    
    //Decode received compressed image
    cv::Mat decoded_image;
    if (channels == 3) {
      decoded_image = cv::imdecode(cv::Mat(compressed_image), CV_LOAD_IMAGE_COLOR);
      cv::cvtColor(decoded_image, m_buffer, CV_RGB2BGR);
    }
    else {
      decoded_image = cv::imdecode(cv::Mat(compressed_image), CV_LOAD_IMAGE_GRAYSCALE);
      m_buffer = decoded_image;
    }
  }
  
  cv::imwrite(m_logDir + "/" + filename, m_buffer);
  
#endif  

  m_ImgLog << filename << std::endl;

  float sum = 0;

  bool jointArrived = false;
   //long counter = 0;

  //Inport data check
  while (m_inIn.isNew() && m_jointRIn.isNew() && m_jointLIn.isNew() && (!jointArrived)) {
    m_inIn.read();
    m_jointRIn.read();
    m_jointLIn.read();
    jointArrived = true;
  }

  if(m_gripperOCIn.isNew()){
    m_gripperOCIn.read();
  }

    std::cout << "joint_R: "<< std::endl;
    for(int i=0; i < m_jointR.data.length(); i++){
      m_JointLog << m_jointR.data[i] << ",";
    }

    m_JointLog << m_gripperOC.data[0] << ",";

    std::cout << "joint_L: "<< std::endl;
    for(int i=0; i < m_jointL.data.length(); i++){
      m_JointLog << m_jointL.data[i] << ",";
    }

    m_JointLog << m_gripperOC.data[1] << ",";

    std::cout << "photointerrupta: "<< std::endl;
    sum += m_in.data[0]*100;
    sum += m_in.data[3]*100;
    m_JointLog << sum << std::endl; 

    
  m_JointLog.flush();
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t DataCollectingRTC::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DataCollectingRTC::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DataCollectingRTC::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DataCollectingRTC::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t DataCollectingRTC::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void DataCollectingRTCInit(RTC::Manager* manager)
  {
    coil::Properties profile(datacollectingrtc_spec);
    manager->registerFactory(profile,
                             RTC::Create<DataCollectingRTC>,
                             RTC::Delete<DataCollectingRTC>);
  }
  
};


