

#include "stdafx.h"

BM2F_ENTERACE(wm_dyn_sql)
int f_wm_dyn_sql(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);	//系统日志类定义
	/* 程序内部变量 */
	int doFlag = 0;
	int i = 0;
	CString sqlstr = "";	//数据库SQL操作字符串，用于捕获数据库操作异常情况
	CString sqlwhere = "";  //定义的查询条件	

	try
	{
		CDbCommand cmdData(conn);

		bcls_ret->Tables[0].set_TableName("WM_DYN_SQL");
		bcls_ret->Tables["WM_DYN_SQL"].Columns.Add(DT_STRING, "CODE");
		bcls_ret->Tables["WM_DYN_SQL"].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");

		CString sqlstr = ((CString)bcls_rec->Tables[0].Rows[0]["SQL_STR"]).Trim();

		Log::Trace("", __FUNCTION__, "sqlstr: {0}", (const char*)sqlstr);


		cmdData.SetCommandText(sqlstr);
		cmdData.ExecuteReader();

		while (cmdData.Read())
		{
			bcls_ret->Tables["WM_DYN_SQL"].Rows.Add();
			bcls_ret->Tables["WM_DYN_SQL"].Rows[i]["CODE"] = cmdData.GetString(1);
			bcls_ret->Tables["WM_DYN_SQL"].Rows[i]["CODE_DESC_1_CONTENT"] = cmdData.GetString(2);
			i++;
		}
		cmdData.Close();

		Log::Trace("", __FUNCTION__, "sqlstr---------{0}", (const char*)sqlstr);

		/*int i = bcls_ret->Tables["WM_DYN_SQL"].Rows.get_Count();
		if (i <= 0)
		{
			strcpy(s.msg, "没有查到记录");
			Log::Trace("", __FUNCTION__, "结果: {0}", s.msg);
			throw CApplicationException(-2, s.msg, s.svc_name);
		}*/

	}

	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);	//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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

	return(doFlag);
}
