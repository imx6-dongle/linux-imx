/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#define _HAL_INTF_C_
#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <rtw_byteorder.h>

#include <hal_intf.h>

#ifdef CONFIG_SDIO_HCI
	#include <sdio_hal.h>
#elif defined(CONFIG_USB_HCI)
	#include <usb_hal.h>
#endif

void intf_chip_configure(_adapter *padapter)
{
	if(padapter->HalFunc.intf_chip_configure)
		padapter->HalFunc.intf_chip_configure(padapter);
}

void intf_read_chip_info(_adapter *padapter)
{
	if(padapter->HalFunc.read_adapter_info)
		padapter->HalFunc.read_adapter_info(padapter);
}

void intf_read_chip_version(_adapter *padapter)
{
	if(padapter->HalFunc.read_chip_version)
		padapter->HalFunc.read_chip_version(padapter);
}

void	rtw_dm_init(_adapter *padapter)
{
	if(padapter->HalFunc.dm_init)
		padapter->HalFunc.dm_init(padapter);
}

void	rtw_sw_led_init(_adapter *padapter)
{
	if(padapter->HalFunc.InitSwLeds)
		padapter->HalFunc.InitSwLeds(padapter);
}

void rtw_sw_led_deinit(_adapter *padapter)
{
	if(padapter->HalFunc.DeInitSwLeds)
		padapter->HalFunc.DeInitSwLeds(padapter);
}

uint	 rtw_hal_init(_adapter *padapter) 
{
	uint	status = _SUCCESS;

	if(padapter->hw_init_completed == _TRUE)
	{
		DBG_871X("rtw_hal_init: hw_init_completed == _TRUE\n");
		return status;
	}
#ifdef CONFIG_DEINIT_BEFORE_INIT
	status = padapter->HalFunc.hal_deinit(padapter);
	if(status != _SUCCESS){
		DBG_871X("rtw_hal_init: hal_deinit before hal_init FAIL !!\n");
		return status;
	}
#endif
	
#ifdef CONFIG_DUALMAC_CONCURRENT
	// before init mac0, driver must init mac1 first to avoid usb rx error.
	if((padapter->pbuddy_adapter != NULL) && (padapter->DualMacConcurrent == _TRUE)
		&& (padapter->adapter_type == PRIMARY_ADAPTER))
	{
		if(padapter->pbuddy_adapter->hw_init_completed == _TRUE)
		{
			DBG_871X("rtw_hal_init: pbuddy_adapter hw_init_completed == _TRUE\n");
		}
		else
		{
#ifdef CONFIG_DEINIT_BEFORE_INIT
			status = padapter->HalFunc.hal_deinit(padapter->pbuddy_adapter);
			if(status != _SUCCESS){
				DBG_871X("rtw_hal_init: hal_deinit before hal_init FAIL !!(pbuddy_adapter)\n");
				return status;
			}
#endif
			status = 	padapter->HalFunc.hal_init(padapter->pbuddy_adapter);
			if(status == _SUCCESS){
				padapter->pbuddy_adapter->hw_init_completed = _TRUE;
			}
			else{
			 	padapter->pbuddy_adapter->hw_init_completed = _FALSE;
				RT_TRACE(_module_hal_init_c_,_drv_err_,("rtw_hal_init: hal__init fail(pbuddy_adapter)\n"));
				return status;
			}
		}
	}
#else
	if(adapter_to_dvobj(padapter)->NumInterfaces == 2 && padapter->registrypriv.mac_phy_mode != 1)
	{
		if(padapter->pbuddy_adapter->hw_init_completed == _FALSE)
		{
#ifdef CONFIG_DEINIT_BEFORE_INIT
			status = padapter->HalFunc.hal_deinit(padapter->pbuddy_adapter);
			if(status != _SUCCESS){
				DBG_871X("rtw_hal_init: hal_deinit before hal_init FAIL !!(pbuddy_adapter)\n");
				return status;
			}
#endif
			status = padapter->HalFunc.hal_init(padapter->pbuddy_adapter);
			if(status == _SUCCESS){
				padapter->pbuddy_adapter->hw_init_completed = _TRUE;
			}
			else{
				padapter->pbuddy_adapter->hw_init_completed = _FALSE;
				RT_TRACE(_module_hal_init_c_,_drv_err_,("rtw_hal_init: hal__init fail for another interface\n"));
			}
		}
	}
#endif

	padapter->hw_init_completed=_FALSE;

	status = padapter->HalFunc.hal_init(padapter);

	if(status == _SUCCESS){
		padapter->hw_init_completed = _TRUE;
		
		if (padapter->registrypriv.notch_filter == 1)
			rtw_hal_notch_filter(padapter, 1);
		
		rtw_hal_reset_security_engine(padapter);
	}
	else{
	 	padapter->hw_init_completed = _FALSE;
		RT_TRACE(_module_hal_init_c_,_drv_err_,("rtw_hal_init: hal__init fail\n"));
	}

	RT_TRACE(_module_hal_init_c_,_drv_err_,("-rtl871x_hal_init:status=0x%x\n",status));

	return status;

}	

uint	 rtw_hal_deinit(_adapter *padapter)
{
	uint	status = _SUCCESS;
	
_func_enter_;

	status = padapter->HalFunc.hal_deinit(padapter);

	if(status == _SUCCESS){
		padapter->hw_init_completed = _FALSE;
	}
	else
	{
		RT_TRACE(_module_hal_init_c_,_drv_err_,("\n rtw_hal_deinit: hal_init fail\n"));
	}
	
_func_exit_;
	
	return status;
	
}

s32	rtw_hal_mgnt_xmit(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	s32 ret = _FAIL;
	if(padapter->HalFunc.mgnt_xmit)
		ret = padapter->HalFunc.mgnt_xmit(padapter, pmgntframe);
	return ret;
}

#ifdef DBG_CONFIG_ERROR_DETECT
void rtw_hal_sreset_init(_adapter *padapter)
{
	if(padapter->HalFunc.sreset_init_value)
		padapter->HalFunc.sreset_init_value(padapter); 
}

void rtw_hal_sreset_reset(_adapter *padapter)
{
	if(padapter->HalFunc.silentreset)
		padapter->HalFunc.silentreset(padapter);
}

void rtw_hal_sreset_reset_value(_adapter *padapter)
{
	if(padapter->HalFunc.sreset_reset_value)
		padapter->HalFunc.sreset_reset_value(padapter);
}

void rtw_hal_sreset_xmit_status_check(_adapter *padapter)
{
#ifdef CONFIG_CONCURRENT_MODE
	if (padapter->adapter_type != PRIMARY_ADAPTER)
		return;
#endif
	if(padapter->HalFunc.sreset_xmit_status_check)
		padapter->HalFunc.sreset_xmit_status_check(padapter);
}
void rtw_hal_sreset_linked_status_check(_adapter *padapter)
{
	if(padapter->HalFunc.sreset_linked_status_check)
		padapter->HalFunc.sreset_linked_status_check(padapter);
}
u8 rtw_hal_sreset_get_wifi_status(_adapter *padapter)
{
	u8 status = 0;
	if(padapter->HalFunc.sreset_get_wifi_status)
		status = padapter->HalFunc.sreset_get_wifi_status(padapter);
	return status;
}
#endif

void rtw_hal_notch_filter(_adapter * adapter, bool enable)
{
	if(adapter->HalFunc.hal_notch_filter)
		adapter->HalFunc.hal_notch_filter(adapter,enable);
}

void rtw_hal_reset_security_engine(_adapter * adapter)
{
	if(adapter->HalFunc.hal_reset_security_engine)
		adapter->HalFunc.hal_reset_security_engine(adapter);
}

s32 rtw_hal_c2h_handler(_adapter *adapter, struct c2h_evt_hdr *c2h_evt)
{
	s32 ret = _FAIL;
	if (adapter->HalFunc.c2h_handler)
		ret = adapter->HalFunc.c2h_handler(adapter, c2h_evt);
	return ret;
}

c2h_id_filter rtw_hal_c2h_id_filter_ccx(_adapter *adapter)
{
	return adapter->HalFunc.c2h_id_filter_ccx;
}

