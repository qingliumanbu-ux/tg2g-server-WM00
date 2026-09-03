/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:
Description:	垛位静态属性查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件





BM2F_ENTERACE(wm0e_inq);

int f_wm0e_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;


	/* 业务变量 */
	CString PILE_JUDGE_TYPE = "";
	CString PILE_JUDGE_CODE = "";


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";



	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{


		//查询条件获取
		if (bcls_rec->Tables[0].Columns.Contains("PILE_JUDGE_TYPE")){
			PILE_JUDGE_TYPE = bcls_rec->Tables[0].Rows[0]["PILE_JUDGE_TYPE"].ToString();
			Log::Trace("", __FUNCTION__, "PILE_JUDGE_TYPE = [{0}]", PILE_JUDGE_TYPE);
		}
		if (bcls_rec->Tables[0].Columns.Contains("PILE_JUDGE_CODE")){
			PILE_JUDGE_CODE = bcls_rec->Tables[0].Rows[0]["PILE_JUDGE_CODE"].ToString();
			Log::Trace("", __FUNCTION__, "PILE_JUDGE_CODE = [{0}]", PILE_JUDGE_CODE);
		}
		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;  //每页记录数量
		}
		//sql
		sqlstr = "SELECT * from twm0e  a WHERE 1=1 ";
		if (PILE_JUDGE_TYPE.Trim() != "")
		{
			sqlstr += " AND A.PILE_JUDGE_TYPE like @PILE_JUDGE_TYPE ||'%' ";
		}
		if (PILE_JUDGE_CODE.Trim() != "")
		{
			sqlstr += " AND A.PILE_JUDGE_CODE =@PILE_JUDGE_CODE ";
		}
		

		Log::Debug("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("PILE_JUDGE_CODE", PILE_JUDGE_CODE);
		cmd_inq.Parameters.Set("PILE_JUDGE_TYPE", PILE_JUDGE_TYPE);
		int rowCount=cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

		cmd_inq.Close();

		//返回记录总数
		//bcls_ret->Tables.Add();
		//bcls_ret->Tables[1].set_TableName("PAGEINFO");
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount;
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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

