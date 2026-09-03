/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      
Version:     1.0
Date:        2016-04-13
Description:  转库材料吊销
**************************************************/

#include "stdafx.h"
//#include "twm42.h"
//#include "twm41.h"
//#include "twma0.h"
//#include "xwmy100.h"  

/*<remark>=========================================================
/// <summary>
///   转库材料吊销
/// <para>
///    删除转库计划明细表；
///    调用物料跟踪函数PM07事件（转库计划撤销）； 
/// </para>
/// </summary>
/// <param name="param1">参数1  </param>
/// <param name="param2">参数2  </param>
/// <returns>返回参数：0 成功；-1 失败</returns>
===========================================================</remark>*/

//调用外部函数
#if defined(_SYS_PES) 
int f_wm00_wmy1_snd(EIClass * bcls_rec, EIClass * bcls_ret ,CDbConnection * conn);
int f_mm0099(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#if defined (_SYS_MES)  || defined (_SYS_MMS)
int f_pmol25_del_all(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//转库计划吊销
#endif

// Service 入口
BM2F_ENTERACE(wm_transfer_del)
int f_wm_transfer_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	CDecimal del_ym0042 = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString mat_no = "";
	CString transfer_plan_no = "";
	CString reje_reas_code = "";
	CString reje_reas_desc = "";
	CString operate_time = "";
	CString mat_kind = "";
	
	CString sqlstr = "";

	CDbCommand cmd_inq(conn);

	// 定义表的实体对象
	//CTWM42 twm42(conn);
	//CTWM41 twm41(conn);
	//CTWMA0 twma0(conn);
	CModel twm42("TWM42");
	CModel twm41("TWM41");
	CModel twma0("TWMA0");
	//CWMY100 xwmy100(conn);

	EIClass bcls_pmol25;
	bcls_pmol25.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	bcls_pmol25.Tables[0].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
	bcls_pmol25.Tables[0].Columns.Add(DT_STRING, "REJE_REAS_CODE");
	bcls_pmol25.Tables[0].Columns.Add(DT_STRING, "OPERATE_TIME");
	bcls_pmol25.Tables[0].Columns.Add(DT_STRING, "OPERATOR");

	EIClass bcls_wm00_wmy1;
	if (!bcls_wm00_wmy1.Tables.Contains("WMY1"))
	{
		bcls_wm00_wmy1.Tables[0].set_TableName("WMY1");
	}
	bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
	bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(DT_STRING, "REJE_REAS_CODE");
	bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(DT_STRING, "OPERATE_TIME");
	bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(DT_STRING, "USER_ID");
	bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(DT_STRING, "REJE_REAS_DESC");
	//bcls_wm00_wmy1.Tables["WMY1"].Columns.Add(xwmy100);

	EIClass bcls_rec_mm99;
	bcls_rec_mm99.Tables[0].set_TableName("MM0099");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_KIND");


	try
	{

		//前台传入参数检核
		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			strcpy(s.msg, _RES("YM00S0000710")/*传入记录数不能为0！*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
			transfer_plan_no = bcls_rec->Tables[0].Rows[i]["TRANSFER_PLAN_NO"];
			reje_reas_code = bcls_rec->Tables[0].Rows[i]["REJE_REAS_CODE"];
			reje_reas_desc = bcls_rec->Tables[0].Rows[i]["REJE_REAS_DESC"];
			if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND") == true)
			{
				mat_kind = bcls_rec->Tables[0].Rows[i]["MAT_KIND"];
			}
			Log::Trace("", __FUNCTION__, "传入参数 MAT_NO			= [{0}]", mat_no);
			Log::Trace("", __FUNCTION__, "传入参数 TRANSFER_PLAN_NO	= [{0}]", transfer_plan_no);
			Log::Trace("", __FUNCTION__, "传入参数 REJE_REAS_CODE	= [{0}]", reje_reas_code);
			Log::Trace("", __FUNCTION__, "传入参数 REJE_REAS_DESC	= [{0}]", reje_reas_desc);
			Log::Trace("", __FUNCTION__, "传入参数 MAT_KIND			= [{0}]", mat_kind);


			//若MMS&PES模式，则调用转库吊销电文，否则调用生产接口函数
#if defined(_SYS_PES) 
			bcls_wm00_wmy1.Tables[0].Rows.Add();
			bcls_wm00_wmy1.Tables[0].Rows[0]["MAT_NO"] = mat_no;
			bcls_wm00_wmy1.Tables[0].Rows[0]["TRANSFER_PLAN_NO"] = transfer_plan_no;
			bcls_wm00_wmy1.Tables[0].Rows[0]["REJE_REAS_CODE"] = reje_reas_code;
			bcls_wm00_wmy1.Tables[0].Rows[0]["OPERATE_TIME"] = datetime;
			bcls_wm00_wmy1.Tables[0].Rows[0]["USER_ID"] = s.userid;
			bcls_wm00_wmy1.Tables[0].Rows[0]["REJE_REAS_DESC"] = reje_reas_desc;

			doFlag = f_wm00_wmy1_snd(&bcls_wm00_wmy1, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("", __FUNCTION__, "111");
			bcls_rec_mm99.Tables["MM0099"].Rows.Clear();
			bcls_rec_mm99.Tables["MM0099"].Rows.Add();
			bcls_rec_mm99.Tables["MM0099"].Rows[0]["EVENT_ID"] = "PM07";
			bcls_rec_mm99.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_mm99.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "WM00";
			bcls_rec_mm99.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
			bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_NO"] = mat_no;
			bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_KIND"] = mat_kind;
			doFlag = f_mm0099(&bcls_rec_mm99, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif

			//计划体
			twm42["MAT_NO"] = mat_no;
			twm42["TRANSFER_PLAN_NO"] = transfer_plan_no;
			if (!twm42.Query("MAT_NO, TRANSFER_PLAN_NO"))
			{
				CFormattable arguments[] = { mat_no }; // 定义参数列表的数组
				strcpy(s.msg, "材料在转库计划体表TWM42中不存在");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twma0["MAT_NO"] = mat_no;
			twma0["STOCK_OPER_ORDER"] = "2G";
			if (twma0.QueryCount("MAT_NO, STOCK_OPER_ORDER") == 0 &&
				twm42["AFFIRM_MARK"].ToString() == "3")
			{
				CFormattable arguments[] = { mat_no }; // 定义参数列表的数组
				strcpy(s.msg, "材料已经完成出库，不允许吊销！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma0.QueryCount("MAT_NO, STOCK_OPER_ORDER") > 0)
			{
				twma0.Delete("MAT_NO, STOCK_OPER_ORDER");
			}


			Log::Trace("", __FUNCTION__, "传入数据 测试 = [{0}]", twm42["MAT_NO"].ToString());

			twm42.Delete("MAT_NO");

			del_ym0042 = twm42.QueryCount("TRANSFER_PLAN_NO");
			if (del_ym0042 == 0)
			{
				twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
				Log::Trace("", __FUNCTION__, "转库计划下无材料信息，准备删除转库计划信息，transfer_plan_no[{0}]",
					twm41["TRANSFER_PLAN_NO"].ToString());
				twm41.Delete("TRANSFER_PLAN_NO");
			}

			//传入数据  

#if defined (_SYS_MES) || defined (_SYS_MMS)
			Log::Trace("", __FUNCTION__, "-----------如果是MMS系统的话，吊销的时候需要调用生产函数-----------开始-------------");
			bcls_pmol25.Tables[0].Rows.Add();
			bcls_pmol25.Tables[0].Rows[0]["MAT_NO"] = mat_no;
			bcls_pmol25.Tables[0].Rows[0]["TRANSFER_PLAN_NO"] = transfer_plan_no;
			bcls_pmol25.Tables[0].Rows[0]["REJE_REAS_CODE"] = "吊销";
			bcls_pmol25.Tables[0].Rows[0]["OPERATE_TIME"] = datetime;
			bcls_pmol25.Tables[0].Rows[0]["OPERATOR"] = s.userid;

			doFlag = f_pmol25_del_all(&bcls_pmol25, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("", __FUNCTION__, "-----------如果是MMS系统的话，吊销的时候需要调用生产函数-----------结束-------------");
#endif
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

