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
///  库区号查询
/// <para>查询库区号、库区描述。
/// </para>
/// <para>数据库表：(库号定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：库区号、库区描述</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(wm00_max)
int f_wm00_max(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString s_userid("");
	CString v_stock_type_code = "";
	CString stock_place_no = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr(" ");

	/* 实体类定义 */

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;

		if (bcls_rec->Tables[0].Columns.Contains("STOCK_PLACE_NO"))
		{
			stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
		}

		/* ***** 打印输入参数 ***** */
		Log::Debug("", __FUNCTION__, "传入参数 USERID = [{0}]", s_userid);
		Log::Debug("", __FUNCTION__, "传入参数 stock_place_no = [{0}]", stock_place_no);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ROWNO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "LAYERNO");
		if (stock_place_no.Trim() != "")
		{
			bcls_ret->Tables[0].Rows.Add();
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:			// MS SQL Server数据库
			case DB_KIND_ORACLE:		// Oracle 数据库
			default:					// 所有数据库适用，通用SQL语句
				sqlstr =
					" select nvl(max(a.layerno),0) as layerno from twma2 a WHERE EXISTS (SELECT NULL FROM TMMSM01 b WHERE b.MAT_NO = a.MAT_NO ) AND  a.stock_place_no = @stock_place_no ";

				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_place_no", stock_place_no);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.ExecuteReader();
			//cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[0]["LAYERNO"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();

			sqlstr =
				" SELECT nvl(max(TO_NUMBER(a.rowno)),0) as rowno FROM twma2 a"
				" WHERE EXISTS (SELECT NULL FROM TMMSM01 b WHERE b.MAT_NO = a.MAT_NO ) AND  a.layerno=@layerno and a.stock_place_no = @stock_place_no and a.rowno !=''";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("layerno", bcls_ret->Tables[0].Rows[0]["LAYERNO"].ToDecimal() > 0 ? bcls_ret->Tables[0].Rows[0]["LAYERNO"].ToString() : "0" + bcls_ret->Tables[0].Rows[0]["LAYERNO"].ToString());
			cmd_inq.Parameters.Set("stock_place_no", stock_place_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				bcls_ret->Tables[0].Rows[0]["ROWNO"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
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

	return(doFlag);
}
