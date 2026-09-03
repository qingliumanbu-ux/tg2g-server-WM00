/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:			QL
Version:		1.0
Date:			2016-06-30
Description:	实物存货信息删除
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twmb1.h"

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//函数申明

/*<remark>=========================================================
///<summary>
///库区定义信息删除
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWMB1 盘库实物物料表；
///<returns>实物存货信息删除</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmc1_del);

int f_wmc1_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	CModel twmb1 = CModel("TWMB1");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */



	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twmb1.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twmb1["STOCK_NO"].ToString() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twmb1["MAT_NO"].ToString() == "")
			{
				sprintf(s.msg, "材料号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			sqlstr = "SELECT COUNT(1) FROM TWMB1 WHERE STOCK_NO =@stock_no ";
			Log::Trace("", "", "sqlstr：{0}", sqlstr);
		

		   if (twmb1.QueryCount("STOCK_NO") < 1)
		   {
			   sprintf(s.msg, "该库区该库位不存在，无需删除。");
			   throw CApplicationException(-1, s.msg, log.Location);
		   }
		/*if (Count < 1)
		{
		sprintf(s.msg, "该库区该库位不存在，无需删除。");
		throw CApplicationException(-1, s.msg, log.Location);
		}*/

		   twmb1.Delete("STOCK_NO");
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