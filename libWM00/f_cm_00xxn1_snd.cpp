/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     3.0
Date:        2011-12-12 11:35:08
Description: 板坯/方坯入出库确认应答电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_cm_00xxn1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	CString lpsz_tc_no = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString sqlstr = "";
	CString oper_flag = "";
	CString mat_no = "";
	CString mat_line_type = "";
	CString stock_oper_order = "";
	CString stock_no_fr = "";
	CString stock_no_to = "";
	CString in_flag = "";
	
	//定义表实体对象

	try
	{
		//传入参数检核
		if (!bcls_rec->Tables[0].Columns.Contains("TC_NO"))
		{
			strncpy(s.msg, "请传入参数TC_NO...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		lpsz_tc_no = bcls_rec->Tables[0].Rows[0]["TC_NO"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "lpsz_tc_no[{0}]", lpsz_tc_no);

		//初始化
		doFlag = epex.Initialize(lpsz_tc_no);
		if (doFlag < 0)
		{
			CFormattable arguments[] = { lpsz_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//传入参数检核
		if (!bcls_rec->Tables[0].Columns.Contains("OPER_FLAG"))
		{
			strncpy(s.msg, "请传入参数OPER_FLAG...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			strncpy(s.msg, "请传入参数MAT_NO...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
		{
			strncpy(s.msg, "请传入参数MAT_LINE_TYPE...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables[0].Columns.Contains("STOCK_OPER_ORDER"))
		{
			strncpy(s.msg, "请传入参数STOCK_OPER_ORDER...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if (!bcls_rec->Tables[0].Columns.Contains("IN_FLAG"))
		{
			strncpy(s.msg, "请传入参数IN_FLAG...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			strncpy(s.msg, "传入参数出错...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//获取输入参数
			oper_flag = bcls_rec->Tables[0].Rows[i]["OPER_FLAG"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "OPER_FLAG[{0}]", oper_flag);
			if ("" == oper_flag)
			{
				strncpy(s.msg, "传入参数OPER_FLAG不能为空。", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "MAT_NO[{0}]", mat_no);
			if ("" == mat_no)
			{
				strncpy(s.msg, "传入参数MAT_NO不能为空。", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			mat_line_type = bcls_rec->Tables[0].Rows[i]["MAT_LINE_TYPE"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "MAT_LINE_TYPE[{0}]", mat_line_type);
			if ("" == mat_line_type)
			{
				strncpy(s.msg, "传入参数MAT_LINE_TYPE不能为空。", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			stock_oper_order = bcls_rec->Tables[0].Rows[i]["STOCK_OPER_ORDER"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "STOCK_OPER_ORDER[{0}]", stock_oper_order);
			if ("" == stock_oper_order)
			{
				strncpy(s.msg, "传入参数STOCK_OPER_ORDER不能为空。", sizeof(s.msg) - 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			stock_no_fr = bcls_rec->Tables[0].Rows[i]["STOCK_NO_FR"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "STOCK_NO_FR[{0}]", stock_no_fr);

			stock_no_to = bcls_rec->Tables[0].Rows[i]["STOCK_NO_TO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "STOCK_NO_TO[{0}]", stock_no_to);

			in_flag = bcls_rec->Tables[0].Rows[i]["IN_FLAG"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "IN_FLAG[{0}]", in_flag);

			epex.SetValue("OPER_FLAG", i, oper_flag);
			epex.SetValue("MAT_NO", i, mat_no);
			epex.SetValue("MAT_LINE_TYPE", i, mat_line_type);
			epex.SetValue("STOCK_OPER_ORDER", i, stock_oper_order);
			epex.SetValue("STOCK_NO_FROM", i, stock_no_fr);
			epex.SetValue("STOCK_NO_TO", i, stock_no_to);
			epex.SetValue("IN_FLAG", i, in_flag);
			epex.SetValue("OPERAOR_NAME", i, s.userid);
			epex.SetValue("OPERATOR_TIME", i, datetime);

			if (epex.SendTele() < 0)
			{
				strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
				strcpy(s.sysmsg, "GMGEN1发送失败");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		epex.Uninitialize();
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
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