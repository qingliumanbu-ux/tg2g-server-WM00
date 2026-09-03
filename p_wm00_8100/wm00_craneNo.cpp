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
///  吊车号查询
/// <para>查询吊车号。
/// </para>
/// <para>数据库表：(吊车定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：吊车号</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(wm00_craneNo)

int f_wm00_craneNo(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString stock_no("");
	CString hall_no("");
	CString crane_no("");
	CString crane_status("");

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
		if (bcls_rec->Tables[0].Columns.Contains("HALL_NO"))
		{
			hall_no = bcls_rec->Tables[0].Rows[0]["HALL_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CRANE_NO"))
		{
			crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CRANE_STATUS"))
		{
			crane_status = bcls_rec->Tables[0].Rows[0]["CRANE_STATUS"].ToString().Trim();
		}

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "传入参数STOCK_NO		= [{0}]", stock_no);
		Log::Trace("", __FUNCTION__, "传入参数HALL_NO		= [{0}]", hall_no);
		Log::Trace("", __FUNCTION__, "传入参数CRANE_NO		= [{0}]", crane_no);
		Log::Trace("", __FUNCTION__, "传入参数CRANE_STATUS	= [{0}]", crane_status);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句

			sqlstr = " select distinct CRANE_NO "
				     " from   twm06 "
					 " where  hall_no like @hall_no "
					 " and    crane_no like @crane_no "
				     " and    hall_no in ( "
					 "                      select distinct hall_no from twm03 "
					 "                      where  stock_no like @stock_no "
					 "                      and    stock_no in ( "
					 "											  select stock_no from twm0a "
			         "											  where  groupid in ( "
					 "																	select groupid from tesgroupmember "
			         "																	where memberid in ( "
					 "																						 select id from tesuserinfo where ename = @userid "
					 "																					  ) "
					 "																) "
					 "											  union "
					 "						                      select stock_no from twm01 where 'admin' = @userid "
					 "										   ) "
					 "                   ) "
					 ;
			if (crane_status.Trim() != "")
			{
				sqlstr += " and crane_status = @crane_status ";
			}

			sqlstr += " order by crane_no ";
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no+"%");
		cmd_inq.Parameters.Set("hall_no", hall_no + "%");
		cmd_inq.Parameters.Set("crane_no", crane_no + "%");
		cmd_inq.Parameters.Set("crane_status", crane_status);
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