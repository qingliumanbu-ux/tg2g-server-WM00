/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         zcx
Version:		1.0
Date:			2016-09-27
Description:	事件与库区对应关系新增
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm000e.h"


//函数申明

/*<remark>=========================================================
///<summary>
///事件与库区对应关系新增
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWM000E 事件与库区对应关系表；
///<returns>新增传入的事件与库区对应关系信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm000e_ins);

int f_wm000e_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM000E twm000e(conn);
	CModel twm000e = CModel("TWM000E");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm000e.Reset();
			twm000e.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "twm000e.STOCK_NO[{0}]", twm000e["STOCK_NO"].ToString());
			Log::Trace("", __FUNCTION__, "i[{0}]", i);

			if (twm000e["STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm000e["EVENT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "事件标识不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm000e["UNIT_CODE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "机组代码不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if (twm000e.NEXT_UNIT_CODE == 0)
			//{
			//	sprintf(s.msg, "下道机组代码不能为空");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			twm000e["STOCK_NO"] = twm000e["STOCK_NO"].ToString().ToUpper();



			if (twm000e.QueryCount(" STOCK_NO, EVENT_ID, UNIT_CODE") > 0)
			{
				sprintf(s.msg, "该库区[%s]已存在，无需新增。", (const char*)twm000e["STOCK_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm000e["REC_CREATE_TIME"] = datetime;
			twm000e["REC_CREATOR"] = s.userid;
			twm000e.Insert();
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
	return doFlag;

}

