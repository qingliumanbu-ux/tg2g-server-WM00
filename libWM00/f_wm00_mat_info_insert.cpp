/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_stock_for_mm
*  程序描述			: 用于重量、规格、牌号等变化时，记录仓库履历
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 


BM2_FUNCTION_EXPORT
int f_wm00_mat_info_insert(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	int iRowCount_old = 0;
	int iRowCount_new = 0;

	CString v_eventid = "";
	CString v_mat_kind = "";
	CString table_name = "";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);

	/* ***** 定义表实体对象 ***** */



	//记录履历
	EIClass bcls_rec_stock_log;
	bcls_rec_stock_log.Tables[0].set_TableName("WM_STOCK_LOG");
	bcls_rec_stock_log.Tables[0].Rows.Clear();


	/* ***** 应用程序开始处理 ***** */
	try
	{
#if !defined(_SYS_PES) && !defined (_SYS_MES)  && !defined (_SYS_MMS)
		if (!bcls_rec->Tables.Contains("NEWMM_TABLE"))
		{
			sprintf(s.msg, "函数f_wm00_mat_info_insert中找不到接收块名[NEWMM_TABLE]");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		for (int i = 0; i < bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables["OLDMM_TABLE"].Columns.Contains("MAT_KIND"))
			{
				v_mat_kind = bcls_rec->Tables["OLDMM_TABLE"].Rows[i]["MAT_KIND"].ToString().Trim();
			}
			Log::Trace("", __FUNCTION__, "传入参数 v_mat_kind\t[{0}]", v_mat_kind);
			if (v_mat_kind.Trim() == "SM")
			{
				table_name = "TMMSM01";
			}
			else if (v_mat_kind.Trim() == "HR")
			{
				table_name = "TMMHR01";
			}
			else if (v_mat_kind.Trim() == "CR")
			{
				table_name = "TMMCR01";
			}
			else if (v_mat_kind.Trim() == "BW")
			{
				table_name = "TMMBW01";
			}
			else if (v_mat_kind.Trim() == "SF")
			{
				table_name = "TMMSF01";
			}
			else if (v_mat_kind.Trim() == "HP")
			{
				table_name = "TMMHP01";
			}
			else
			{
				sprintf(s.msg, "物料种类【%s】无法识别。", (const char*)v_mat_kind);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			CModel twma1_new(table_name);

			twma1_new.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[i]);

			if (bcls_rec->Tables["NEWMM_TABLE"].Columns.Contains("EVENT_ID"))
			{
				v_eventid = bcls_rec->Tables["NEWMM_TABLE"].Rows[i]["EVENT_ID"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "传入参数 v_eventid\t[{0}]", v_eventid);
			Log::Trace("", __FUNCTION__, "传入参数 twma1_new.MAT_NO\t[{0}]", twma1_new["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 twma1_new.MAT_LINE_TYPE\t[{0}]", twma1_new["MAT_LINE_TYPE"].ToString());

			if (twma1_new.QueryCount("MAT_NO") > 0)
			{
				sprintf(s.msg, "材料【%s】已存在。", (const char*)twma1_new["MAT_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma1_new.Insert();
		}
#endif

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
