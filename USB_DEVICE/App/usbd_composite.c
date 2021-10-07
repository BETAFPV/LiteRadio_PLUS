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
  /* ���������� */
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USBD_COMPOSITE_DESC_SIZE,  
  0x00,
  USBD_MAX_NUM_INTERFACES ,  /* bNumInterfaces: */
  0x01,   /* bConfigurationValue: 0 ���õ�ֵ */
  0x00,   /* iConfiguration: 00 �ַ������� */
  0x80,   /* bmAttributes:no-bus powered and Dissupport Remote Wake-up*/
  0x32,   /* MaxPower 100 mA  */


  /****************************HID************************************/
  /* Interface Association Descriptor */
  USBD_IAD_DESC_SIZE,                        // bLength IAD��������С
  USBD_IAD_DESCRIPTOR_TYPE,                  // bDescriptorType IAD����������
  0x00,                                      // bFirstInterface �ӿ������������ܵ������������еĵڼ�����0��ʼ��
  0x01,                                      // bInterfaceCount �ӿ�����������
  0x03,                                      // bFunctionClass  �豸���е�bDeviceClass
  0x00,                                      // bFunctionSubClass  �豸���е�bDeviceSubClass
  0x00,                                      // bInterfaceProtocol �豸���е�bDeviceProtocol
  0x00,

  /********************  HID interface ********************/
  /************** Descriptor of Custom HID interface ****************/
  /* 09 */
  0x09,                   /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  USBD_HID_INTERFACE,     /*bInterfaceNumber: Number of Interface �ӿڱ�� 0 */
  0x00,                   /*bAlternateSetting: Alternate setting  ���ýӿ� */
  0x01,                   /*bNumEndpoints ʹ�õĶ˵��� 1 */
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
  /* IAD������ */
  /* Interface Association Descriptor */
  USBD_IAD_DESC_SIZE,               // bLength
  USBD_IAD_DESCRIPTOR_TYPE,         // bDescriptorType
  0x01,                             // bFirstInterface �ӿ������������ܵ������������еĵڼ�����0��ʼ�� 1
  0x02,                             // bInterfaceCount �ӿ����������� 2
  0x02,                             // bFunctionClass     CDC Control
  0x02,                             // bFunctionSubClass  Abstract Control Model
  0x01,                             // bInterfaceProtocol  AT Commands: V.250 etc
  0x00,                             // iFunction
  
  /* CDC����ӿ������� */
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size ���� */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface �ӿڱ��0x04 */
  /* Interface descriptor type */
  USBD_CDC_CMD_INTERFACE,   /* bInterfaceNumber: Number of Interface �ӿڱ�ţ���һ���ӿڱ��Ϊ1 */
  0x00,   /* bAlternateSetting: Alternate setting �ӿڱ��ñ�� 0 */
  0x01,   /* bNumEndpoints: One endpoints used ��0�˵����Ŀ 1 cdc�ӿ�ֻʹ����һ���ж�����˵� */
  0x02,   /* bInterfaceClass: Communication Interface Class �ӿ���ʹ�õ���0x02 */
  0x02,   /* bInterfaceSubClass: Abstract Control Model �ӿ���ʹ�õ�����0x02 */
  0x01,   /* bInterfaceProtocol: Common AT commands ʹ��AT����Э�� */
  0x00,   /* iInterface: �ӿ��ַ�������ֵ 0��ʾû�� */

  /* ������ӿ�������--���������� ���������ӿڵĹ��� */
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size ����������Ϊ5�ֽ� */
  0x24,   /* bDescriptorType: CS_INTERFACE ����������Ϊ������ӿ�CS_INTERFACE*/
  0x00,   /* bDescriptorSubtype: Header Func Desc ����Ϊ Header Func Desc�����0x00 */
  0x10,   /* bcdCDC: spec release number CDC�汾 */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE ����������Ϊ������ӿ�CS_INTERFACE*/
  0x01,   /* bDescriptorSubtype: Call Management Func Desc ����ΪCall Management Func Desc ���0x01*/
  0x00,   /* bmCapabilities: D0+D1 �豸�Լ�������call management */
  0x01,   /* bDataInterface: 1 ��һ��������ӿ�����call management */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE ����������Ϊ������ӿ�CS_INTERFACE*/
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc ����ΪAbstract Control Management desc���0x02*/
  0x02,   /* bmCapabilities ֧��Set_Control_Line_State��Get_Line_Coding�����Serial_State֪ͨ*/

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE ����������Ϊ������ӿ�CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc ����ΪUnion func desc ���0x06*/
  USBD_CDC_CMD_INTERFACE,    /* bMasterInterface: Communication class interface ���Ϊ1��CDC�ӿ� */
  USBD_CDC_DATA_INTERFACE,   /* bSlaveInterface0: Data Class Interface ���Ϊ2��������ӿ� */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   			/* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),    /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_FS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

	/* ������ӿڵĽӿ������� */
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size �ӿ�����������9�ֽ�*/
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: �ӿ��������ı��0x04*/
  USBD_CDC_DATA_INTERFACE,   /* bInterfaceNumber: Number of Interface �ӿڵı��Ϊ2*/
  0x00,   /* bAlternateSetting: Alternate setting �ýӿڵı��ñ��Ϊ0 */
  0x02,   /* bNumEndpoints: Two endpoints used ��0�˵������ �豸��Ҫʹ��һ�������˵㣬����Ϊ2*/
  0x0A,   /* bInterfaceClass: CDC �ýӿ���ʹ�õ��� ������ӿڴ���Ϊ0x0A */
  0x00,   /* bInterfaceSubClass: �ӿ���ʹ�õ�����Ϊ0*/
  0x00,   /* bInterfaceProtocol: �ӿ���ʹ�õ�Э��Ϊ0*/
  0x00,   /* iInterface:  �ӿڵ��ַ�������ֵ��0��ʾû��*/

	/* ����˵�Ķ˵������� */
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size �˵�����������7�ֽ� */
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType: Endpoint �˵����������Ϊ0x05 */
  CDC_OUT_EP,                           /* bEndpointAddress �˵�ĵ�ַ0x02 D7Ϊ����*/
  0x02,                                 /* bmAttributes: Bulk ��������*/
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: �˵�������� 512�ֽ�*/
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00,                                 /* bInterval: ignore for Bulk transfer �˵��ѯʱ�䣬�������˵���Ч */

	/* ����˵�Ķ˵������� */
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType: Endpoint �˵����������Ϊ0x05*/
  CDC_IN_EP,                            /* bEndpointAddress �˵�ĵ�ַ0x82 D7Ϊ����*/
  0x02,                                 /* bmAttributes: Bulk ��������*/
  LOBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: �˵�������� 512�ֽ�*/
  HIBYTE(CDC_DATA_HS_MAX_PACKET_SIZE),
  0x00                                /* bInterval: ignore for Bulk transfer �˵��ѯʱ�䣬�������˵���Ч*/
};

/* USB �豸�޶��������� */
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
