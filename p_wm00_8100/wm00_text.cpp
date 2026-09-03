/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:
Description:
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(wm00_text)

BM2_FUNCTION_IMPORT
//int f_auto_sail(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

int f_wm00_text(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	CString sqlstr = "";
	int doFlag = 0;
	try
	{
		//if (bcls_rec->Tables["FLAG"].Rows.get_Count() == 0)
		if (1==1)
		{
			bcls_rec->Tables[0].set_TableName("AUTO_INFO_IN");
			bcls_rec->Tables["AUTO_INFO_IN"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["AUTO_INFO_IN"].Columns.Add(DT_STRING, "STOCK_NO");
			bcls_rec->Tables["AUTO_INFO_IN"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables[0].Rows.Add();
			bcls_rec->Tables[0].Rows[0]["MAT_NO"] = "TEST160822576";
			bcls_rec->Tables[0].Rows[0]["STOCK_NO"] = "P31";
			//doFlag = f_auto_sail(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			return doFlag;
		}
		else if(bcls_rec->Tables["FLAG"].Rows[0]["FLAG"].ToString() == "F2")
		{
			return doFlag;
		}
		else if (bcls_rec->Tables["FLAG"].Rows[0]["FLAG"].ToString() == "F3")
		{
			return doFlag;
		}
		else if (bcls_rec->Tables["FLAG"].Rows[0]["FLAG"].ToString() == "F4")
		{
			return doFlag;
		}
		else if (bcls_rec->Tables["FLAG"].Rows[0]["FLAG"].ToString() == "F5")
		{
			return doFlag;
		}



	}
	catch (CDbException& ex)					//捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)	//捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
}