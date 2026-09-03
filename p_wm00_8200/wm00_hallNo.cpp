/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:
Description:  
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
/// <summary>
///  跨号查询
/// <para>查询跨号。
/// </para>
/// <para>数据库表：(跨号定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：跨号</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(wm00_hallNo)

int f_wm00_hallNo(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString stock_no("");
	CString v_mat_line_type = "";
	CString v_mat_kind = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr(" ");

	/* 实体类定义 */	 

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 获取前台传入参数
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
		{
			stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
		{
			v_mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))
		{
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString();
		}

		//获取库区授权
		CString stock_no_auth = "' '";
		EIClass *bcls_auth = new EIClass;
		if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		{
			stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		}
		delete bcls_auth;

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "传入参数STOCK_NO		= [{0}]", stock_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句

			sqlstr = " select distinct HALL_NO "
				     " from   twm03 T1"
					 " where  stock_no like @stock_no "
					 " AND EXISTS (SELECT NULL FROM TWM01 T2"
					 " WHERE T1.STOCK_NO = T2.STOCK_NO"
					 " AND T2.MAT_LINE_TYPE LIKE @mat_line_type"
					 " AND T2.MAT_KIND LIKE @mat_kind)"
					 ;
			sqlstr +=
				" AND STOCK_NO IN (" + stock_no_auth + ")";
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no + "%");
		cmd_inq.Parameters.Set("mat_line_type", v_mat_line_type + "%");
		cmd_inq.Parameters.Set("mat_kind", v_mat_kind + "%");
		cmd_inq.Parameters.Set("userid", s.userid);
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