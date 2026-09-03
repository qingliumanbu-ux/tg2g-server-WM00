/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:		Lsy
Version:     1.0
Date:
Description:  
**************************************************/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

/*<remark>=========================================================
/// <summary>
///  代码值集查询
/// <para>查询库区号、库区描述。
/// </para>
/// <para>数据库表：(库号定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：库区号、库区描述</returns>
===========================================================</remark>*/
// Service 入口
BM2F_ENTERACE(wm00_code)

int f_wm00_code(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	
	/* 业务变量 */
	CString s_userid("");

	/* 数据库SQL操作字符串 */
	CString sqlstr(" ");
	CString code_class(" ");
	/* 实体类定义 */	 

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;
		
		code_class = bcls_rec->Tables[0].Rows[0]["CODE_CLASS"].ToString().Trim();

		/* ***** 打印输入参数 ***** */
		Log::Debug("", __FUNCTION__, "传入参数 code_class = [{0}]", code_class);
		
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
		default:					// 所有数据库适用，通用SQL语句
			sqlstr = " select CODE,CODE_DESC_1_CONTENT from tep0002 t where CODE_CLASS = @code_class " ;
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("code_class", code_class);

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