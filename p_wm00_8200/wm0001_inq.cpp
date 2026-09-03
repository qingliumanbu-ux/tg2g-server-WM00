/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:        
Version:		1.0
Date:			
Description:	
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wm0001_inq);

int f_wm0001_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString s_userid("");
	CString v_table_name = "";

	try
	{
		//获取前台传入参数
		s_userid = s.userid;
		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "传入数据为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CString mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString();

		//获取主档表
		if (mat_line_type.Trim() == "SM")
		{
			v_table_name = "TMMSM01";
		}
		else if (mat_line_type.Trim() == "HR")
		{
			v_table_name = "TMMHR01";
		}
		else if (mat_line_type.Trim() == "CR")
		{
			v_table_name = "TMMCR01";
		}
		else if (mat_line_type.Trim() == "BW")
		{
			v_table_name = "TMMBW01";
		}
		else if (mat_line_type.Trim() == "SF")
		{
			v_table_name = "TMMSF01";
		}
		else if (mat_line_type.Trim() == "HP")
		{
			v_table_name = "TMMHP01";
		}
		else
		{
			sprintf(s.msg, "物料种类【%s】无法识别。", (const char*)mat_line_type);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		sqlstr = "select a.stock_no,a.stock_desc,a.stock_wgt_max,a.mat_line_type,value(b.sum_wt,0) as current_wt from twm01 a"
			" left join(select b.stock_no, count(1) as count_num, sum(a.mat_act_wt) sum_wt from " + v_table_name + " a, twma2 b where a.mat_no = b.mat_no group by b.stock_no) b"
			" on a.stock_no = b.stock_no"
			" where a.mat_line_type = '" + mat_line_type .Trim()+ "'";
		Log::Trace("", __FUNCTION__, "sqlstr= [{0}]", sqlstr);
		Db::QueryTable(sqlstr, bcls_ret->Tables[0]);
		Log::Trace("", __FUNCTION__, "get_Count= [{0}]", bcls_ret->Tables[0].Rows.get_Count());
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