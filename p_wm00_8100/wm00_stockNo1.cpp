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
BM2F_ENTERACE(wm00_stockNo1)

int f_wm00_stockNo1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	
	/* 业务变量 */
	CString s_userid("");
	CString hall_no(" ");
	/* 数据库SQL操作字符串 */
	CString sqlstr(" ");
	

	/* 实体类定义 */	 

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;
		
		bcls_ret->Tables[0].set_TableName("WM_DYN_SQL");
		bcls_ret->Tables["WM_DYN_SQL"].Columns.Add(DT_STRING, "CODE");
		bcls_ret->Tables["WM_DYN_SQL"].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");
		hall_no = bcls_rec->Tables[0].Rows[0]["HALL_NO"].ToString().Trim();
		/* ***** 打印输入参数 ***** */
		Log::Debug("", __FUNCTION__, "传入参数 USERID = [{0}]", s_userid);
		

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句

			sqlstr = " select t1.STOCK_NO, t2.STOCK_DESC "
					 " from   twm0a t1, twm03 t2 "
			         " where  t1.stock_no = t2.stock_no "
					 " and    t2.HALL_NO  =@hall_no "
					 " and    t1.groupid in ( "
					 "						   select groupid from TESGROUPMEMBER "
			         "                         where memberid in ( "
					 "											    select id from tesuserinfo where ename = @userid "
					 "										     ) "
					 "                      ) "
					 " union "
					 " select STOCK_NO, STOCK_DESC from twm01 where 'admin' = @userid "
					 ;

			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("hall_no", hall_no);
		cmd_inq.Parameters.Set("userid", s_userid);
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