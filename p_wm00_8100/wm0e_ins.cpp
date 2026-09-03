/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:
Description:	垛位静态属性新增
**************************************************/

//框架头文件
#include "stdafx.h"
#include "twm0e.h"
//程序用头文件





BM2F_ENTERACE(wm0e_ins);

int f_wm0e_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal SEQ_NO = 0;
	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CTWM0E twm0e(conn);

	CString sqlstr = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm0e.Reset();
			twm0e.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (twm0e.QueryCount("PILE_JUDGE_TYPE,PILE_JUDGE_CODE")>0){
				sprintf(s.msg, "记录已存在，请重新输入");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr = "SELECT MAX(SEQ_NO)+1 FROM twm0e";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			SEQ_NO = cmd_inq.ExecuteScalar();
			Log::Trace("", __FUNCTION__, "SEQ_NO = [{0}]", SEQ_NO);
			twm0e.SEQ_NO = SEQ_NO;
			twm0e.REC_CREATOR = s.userid;
			twm0e.REC_CREATE_TIME = datetime;
			twm0e.Insert();
		}
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

