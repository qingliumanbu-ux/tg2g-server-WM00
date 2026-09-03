/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      吴新
Version:     1.0
Date:        2016-03-25 11:35:08
Description: 出库电文发送
**************************************************/

#include "stdafx.h"
#include "epex.h"
 
#include "xwm0000.h"

BM2_FUNCTION_EXPORT
int f_wm00_wm02_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	CString s_tc_no = " ";
	CString sqlstr = "";

	//定义表实体对象 
	CWM0000 xwm0000(conn);

	CDbCommand comm(conn);

	try
	{

		//传入参数检核
		if (bcls_rec->Tables["WM02"].Rows.get_Count() == 0)
		{
			strncpy(s.msg, "传入参数出错...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//获取输入参数
		xwm0000.MergeFrom(bcls_rec->Tables["WM02"].Rows[0]);

		Log::Trace("", __FUNCTION__, "传入参数 xwm0000.MAT_NO\t[{0}]", xwm0000.MAT_NO);
		Log::Trace("", __FUNCTION__, "传入参数 xwm0000.MAT_LINE_TYPE\t[{0}]", xwm0000.MAT_LINE_TYPE);
		Log::Trace("", __FUNCTION__, "传入参数 xwm0000.MAT_KIND\t[{0}]", xwm0000.MAT_KIND);
		Log::Trace("", __FUNCTION__, "传入参数 xwm0000.STOCK_OPER_ORDER\t[{0}]", xwm0000.STOCK_OPER_ORDER);
		Log::Trace("", __FUNCTION__, "传入参数 xwm0000.STOCK_NO\t[{0}]", xwm0000.FROM_STOCK_NO);

		////冷轧机组上料出库， 不需要发电文给四级 add liQing
		////
		//if (xwm0000.MAT_LINE_TYPE.Trim() == "CR" &&xwm0000.STOCK_OPER_ORDER.Trim()=="2B")
		//{
		//	return 0;
		//}

		////热轧机组上料出库， 不需要发电文给四级 add liQing
		////
		//if (xwm0000.MAT_LINE_TYPE.Trim() == "HR" && xwm0000.STOCK_OPER_ORDER.Trim() == "2B")
		//{
		//	return 0;
		//}

		sqlstr =
			" SELECT TC_NO FROM TWM0B"
			" WHERE MAT_LINE_TYPE = @mat_line_type"
			" AND MAT_KIND = @mat_kind"
			" AND STOCK_OPER_ORDER = @stock_oper_order"
			" AND STOCK_NO = @stock_no"
			" AND MODULE_NAME = 'MM'";
		comm.SetCommandText(sqlstr);

		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Set("mat_line_type", xwm0000.MAT_LINE_TYPE);
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", xwm0000.STOCK_OPER_ORDER);
			comm.Parameters.Set("stock_oper_order_div", "1");
			comm.Parameters.Set("stock_no", " ");
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}
		Log::Trace("", __FUNCTION__, "传入参数 s_tc_no\t[{0}]", s_tc_no);
		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Set("mat_line_type", "00");
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", xwm0000.STOCK_OPER_ORDER);
			comm.Parameters.Set("stock_oper_order_div", "1");
			comm.Parameters.Set("stock_no", " ");
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}
		Log::Trace("", __FUNCTION__, "传入参数 s_tc_no1\t[{0}]", s_tc_no);

		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Set("mat_line_type", xwm0000.MAT_LINE_TYPE);
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", xwm0000.STOCK_OPER_ORDER);
			comm.Parameters.Set("stock_no", xwm0000.FROM_STOCK_NO);
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}

		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Set("mat_line_type", xwm0000.MAT_LINE_TYPE);
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", xwm0000.STOCK_OPER_ORDER);
			comm.Parameters.Set("stock_no", " ");
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}

		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Set("mat_line_type", xwm0000.MAT_LINE_TYPE);
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", "2");
			comm.Parameters.Set("stock_oper_order_div", " ");
			comm.Parameters.Set("stock_no", " ");
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}

		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Set("mat_line_type", "00");
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", xwm0000.STOCK_OPER_ORDER);
			comm.Parameters.Set("stock_no", xwm0000.FROM_STOCK_NO);
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}

		if (s_tc_no.Trim() == "")
		{
			comm.Parameters.Clear();
			comm.Parameters.Set("mat_line_type", "00");
			comm.Parameters.Set("mat_kind", xwm0000.MAT_KIND);
			comm.Parameters.Set("stock_oper_order", xwm0000.STOCK_OPER_ORDER);
			comm.Parameters.Set("stock_no", " ");
			comm.ExecuteReader();
			if (comm.Read())
			{
				s_tc_no = comm.GetString(1);
			}
			comm.Close();
		}

		if (s_tc_no.Trim() == "@@")
		{
			Log::Trace("", __FUNCTION__, "TWM0B表中配置不发送。");

			//测试，发WM0000看，否则未配置则continue
			//return 0;
		}

		if (s_tc_no.Trim() == "")
		{
			Log::Trace("", __FUNCTION__, "TWM0B表中未配置电文号。");

			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			//CMessageFormat::Format(s.msg, "TWM0B表中配置不发送。")/*初始化电文[{0}]失败。*/, arguments, 1);
			sprintf(s.msg, "TWM0B表中未配置电文号。");
			throw CApplicationException(-1, s.msg, s.svc_name);
			//测试，发WM0000看，否则未配置则continue
			//return 0;
			//s_tc_no = "WM0000";
		}

		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//拼电文数据
		if (epex.SetValue(0, xwm0000) < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SendTele() < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
			sprintf(s.sysmsg, "[%s]发送失败", (const char*)s_tc_no);
			throw CApplicationException(-1, s.msg, s.svc_name);
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

