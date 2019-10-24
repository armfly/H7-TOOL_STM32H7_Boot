/*
*********************************************************************************************************
*
*	ģ������ : DM9162����
*	�ļ����� : dm9162.c
*	��    �� : V1.0
*	˵    �� : ���PHY�����ṹ��ͨ��������ͬ����Ҫ��ע��SMI���߶�д��������ʹ�á�
*		�������ṹ���ŵ��������ļ������޸ľͿ�����ֲ����ͬ��CPUƽ̨��
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2018-10-20 armfly  �װ�.
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "dm9162.h"

#define DM9162_SW_RESET_TO    ((uint32_t)500U)
#define DM9162_INIT_TO        ((uint32_t)2000U)

/*
*********************************************************************************************************
*	�� �� ��: DM9162_RegisterBusIO
*	����˵��: ע����������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t DM9162_RegisterBusIO(dm9162_Object_t *pObj, dm9162_IOCtx_t *ioctx)
{
  if(!pObj || !ioctx->ReadReg || !ioctx->WriteReg || !ioctx->GetTick)
  {
    return DM9162_STATUS_ERROR;
  }
  
  pObj->IO.Init = ioctx->Init;
  pObj->IO.DeInit = ioctx->DeInit;
  pObj->IO.ReadReg = ioctx->ReadReg;
  pObj->IO.WriteReg = ioctx->WriteReg;
  pObj->IO.GetTick = ioctx->GetTick;
  
  return DM9162_STATUS_OK;
}

/*
*********************************************************************************************************
*	�� �� ��: DM9162_Init
*	����˵��: Initialize the dm9162 and configure the needed hardware resources
*	��    ��: ��
*	�� �� ֵ: ״̬
*			DM9162_STATUS_OK  if OK
*			DM9162_STATUS_ADDRESS_ERROR if cannot find device address
*			DM9162_STATUS_READ_ERROR if connot read register
*			DM9162_STATUS_WRITE_ERROR if connot write to register
*			DM9162_STATUS_RESET_TIMEOUT if cannot perform a software reset
*********************************************************************************************************
*/
int32_t DM9162_Init(dm9162_Object_t *pObj)
{
   uint32_t tickstart = 0, regvalue = 0;
   int32_t status = DM9162_STATUS_OK;
   
   if(pObj->Is_Initialized == 0)
   {
     if(pObj->IO.Init != 0)
     {
       /* GPIO and Clocks initialization */
       pObj->IO.Init();
     }
   
     /* for later check */
     pObj->DevAddr = DM9162_PHY_ADDR;
     
     /* if device address is matched */
     if(status == DM9162_STATUS_OK)
     {
       /* set a software reset  */
       if(pObj->IO.WriteReg(pObj->DevAddr, DM9162_BCR, DM9162_BCR_SOFT_RESET) >= 0)
       { 
         /* get software reset status */
         if(pObj->IO.ReadReg(pObj->DevAddr, DM9162_BCR, &regvalue) >= 0)
         { 
           tickstart = pObj->IO.GetTick();
           
           /* wait until software reset is done or timeout occured  */
           while(regvalue & DM9162_BCR_SOFT_RESET)
           {
             if((pObj->IO.GetTick() - tickstart) <= DM9162_SW_RESET_TO)
             {
               if(pObj->IO.ReadReg(pObj->DevAddr, DM9162_BCR, &regvalue) < 0)
               { 
                 status = DM9162_STATUS_READ_ERROR;
                 break;
               }
             }
             else
             {
               status = DM9162_STATUS_RESET_TIMEOUT;
             }
           } 
         }
         else
         {
           status = DM9162_STATUS_READ_ERROR;
         }
       }
       else
       {
         status = DM9162_STATUS_WRITE_ERROR;
       }
     }
   }
      
   if(status == DM9162_STATUS_OK)
   {
     tickstart =  pObj->IO.GetTick();
     
     /* Wait for 2s to perform initialization */
     while((pObj->IO.GetTick() - tickstart) <= DM9162_INIT_TO)
     {
     }
     pObj->Is_Initialized = 1;
   }
   
   return status;
}

/*
*********************************************************************************************************
*	�� �� ��: DM9162_GetLinkState
*	����˵��: Get the link state of DM9162 device.
*	��    ��: ��
*	�� �� ֵ: pLinkState: Pointer to link state
*			DM9162_STATUS_LINK_DOWN  if link is down
*			DM9162_STATUS_AUTONEGO_NOTDONE if Auto nego not completed 
*			DM9162_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
*			DM9162_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
*			DM9162_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
*			DM9162_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD       
*			DM9162_STATUS_READ_ERROR if connot read register
*			DM9162_STATUS_WRITE_ERROR if connot write to register
*********************************************************************************************************
*/
int32_t DM9162_GetLinkState(dm9162_Object_t *pObj)
{
  uint32_t readval = 0;
  
  /* Read Status register  */
  if(pObj->IO.ReadReg(pObj->DevAddr, DM9162_BSR, &readval) < 0)
  {
    return DM9162_STATUS_READ_ERROR;
  }
  
  /* Read Status register again */
  if(pObj->IO.ReadReg(pObj->DevAddr, DM9162_BSR, &readval) < 0)
  {
    return DM9162_STATUS_READ_ERROR;
  }
  
  if((readval & DM9162_BSR_LINK_STATUS) == 0)
  {
    /* Return Link Down status */
    return DM9162_STATUS_LINK_DOWN;    
  }
  
  /* Check Auto negotiaition */
  if(pObj->IO.ReadReg(pObj->DevAddr, DM9162_BCR, &readval) < 0)
  {
    return DM9162_STATUS_READ_ERROR;
  }
  
  if((readval & DM9162_BCR_AUTONEGO_EN) != DM9162_BCR_AUTONEGO_EN)
  {
    if(((readval & DM9162_BCR_SPEED_SELECT) == DM9162_BCR_SPEED_SELECT) && ((readval & DM9162_BCR_DUPLEX_MODE) == DM9162_BCR_DUPLEX_MODE)) 
    {
      return DM9162_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & DM9162_BCR_SPEED_SELECT) == DM9162_BCR_SPEED_SELECT)
    {
      return DM9162_STATUS_100MBITS_HALFDUPLEX;
    }        
    else if ((readval & DM9162_BCR_DUPLEX_MODE) == DM9162_BCR_DUPLEX_MODE)
    {
      return DM9162_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return DM9162_STATUS_10MBITS_HALFDUPLEX;
    }  		
  }
  else /* Auto Nego enabled */
  {
    if(pObj->IO.ReadReg(pObj->DevAddr, DM9162_PHYSCSR, &readval) < 0)
    {
      return DM9162_STATUS_READ_ERROR;
    }
    
    /* Check if auto nego not done */
    if((readval & DM9162_PHYSCSR_AUTONEGO_MASK) != DM9162_PHYSCSR_AUTONEGO_DONE)
    {
      return DM9162_STATUS_AUTONEGO_NOTDONE;
    }
    
    if((readval & DM9162_PHYSCSR_HCDSPEEDMASK) == DM9162_PHYSCSR_100BTX_FD)
    {
      return DM9162_STATUS_100MBITS_FULLDUPLEX;
    }
    else if ((readval & DM9162_PHYSCSR_HCDSPEEDMASK) == DM9162_PHYSCSR_100BTX_HD)
    {
      return DM9162_STATUS_100MBITS_HALFDUPLEX;
    }
    else if ((readval & DM9162_PHYSCSR_HCDSPEEDMASK) == DM9162_PHYSCSR_10BT_FD)
    {
      return DM9162_STATUS_10MBITS_FULLDUPLEX;
    }
    else
    {
      return DM9162_STATUS_10MBITS_HALFDUPLEX;
    }				
  }
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
