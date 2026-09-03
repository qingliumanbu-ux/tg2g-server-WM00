/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      LQ
Version:     1.0
Date:        XXXX
Description: WM00 低代码开发配置的加载service,如无特殊逻辑，可以通用。
**************************************************/

//框架头文件
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// WM00  低代码开发配置的加载service
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//引用头文件
#include "Be2UserModel/SI/CFormDevConfig.h"
//外部函数声明

BM2F_ENTERACE(wm00_getFormConfig)


int f_wm00_getFormConfig(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	//系统日志类定义
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";

	try
	{
		//获取画面布局配置信息
		if (BE2::CFormDevConfig::GetFormDevConfig(bcls_rec, bcls_ret, conn) != 0)
		{
			s.flag = -1;
			return -1;
		}
		//以下可以写画面加载时需设置的默认值

		///例子：
		{
			//////自定义默认值-获取登录用户所属部门
			//////增加自定义默认值参数名
			////if (!bcls_ret->Tables["BM2A_SYSTEM_INFO"].Columns.Contains("UserDeptCode"))
			////	bcls_ret->Tables["BM2A_SYSTEM_INFO"].Columns.Add(DT_STRING, "UserDeptCode");
			//// 
			//////获取默认值
			////CDataRow& drSystemInfo = bcls_ret->Tables["BM2A_SYSTEM_INFO"].Rows[0];
			////CString strsql = "SELECT DEPT_CODE FROM TABLE WHERE USER_ID = @USER_ID ";
			////paraList.Set("USER_ID", s.userid);
			////drSystemInfo["UserDeptCode"] = Db::QueryCString(strsql, paraList);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}

