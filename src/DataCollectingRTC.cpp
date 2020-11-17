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
  addInPort("in", m_inIn);
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
  return RTC::RTC_OK;
}


RTC::ReturnCode_t DataCollectingRTC::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t DataCollectingRTC::onExecute(RTC::UniqueId ec_id)
{
  float sum = 0;

  if(m_inIn.isNew()) {
    m_inIn.read();
    sum += m_in.data[0]*100;
    sum += m_in.data[3]*100;
    std::cout << "Sum= " << sum << std::endl; 
  }
  coil::sleep(2.0);
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


