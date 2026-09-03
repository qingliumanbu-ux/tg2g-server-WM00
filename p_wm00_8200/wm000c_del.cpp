/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         zcx
Version:		1.0
Date:			2016-09-27
Description:	业务步骤配置信息删除
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm000c.h"


//函数申明

/*<remark>=========================================================
///<summary>
///业务步骤配置信息删除
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWM000C 业务步骤配置信息表；
///<returns>删除传入的业务步骤配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm000c_del);

int f_wm000c_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM000C twm000c(conn);
	CModel twm000c = CModel("TWM000C");

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

			twm000c.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", __FUNCTION__, "i[{0}]", i);
			Log::Trace("", __FUNCTION__, "twm000c.STOCK_NO[{0}]", twm000c["STOCK_NO"].ToString());
			Log::Trace("", __FUNCTION__, "twm000c.STOCK_OPER_ORDER[{0}]", twm000c["STOCK_OPER_ORDER"].ToString());
			Log::Trace("", __FUNCTION__, "twm000c.UNIT_CODE[{0}]", twm000c["UNIT_CODE"].ToString());
			Log::Trace("", __FUNCTION__, "twm000c.SEQ_NO[{0}]", twm000c["SEQ_NO"].ToString());


			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM000C WHERE STOCK_NO =@stock_no AND STOCK_OPER_ORDER = @stock_oper_order AND UNIT_CODE = @unit_code AND SEQ_NO = @seq_no ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_no", twm000c["STOCK_NO"].ToString());
			cmd_inq.Parameters.Set("stock_oper_order", twm000c["STOCK_OPER_ORDER"].ToString());
			cmd_inq.Parameters.Set("unit_code", twm000c["UNIT_CODE"].ToString());
			cmd_inq.Parameters.Set("seq_no", twm000c["SEQ_NO"].ToDecimal());
			Count = cmd_inq.ExecuteScalar();

			if (Count < 1)
			{
				sprintf(s.msg, "业务步骤配置信息不存在，无需删除。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm000c.Delete("STOCK_NO,STOCK_OPER_ORDER,UNIT_CODE,SEQ_NO");
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