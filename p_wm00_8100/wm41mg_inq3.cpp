/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   吴新
Version:
Date:     2016-04-13
Description: 查询转库材料
**************************************************/



#include "stdafx.h" //框架头

// service入口
BM2F_ENTERACE(wm41mg_inq3)

int f_wm41mg_inq3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString  transfer_plan_no = "";
	CString  transfer_bill_no = "";

	/* 实体类定义 */
	CString sqlstr("");
	CString  sql("");              // 数据库SQL操作字符串
	CString sqlwhere("");
	CString sqlorderby("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{

		/* ***** 获取输入参数 ***** */
		transfer_plan_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"];
		transfer_bill_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_BILL_NO"];

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sql = CString(
				" SELECT A.* FROM TWM42 A "
				" WHERE A.TRANSFER_PLAN_NO = @transfer_plan_no  and a.TRANSFER_BILL_NO=@transfer_bill_no"
				);
			break;
		}
		sqlstr = sql + sqlwhere + sqlorderby;
		Log::Debug("", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("transfer_plan_no", transfer_plan_no);
		cmd_inq.Parameters.Set("transfer_bill_no", transfer_bill_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], 0, -1);
		cmd_inq.Close();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sql + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		////EDLog(1,1, "[%s]", s.sysmsg);
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

