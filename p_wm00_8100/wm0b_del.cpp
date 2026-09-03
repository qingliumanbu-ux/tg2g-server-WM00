/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         zcx
Version:		1.0
Date:			2016-09-27
Description:	物料调拨（流转）电文号配置信息删除
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm0b.h"

//函数申明

/*<remark>=========================================================
///<summary>
///物料调拨（流转）电文号配置信息删除
///<para>
///2.排序方式：FROM_STOCK_NO
///</para>
///<para>数据库表：TWM0B 物料调拨（流转）电文号配置表；
///<returns>删除传入的物料调拨（流转）电文号配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm0b_del);

int f_wm0b_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM0B twm0b(conn);
	CModel twm0b = CModel("TWM0B");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twm0b.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm0b["FROM_STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "物料调拨（流转）电文号配置信息不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM0B WHERE FROM_STOCK_NO =@from_stockNo ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("from_stockNo", twm0b["FROM_STOCK_NO"].ToString());
			Count = cmd_inq.ExecuteScalar();

			if (Count < 1)
			{
				sprintf(s.msg, "该应用业务不存在，无需删除。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm0b.Delete("FROM_STOCK_NO");
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