/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author: 吴新
Version:     1.0
Date: 
Description:  
**************************************************/
#include "stdafx.h"
// Service 入口
BM2F_ENTERACE(wm00_in_stockNo)

int f_wm00_in_stockNo(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString s_userid(""), s_mat_no(" ");

	/* 数据库SQL操作字符串 */
	CString sqlstr(" "); 

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* ***** 获取输入参数 ***** */
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			s_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		}

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "传入参数 USERID = [{0}]", s_userid);
		Log::Trace("", __FUNCTION__, "传入参数 MAT_NO = [{0}]", s_mat_no);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句
			sqlstr =
				" SELECT Distinct t.STOCK_NO,t.STOCK_DESC"
				" FROM (SELECT STOCK_NO , ' ' AS STOCK_DESC FROM TWMA0 WHERE MAT_NO = @mat_no) st, TWM01 t "
				" WHERE st.stock_no = t.stock_no  "
				;
			break;
		}

		Log::Trace("", __FUNCTION__, "twma1.MAT_NO = [{0}]", s_mat_no);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("mat_no", s_mat_no);

		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["WM_DYN_SQL"], 0, -1);  //0,-1：非翻页查询
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