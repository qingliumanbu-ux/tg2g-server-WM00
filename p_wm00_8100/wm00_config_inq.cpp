/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      LQ
Version:     1.0
Date:        XXXX
Description: WM00 低代码开发配置的inq查询后台,如无特殊逻辑，可以通用。
**************************************************/

//框架头文件

#include "stdafx.h"
#include "./Be2UserModel/SI/CFormDevConfig.h"

/*<remark>=========================================================
/// <summary>
/// WM00  低代码开发配置的inq查询后台
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//外部函数声明

BM2F_ENTERACE(wm00_config_inq)


int f_wm00_config_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	int doFlag = 0;

	try
	{
		doFlag = BE2::CFormDevConfig::QueryUtility(bcls_rec, bcls_ret, conn);
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = -1;
		doFlag = -1;
	}
	return doFlag;
}

