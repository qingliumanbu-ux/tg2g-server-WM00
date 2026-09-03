/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	库区与出库运输方式对应关系信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
#include "Be2UserModel/SI/CFormDevConfig.h" 


BM2F_ENTERACE(wm000n_inq);

int f_wm000n_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	CModel twm04 = CModel("TWM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		
		s_userid = s.userid;
		CString columns_temp = "";
		/*twm04.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV") && bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim() != "")
		{
			sqlwhere += " AND EXISTS (SELECT NULL FROM TWM01 T2 WHERE TWM04.STOCK_NO = T2.STOCK_NO AND T2.FACTORY_DIV = @factory_div) ";
		}*/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT * FROM TWM000N WHERE 1=1 ";
			break;
		}
		sqlwhere += " ORDER BY STOCK_NO ASC,"
					" UNIT_CODE ASC, "
					" RULE_TYPE DESC, "
					" TRNP_MODE_CODE ASC, "
					" AIM_STOCK_NO ASC ";
		if (bcls_rec->Tables.Contains("QUERY_FILTER"))
		{
			BE2::CFormDevConfig::SetParameters(cmd_inq, sqlstr, bcls_rec->Tables["QUERY_FILTER"]);
		}
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		//bcls_ret->Tables.SetTableName(0, "TWM04SM");
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		
		cmd_inq.Close();


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