/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                CUSTOM_HID Class  Description
  *          ===================================================================
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CUSTOM_HID
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_CUSTOM_HID_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_CUSTOM_HID_Private_Macros
  * @{
  */
/**
  * @}
  */
/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @{
  */

static uint8_t USBD_CUSTOM_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_CUSTOM_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_CUSTOM_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t USBD_CUSTOM_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CUSTOM_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_CUSTOM_HID_EP0_RxReady(USBD_HandleTypeDef  *pdev);

static uint8_t *USBD_CUSTOM_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_CUSTOM_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_CUSTOM_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_CUSTOM_HID_GetDeviceQualifierDesc(uint16_t *length);
extern uint8_t* new_settings;
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_CUSTOM_HID =
{
  USBD_CUSTOM_HID_Init,
  USBD_CUSTOM_HID_DeInit,
  USBD_CUSTOM_HID_Setup,
  NULL, /*EP0_TxSent*/
  USBD_CUSTOM_HID_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */
  USBD_CUSTOM_HID_DataIn, /*DataIn*/
  USBD_CUSTOM_HID_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_CUSTOM_HID_GetHSCfgDesc,
  USBD_CUSTOM_HID_GetFSCfgDesc,
  USBD_CUSTOM_HID_GetOtherSpeedCfgDesc,
  USBD_CUSTOM_HID_GetDeviceQualifierDesc,
};

__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[USB_HID_MOUSE_REPORT_DESC_SIZ] __ALIGN_END =
{
	    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	    0x09, 0x02,                    // USAGE (Mouse)
	    0xa1, 0x01,                    // COLLECTION (Application)
	    0x09, 0x01,                    //   USAGE (Pointer)
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x05, 0x09,                    //     USAGE_PAGE (Button)
	    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
	    0x29, 0x05,                    //     USAGE_MAXIMUM (Button 5)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
	    0x95, 0x05,                    //     REPORT_COUNT (5)
	    0x75, 0x01,                    //     REPORT_SIZE (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x03,                    //     REPORT_SIZE (3)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
	    0x09, 0x30,                    //     USAGE (X)
	    0x09, 0x31,                    //     USAGE (Y)
	    0x15, 0x7f,                    //     LOGICAL_MINIMUM (-127)
	    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
	    0x75, 0x08,                    //     REPORT_SIZE (8)
	    0x95, 0x02,                    //     REPORT_COUNT (2)
	    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
	    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
	    0x09, 0x38,                    //     USAGE (Wheel)
	    0x15, 0x7f,                    //     LOGICAL_MINIMUM (-127)
	    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
	    0x75, 0x08,                    //     REPORT_SIZE (8)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
	    0xc0,                          //   END_COLLECTION
	    0xc0,                          // END_COLLECTION
};

__ALIGN_BEGIN static uint8_t HID_KEY_ReportDesc[USB_HID_KEY_REPORT_DESC_SIZ] __ALIGN_END = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x16,                    //   REPORT_COUNT (22)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};

__ALIGN_BEGIN static uint8_t HID_Comm_ReportDesc[USB_HID_COMM_REPORT_DESC_SIZ] __ALIGN_END = {
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    //   COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x1d,                    //   REPORT_COUNT (29)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

__ALIGN_BEGIN static uint8_t USBD_HID_ConfigDesc[USB_HID_CFG_DESC_CUSTOM_SIZ] __ALIGN_END = {
		/******************config desc***********************/
		9, 				                                      //bLength
		USB_DESC_TYPE_CONFIGURATION, 				            //bDescriptortype
		USB_HID_CFG_DESC_CUSTOM_SIZ, 	                  //wTotalLength (lo byte)
		0,											                       	//wTotalLength (hi byte)
		NUM_OF_INTERFACES,								      				//bNuminterfaces
		1,											                      	//bConfigurationValue
		0,											                      	//iConfiguration
		0x80 | 0x40,									                  //bmAttributes
		50,												                      //bMaxPower
		/******************interface desc***********************/
		9,                                              //bLength
		USB_DESC_TYPE_INTERFACE,                        //bDescriptortype
		0,                                              //bInterfaceNum
		0,                                              //bAlternateSettings
		1,                                              //bNumEndpoints
		3,                                              //bInterfaceClass: 3=HID
		0,                                              //bInterfaceSubClass: 1=Boot, 0=NoBoot
		2,                                              //nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
		0,                                              //iInterface: Index of string descriptor
		/*********************HID desc**************************/
		9,                                              //bLength
		HID_DESCRIPTOR_TYPE,                            //bDescriptortype
		0x11,                                           //bcdHID: HID Class Spec release number lobyte
		0x01,											                      //bcdHID: HID Class Spec release number hibyte VERSION 1.11
		0,                                              //bCountryCode
		1,                                              //bNumDescriptors: Number of hid class descriptors to follow
		0x22,                                           //bDescriptorType: 0x22=HID REPORT Type
		USB_HID_MOUSE_REPORT_DESC_SIZ,                  //wItemLength  (lobyte)
		0,                                              //wItemLength  (hibyte)
		/******************Endpoint desc***********************/
		7,                                              //bLength
		USB_DESC_TYPE_ENDPOINT,                         //bDescriptorType
		MOUSE_EP_IN_ADDR,                               //bEndpointAddress
		3,                                              //bmAttributes: Interrupt Type
		HID_EPIN1_SIZE,                                 //wMaxPacketSize: 4 bytes (lo byte)
		0,                                              //wMaxPacketSize: 4 bytes (hi byte)
		CUSTOM_HID_FS_BINTERVAL,                                //bInterval
		/******************interface desc***********************/
		9,                                              //bLength
		USB_DESC_TYPE_INTERFACE,                        //bDescriptortype
		1,                                              //bInterfaceNum
		0,                                              //bAlternateSettings
		1,                                              //bNumEndpoints
		3,                                              //bInterfaceClass: 3=HID
		0,                                              //bInterfaceSubClass: 1=Boot, 0=NoBoot
		1,                                              //nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
		0,                                              //iInterface: Index of string descriptor
		/*********************HID desc**************************/
		9,                                              //bLength
		HID_DESCRIPTOR_TYPE,                            //bDescriptortype
		0x11,                                           //bcdHID: HID Class Spec release number lobyte
		0x01,											                      //bcdHID: HID Class Spec release number hibyte VERSION 1.11
		0,                                              //bCountryCode
		1,                                              //bNumDescriptors: Number of hid class descriptors to follow
		0x22,                                           //bDescriptorType: 0x22=HID REPORT Type
		USB_HID_KEY_REPORT_DESC_SIZ,                    //wItemLength  (lobyte)
		0,                                              //wItemLength  (hibyte)
		/******************Endpoint desc***********************/
		7,                                              //bLength
		USB_DESC_TYPE_ENDPOINT,                         //bDescriptorType
		KEYBD_EP_IN_ADDR,                               //bEndpointAddress
		3,                                              //bmAttributes: Interrupt Type
		HID_EPIN2_SIZE,                                 //wMaxPacketSize: 20 bytes (lo byte)
		0,                                              //wMaxPacketSize: 20 bytes (hi byte)
		CUSTOM_HID_FS_BINTERVAL,                        //bInterval
    /******************interface desc***********************/
    9,                                              //bLength
		USB_DESC_TYPE_INTERFACE,                        //bDescriptortype
		2,                                              //bInterfaceNum
		0,                                              //bAlternateSettings
		1,                                              //bNumEndpoints
		3,                                              //bInterfaceClass: 3=HID
		0,                                              //bInterfaceSubClass: 1=Boot, 0=NoBoot
		0,                                              //nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
		0,                                              //iInterface: Index of string descriptor
    /*********************HID desc**************************/
  	9,                                              //bLength
		HID_DESCRIPTOR_TYPE,                            //bDescriptortype
		0x11,                                           //bcdHID: HID Class Spec release number lobyte
		0x01,											                      //bcdHID: HID Class Spec release number hibyte VERSION 1.11
		0,                                              //bCountryCode
		1,                                              //bNumDescriptors: Number of hid class descriptors to follow
		0x22,                                           //bDescriptorType: 0x22=HID REPORT Type
		USB_HID_COMM_REPORT_DESC_SIZ,                   //wItemLength  (lobyte)
		0,                                              //wItemLength  (hibyte)
    /******************Endpoint desc***********************/
		7,                                              //bLength
		USB_DESC_TYPE_ENDPOINT,                         //bDescriptorType
		COMM_EP_OUT_ADDR,                               //bEndpointAddress
		3,                                              //bmAttributes: Interrupt Type
		HID_EPIN3_SIZE,                                 //wMaxPacketSize: 20 bytes (lo byte)
		0,                                              //wMaxPacketSize: 20 bytes (hi byte)
		CUSTOM_HID_FS_BINTERVAL                        //bInterval
};


/* USB CUSTOM_HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ] __ALIGN_END =
{
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_HID Descriptor size */
  HID_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_HID */
  0x11,                                               /* bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
  0x22,                                               /* bDescriptorType */
  USB_HID_MOUSE_REPORT_DESC_SIZ,                   /* wItemLength: Total length of Report descriptor */
  0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CUSTOM_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_CUSTOM_HID_HandleTypeDef *hhid;

  hhid = USBD_malloc(sizeof(USBD_CUSTOM_HID_HandleTypeDef));

  if (hhid == NULL)
  {
    pdev->pClassData = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassData = (void *)hhid;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
	pdev->ep_in[MOUSE_EP_IN_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;
	pdev->ep_in[KEYBD_EP_IN_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;
  }
  else   /* LOW and FULL-speed endpoints */
  {
    pdev->ep_in[MOUSE_EP_IN_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;
    pdev->ep_in[KEYBD_EP_IN_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;
    pdev->ep_out[COMM_EP_OUT_ADDR & 0xFU].bInterval = CUSTOM_HID_FS_BINTERVAL;
  }

  /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, MOUSE_EP_IN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN1_SIZE);
  (void)USBD_LL_OpenEP(pdev, KEYBD_EP_IN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN2_SIZE);

  pdev->ep_in[MOUSE_EP_IN_ADDR & 0xFU].is_used = 1U;
  pdev->ep_in[KEYBD_EP_IN_ADDR & 0xFU].is_used = 1U;
  /* Open EP OUT */
  (void)USBD_LL_OpenEP(pdev, COMM_EP_OUT_ADDR, USBD_EP_TYPE_INTR, HID_EPIN3_SIZE);

  pdev->ep_out[COMM_EP_OUT_ADDR & 0xFU].is_used = 1U;

  hhid->state = CUSTOM_HID_IDLE;

  //((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->Init();

  /* Prepare Out endpoint to receive 1st packet */
  (void)USBD_LL_PrepareReceive(pdev, COMM_EP_OUT_ADDR, hhid->Report_buf, TOTAL_SETTINGS);


  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

  /* Close CUSTOM_HID EP IN */
  (void)USBD_LL_CloseEP(pdev, MOUSE_EP_IN_ADDR);
  (void)USBD_LL_CloseEP(pdev, KEYBD_EP_IN_ADDR);

  pdev->ep_in[MOUSE_EP_IN_ADDR & 0xFU].is_used = 0U;
  pdev->ep_in[MOUSE_EP_IN_ADDR & 0xFU].bInterval = 0U;

  pdev->ep_in[KEYBD_EP_IN_ADDR & 0xFU].is_used = 0U;
  pdev->ep_in[KEYBD_EP_IN_ADDR & 0xFU].bInterval = 0U;

  /* Close CUSTOM_HID EP OUT */
  (void)USBD_LL_CloseEP(pdev, COMM_EP_OUT_ADDR);
  pdev->ep_out[COMM_EP_OUT_ADDR & 0xFU].is_used = 0U;
  pdev->ep_out[COMM_EP_OUT_ADDR & 0xFU].bInterval = 0U;

  /* Free allocated memory */
  if (pdev->pClassData != NULL)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }


  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_Setup(USBD_HandleTypeDef *pdev,
                                     USBD_SetupReqTypedef *req)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;
  uint16_t len = 0U;
  uint8_t  *pbuf = NULL;
  uint16_t status_info = 0U;
  USBD_StatusTypeDef ret = USBD_OK;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS:
      switch (req->bRequest)
      {
        case CUSTOM_HID_REQ_SET_PROTOCOL:
          hhid->Protocol = (uint8_t)(req->wValue);
          break;

        case CUSTOM_HID_REQ_GET_PROTOCOL:
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->Protocol, 1U);
          break;

        case CUSTOM_HID_REQ_SET_IDLE:
          hhid->IdleState = (uint8_t)(req->wValue >> 8);
          break;

        case CUSTOM_HID_REQ_GET_IDLE:
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->IdleState, 1U);
          break;

        case CUSTOM_HID_REQ_SET_REPORT:
          hhid->IsReportAvailable = 1U;
          (void)USBD_CtlPrepareRx(pdev, hhid->Report_buf, req->wLength);
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_DESCRIPTOR:
            if ((req->wValue >> 8) == HID_REPORT_DESC)
            {
          	  if(req->wIndex == 0){
                len = MIN(USB_HID_MOUSE_REPORT_DESC_SIZ, req->wLength);
                pbuf = HID_MOUSE_ReportDesc;
          	  } else if (req->wIndex == 1){
                len = MIN(USB_HID_KEY_REPORT_DESC_SIZ, req->wLength);
                pbuf = HID_KEY_ReportDesc;
          	  } else if (req->wIndex == 2){
                len = MIN(USB_HID_COMM_REPORT_DESC_SIZ, req->wLength);
                pbuf = HID_Comm_ReportDesc;               
              }
            }
            else
            {
              if ((req->wValue >> 8) == HID_DESCRIPTOR_TYPE)
              {
                pbuf = USBD_HID_Desc;
                len = MIN(USB_HID_DESC_SIZ, req->wLength);
              }
          }

          (void)USBD_CtlSendData(pdev, pbuf, len);
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->AltSetting, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hhid->AltSetting = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }
  return (uint8_t)ret;
}

/**
  * @brief  USBD_CUSTOM_HID_SendReport
  *         Send CUSTOM_HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len, uint8_t epnum)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid;

  if (pdev->pClassData == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;

  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    if (hhid->state == CUSTOM_HID_IDLE)
    {
      hhid->state = CUSTOM_HID_BUSY;
      (void)USBD_LL_Transmit(pdev, epnum, report, len);
    }
    else
    {
      return (uint8_t)USBD_BUSY;
    }
  }
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_GetFSCfgDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_HID_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_HID_ConfigDesc);

  return USBD_HID_ConfigDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_GetHSCfgDesc
  *         return HS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_HID_GetHSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_HID_ConfigDesc);

  return USBD_HID_ConfigDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_HID_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_HID_ConfigDesc);

  return USBD_HID_ConfigDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);

  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData)->state = CUSTOM_HID_IDLE;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);
  USBD_CUSTOM_HID_HandleTypeDef *hhid;
  UNUSED(hhid);
  if (pdev->pClassData == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application processing */
  ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->OutEvent();

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_HID_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid;

  if (pdev->pClassData == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;

  /* Resume USB Out process */
  (void)USBD_LL_PrepareReceive(pdev, COMM_EP_OUT_ADDR, hhid->Report_buf, TOTAL_SETTINGS);
  new_settings = hhid->Report_buf;
  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hhid->IsReportAvailable == 1U)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)pdev->pUserData)->OutEvent();
    hhid->IsReportAvailable = 0U;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_CUSTOM_HID_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_CUSTOM_HID_DeviceQualifierDesc);

  return USBD_CUSTOM_HID_DeviceQualifierDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CUSTOMHID Interface callback
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_RegisterInterface(USBD_HandleTypeDef *pdev,
                                          USBD_CUSTOM_HID_ItfTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  pdev->pUserData = fops;

  return (uint8_t)USBD_OK;
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
