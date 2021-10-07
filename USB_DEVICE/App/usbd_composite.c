#include "usbd_composite.h"

USBD_CDC_HandleTypeDef pCDCData;
USBD_HID_HandleTypeDef pHIDData;

static uint8_t  USBD_Composite_Init (USBD_HandleTypeDef *pdev,
                            uint8_t cfgidx);
static uint8_t  USBD_Composite_DeInit (USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);
static uint8_t  USBD_Composite_Setup (USBD_HandleTypeDef *pdev,
                             USBD_SetupReqTypedef *req);
static uint8_t  USBD_Composite_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t  USBD_Composite_DataIn (USBD_HandleTypeDef *pdev,
                              uint8_t epnum);
static uint8_t  USBD_Composite_DataOut (USBD_HandleTypeDef *pdev,
                               uint8_t epnum);
static uint8_t  *USBD_Composite_GetFSCfgDesc (uint16_t *length);
static uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor (uint16_t *length);

USBD_ClassTypeDef  USBD_COMPOSITE =
{
  USBD_Composite_Init,
  USBD_Composite_DeInit,
  USBD_Composite_Setup,
  NULL, /*EP0_TxSent*/
  USBD_Composite_EP0_RxReady,  //add
  USBD_Composite_DataIn,
  USBD_Composite_DataOut,
  NULL,
  NULL,
  NULL,
  NULL,
  USBD_Composite_GetFSCfgDesc,
  NULL,
  USBD_Composite_GetDeviceQualifierDescriptor,
};



/* USB composite device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN uint8_t USBD_Composite_CfgFSDesc[USBD_COMPOSITE_DESC_SIZE]  __ALIGN_END =
{
  /* 配置描述符 */
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USBD_COMPOSITE_DESC_SIZE,  
  0x00,
  USBD_MAX_NUM_INTERFACES ,  /* bNumInterfaces: */
  0x01,   /* bConfigurationValue: 0 配置的值 */
  0x00,   /* iConfiguration: 00 字符串索引 */
  0x80,   /* bmAttributes:no-bus powered and Dissupport Remote Wake-up*/
  0x32,   /* MaxPower 100 mA  */


  /****************************HID************************************/
  /* Interface Association Descriptor */
  USBD_IAD_DESC_SIZE,                        // bLength IAD描述符大小
  USBD_IAD_DESCRIPTOR_TYPE,                  // bDescriptorType IAD描述符类型
  0x00,                                      // bFirstInterface 接口描述符是在总的配置描述符中的第几个从0开始数
  0x01,                                      // bInterfaceCount 接口描述符数量
  0x03,                                      // bFunctionClass  设备符中的bDeviceClass
  0x00,                                      // bFunctionSubClass  设备符中的bDeviceSubClass
  0x00,                                      // bInterfaceProtocol 设备符中的bDeviceProtocol
  0x00,

  /********************  HID interface ********************/
  /************** Descriptor of Custom HID interface ****************/
  /* 09 */
  0x09,                   /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  USBD_HID_INTERFACE,     /*bInterfaceNumber: Number of Interface 接口编号 0 */
  0x00,                   /*bAlternateSetting: Alternate setting  备用接口 */
  0x01,                   /*bNumEndpoints 使用的端点数 1 */
  0x03,                   /*bInterfaceClass: HID*/
  0x00,                   /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x00,                   /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,                      /*iInterface: Index of string descriptor*/
  
  /******************** Descriptor of Custom HID ********************/
  /* 18 */
  0x09,                   /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE,    /*bDescriptorType: HID*/
  0x00,                   /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,                   /*bCountryCode: Hardware target country*/
  0x01,                   /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,                   /*bDescriptorType*/
  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of TouchScreen endpoint ********************/
  /* 27 */
  0x07,                   /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  HID_EPIN_ADDR,          /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,                   /*bmAttributes: Interrupt endpoint*/
  HID_EPIN_SIZE,          /*wMaxPacketSize: 16 Byte max */
  0x00,
  HID_FS_BINTERVAL,       /*bInterval: Polling Interval */
  /* 34 */
  
  /****************************CDC************************************/
  /* IAD描述符 */
  /* Interface Association Descriptor */
  USBD_IAD_DESC_SIZE,               // bLength
  USBD_IAD_DESCRIPTOR_TYPE,         // bDescriptorType
  0x01,                             // bFirstInterface 接口描述符是在总的配置描述符中的第几个从0开始数 1
  0x02,                             // bInterfaceCount 接口描述符数量 2
  0x02,                             // bFunctionClass     CDC Control
  0x02,                             // bFunctionSubClass  Abstract Control Model
  0x01,                             // bInterfaceProtocol  AT Commands: V.250 etc
  0x00,                             // iFunction
  
  /* CDC命令接口描述符 */
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size 长度 */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface 接口编号0x04 */
  /* Interface descriptor type */
  USBD_CDC_CMD_INTERFACE,   /* bInterfaceNumber: Number of Interface 接口编号，第一个接口编号为1 */
  0x00,   /* bAlternateSetting: Alternate setting 接口备用编号 0 */
  0x01,   /* bNumEndpoints: One endpoints used 非0端点的数目 1 cdc接口只使用了一个中断输入端点 */
  0x02,   /* bInterfaceClass: Communication Interface Class 接口所使用的类0x02 */
  0x02,   /* bInterfaceSubClass: Abstract Control Model 接口所使用的子类0x02 */
  0x01,   /* bInterfaceProtocol: Common AT commands 使用AT命令协议 */
  0x00,   /* iInterface: 接口字符串索引值 0表示没有 */

  /* 类特殊接口描述符--功能描述符 用来描述接口的功能 */
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size 描述符长度为5字节 */
  0x24,   /* bDescriptorType: CS_INTERFACE 描述符类型为类特殊接口CS_INTERFACE*/
  0x00,   /* bDescriptorSubtype: Header Func Desc 子类为 Header Func Desc，编号0x00 */
  0x10,   /* bcdCDC: spec release number CDC版本 */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE 描述符类型为类特殊接口CS_INTERFACE*/
  0x01,   /* bDescriptorSubtype: Call Management Func Desc 子类为Call Management Func Desc 编号0x01*/
  0x00,   /* bmCapabilities: D0+D1 设备自己不管理call management */
  0x01,   /* bDataInterface: 1 有一个数据类接口用作call management */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE 描述符类型为类特殊接口CS_INTERFACE*/
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc 子类为Abstract Control Management desc编号0x02*/
  0x02,   /* bmCapabilities 支持Set_Control_Line_State、Get_Line_Coding请求和Serial_State通知*/

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE 描述符类型为类特殊接口CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc 子类为Union func desc 编号0x06*/
  USBD_CDC_CMD_INTERFACE,    /* bMasterInterface: Communication class interface 编号为1的CDC接口 */
  USBD_CDC_DATA_INTERFACE,   /* bSlaveInterface0: Data Class Interface 编号为2的数据类接口 */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   			/* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),    /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_FS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

	/* 数据类接口的接口描述符 */
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size 接口描述符长度9字节*/
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: 接口描述符的编号0x04*/
  USBD_CDC_DATA_INTERFACE,   /* bInterfaceNumber: Number of Interface 接口的编号为2*/
  0x00,   /* bAlternateSetting: Alternate setting 该接口的备用编号为0 */
  0x02,   /* bNumEndpoints: Two endpoints used 非0端点的数据 设备需要使用一对批量端点，设置为2*/
  0x0A,   /* bInterfaceClass: CDC 该接口所使用的类 数据类接口代码为0x0A */
  0x00,   /* bInterfaceSubClass: 接口所使用的子类为0*/
  0x00,   /* bInterfaceProtocol: 接口所使用的协议为0*/
  0x00,   /* iInterface:  接口的字符串索引值，0表示没有*/

	/* 输出端点的端点描述符 */
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size 端点描述符长度7字节 */
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType: Endpoint 端点描述符编号为0x05 */
  CDC_OUT_EP,                           /* bEndpointAddress 端点的地址0x02 D7为方向*/
  0x02,                                 /* bmAttributes: Bulk 批量传输*/
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: 端点的最大包长 512字节*/
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                                 /* bInterval: ignore for Bulk transfer 端点查询时间，对批量端点无效 */

	/* 输入端点的端点描述符 */
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType: Endpoint 端点描述符编号为0x05*/
  CDC_IN_EP,                            /* bEndpointAddress 端点的地址0x82 D7为方向*/
  0x02,                                 /* bmAttributes: Bulk 批量传输*/
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: 端点的最大包长 512字节*/
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00                                /* bInterval: ignore for Bulk transfer 端点查询时间，对批量端点无效*/
};

/* USB 设备限定符描述符 */
/* USB Standard Device Descriptor */ 
__ALIGN_BEGIN  uint8_t USBD_Composite_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
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

static uint8_t  USBD_Composite_Init (USBD_HandleTypeDef *pdev,
                            uint8_t cfgidx)
{
  uint8_t res = 0;

  pdev->pUserData =  (void*)&USBD_Interface_fops_FS;
  res +=  USBD_CDC.Init(pdev,cfgidx);
  //pCDCData = pdev->pClassData;
  /* TODO */
  pdev->pUserData = NULL;
  res +=  USBD_HID.Init(pdev,cfgidx);
  //pHIDData = pdev->pClassData;
  return res;
}


static uint8_t  USBD_Composite_DeInit (USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx)
{
  uint8_t res = 0;
	pdev->pClassData = &pCDCData;
	pdev->pUserData = &USBD_Interface_fops_FS;
	res +=  USBD_CDC.DeInit(pdev,cfgidx);

	pdev->pClassData = &pHIDData;
  /* TODO */
	pdev->pUserData = NULL;
	res +=  USBD_HID.DeInit(pdev,cfgidx);

	return res;
}


static uint8_t  USBD_Composite_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	pdev->pClassData = &pCDCData;
	pdev->pUserData = &USBD_Interface_fops_FS;
  return USBD_CDC.EP0_RxReady(pdev);
}


/**
* @brief  USBD_Composite_Setup
*         Handle the Composite requests
* @param  pdev: device instance
* @param  req: USB request
* @retval status
*/
static uint8_t  USBD_Composite_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch (req->bmRequest & USB_REQ_RECIPIENT_MASK)
  {
   case USB_REQ_RECIPIENT_INTERFACE:
     switch(req->wIndex)
      {
         case USBD_CDC_DATA_INTERFACE:
         case USBD_CDC_CMD_INTERFACE:
           pdev->pClassData = &pCDCData;
           pdev->pUserData =  &USBD_Interface_fops_FS;
           return(USBD_CDC.Setup(pdev, req));

         case USBD_HID_INTERFACE:
           pdev->pClassData = &pHIDData;
           /* TODO */
           pdev->pUserData =  NULL;
           return(USBD_HID.Setup (pdev, req));

         default:
            break;
     }
     break;

   case USB_REQ_RECIPIENT_ENDPOINT:
     switch(req->wIndex)
     {

         case CDC_IN_EP:
         case CDC_OUT_EP:
         case CDC_CMD_EP:
           pdev->pClassData = &pCDCData;
           pdev->pUserData =  &USBD_Interface_fops_FS;
           return(USBD_CDC.Setup(pdev, req));

         case HID_EPIN_ADDR:
//         case HID_EPOUT_ADDR:
           pdev->pClassData = &pHIDData;
           /* TODO */
           pdev->pUserData =  NULL;
           return(USBD_HID.Setup (pdev, req));

         default:
            break;
     }
     break;
  }
  return USBD_OK;
}

/**
* @brief  USBD_Composite_DataIn
*         handle data IN Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
static uint8_t  USBD_Composite_DataIn (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{
  switch(epnum)
  {
      case CDC_INDATA_NUM:
         pdev->pUserData =  &USBD_Interface_fops_FS;
         pdev->pClassData = &pCDCData;
         return(USBD_CDC.DataIn(pdev,epnum));
      case HID_INDATA_NUM:
         /* TODO */
         pdev->pUserData = NULL;
         pdev->pClassData = &pHIDData;
         return(USBD_HID.DataIn(pdev,epnum));
      default:
         break;
  }
  return USBD_FAIL;
}

/**
* @brief  USBD_Composite_DataOut
*         handle data OUT Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t  USBD_Composite_DataOut (USBD_HandleTypeDef *pdev,
                               uint8_t epnum)
{
  switch(epnum)
  {
      case CDC_OUTDATA_NUM:
      case CDC_OUTCMD_NUM:
        pdev->pClassData = &pCDCData;
        pdev->pUserData =  &USBD_Interface_fops_FS;
        return(USBD_CDC.DataOut(pdev,epnum));

      default:
         break;
  }
  return USBD_FAIL;
}

/**
* @brief  USBD_Composite_GetHSCfgDesc
*         return configuration descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_Composite_GetFSCfgDesc (uint16_t *length)
{
   *length = sizeof (USBD_Composite_CfgFSDesc);
   return USBD_Composite_CfgFSDesc;
}


/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_Composite_DeviceQualifierDesc);
  return USBD_Composite_DeviceQualifierDesc;
}
