/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      吴新
Version:     1.0
Date:        2016-03-25 11:35:08
Description: 转库计划材料吊销电文发送
**************************************************/

#include "stdafx.h"
#include "epex.h"

#include "xwmy100.h"  
//#include "twm03.h"
//#include "twma1.h"
//#include "twm0b.h"

BM2_FUNCTION_EXPORT
int f_wm00_wmy1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	EPEX epex;

	int doFlag = 0;
	int ret = 0;
	CString lpsz_tc_no = "", s_tc_no = " ";
	CString datetime = "";
	CString sqlstr = "";

	//定义表实体对象 
	CWMY100 xwmy100(conn);
	//CTWMA1 twma1(conn);
	//CTWM0B twm0b(conn);
	CModel twm0b("TWM0B");
	CModel twm41("TWM41");

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{

		//传入参数检核
		if (bcls_rec->Tables["WMY1"].Rows.get_Count() == 0)
		{
			strncpy(s.msg, "传入参数出错...", sizeof(s.msg) - 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//获取输入参数
		xwmy100.MergeFrom(bcls_rec->Tables["WMY1"].Rows[0]);

		Log::Trace("", __FUNCTION__, "xwmy100.MAT_NO[{0}]", xwmy100.MAT_NO);
		Log::Trace("", __FUNCTION__, "xwmy100.TRANSFER_PLAN_NO[{0}]", xwmy100.TRANSFER_PLAN_NO);


		twm41["TRANSFER_PLAN_NO"] = xwmy100.TRANSFER_PLAN_NO;
		twm41.Query("TRANSFER_PLAN_NO");
		Log::Trace("", __FUNCTION__, "twm41.STOCK_NO[{0}]", twm41["STOCK_NO"].ToString());

		//获取电文号
		twm0b["MODULE_NAME"] = "PMOL";
		twm0b["FROM_STOCK_NO"] = twm41["STOCK_NO"];
		twm0b["TO_STOCK_NO"] = "00";
		twm0b["STOCK_OPER_ORDER"] = "D";
		twm0b.Query("MODULE_NAME,STOCK_OPER_ORDER,FROM_STOCK_NO,TO_STOCK_NO");
		twm0b.TrimOrBlank();
		s_tc_no = twm0b["TC_NO"];

		Log::Trace("", __FUNCTION__, "s_tc_no\t[{0}]", s_tc_no);

		if (s_tc_no.Trim() == "")
		{
			strcpy(s.msg, "电文号获取失败");
			throw CApplicationException(-1, s.msg, log.Location);
			return(doFlag);
		}

		//初始化
		ret = epex.Initialize(s_tc_no);
		if (ret < 0)
		{
			//EDLog(1, 1, "epex.Initialize [%s] code= [%d]",lpsz_tc_no,ret);
			CFormattable arguments[] = { s_tc_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("YM00S0000514")/*初始化电文[{0}]失败。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		//拼电文数据
		if (epex.SetValue(0, xwmy100) < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000015"));//系统出现异常，电文拼接出错，请联系系统维护人员。
			//EDLog(1,1,"%s电文内容拼接失败.",lpsz_tc_no);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (epex.SendTele() < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000032")/*电文发送失败。*/);
			strcpy(s.sysmsg, "3000ya发送失败");
			//EDLog(1, 1, "3000ya发送失败 = [%s]", epex.GetMsg());
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