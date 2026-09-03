/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:
Description:
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
/// 机组号查询
/// <para>查询机组号。
/// </para>
/// <para>数据库表：(机组号定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：机组号</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(wm00_unit_code)

int f_wm00_unit_code(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString unit_code("");
	//CString mat_line_type("");

	/* 数据库SQL操作字符串 */
	CString sqlstr(" ");

	/* 实体类定义 */

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 获取前台传入参数
		if (bcls_rec->Tables[0].Columns.Contains("UNIT_CODE"))
		{
			unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString().Trim();
		}

		//if (bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
		//{
		//	mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		//}

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "传入参数UNIT_CODE		= [{0}]", unit_code);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句

			sqlstr = " select distinct UNIT_CODE "
				" from   twma0 "
				//" where MAT_LINE_TYPE=@mat_line_type"
				//" and stock_oper_order='2B' "
				;
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("unit_code", unit_code + "%");
		//cmd_inq.Parameters.Set("userid", s.userid);
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, -1);  //0,-1：非翻页查询
		cmd_inq.Close();
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

	return(doFlag);
}