/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2016-04-07 15:51:46
Description: 转库计划头电文接收
**************************************************/

/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
//#include "twm41.h" 
//#include "twm01.h"

/* -EP_SYSTEM_HEAD_END */
BM2_FUNCTION_EXPORT
int f_wm00_transfer_plan(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int	doFlag = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString sqlstr = "", s_message = " ";;

	Log::Trace("", __FUNCTION__, "gggggggggggggggg");
	//CTWM41 twm41(conn);
	//CTWM01 twm01(conn);
	CModel twm41("TWM41");
	Log::Trace("", __FUNCTION__, "dddddddddddddddddd");
	CModel twm01("TWM01");

	CDbCommand cmd_inq(conn);

	try
	{
		Log::Trace("", __FUNCTION__, "1111111111111111");
		twm41["TRANSFER_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"];	//计划号
		Log::Trace("", __FUNCTION__, "aaaaaaaaaaaaaaaaa");
		twm41["TOTAL_WEI"] = bcls_rec->Tables[0].Rows[0]["TOTAL_WEI"];			//计划总重量
		twm41["TOTAL_NUM"] = bcls_rec->Tables[0].Rows[0]["TOTAL_NUM"];			//计划总个数
		twm41["PRG_SEND_TIME"] = bcls_rec->Tables[0].Rows[0]["PLAN_TO_TIME"];		//计划下达时刻
		twm41["PLAN_MAKER"] = bcls_rec->Tables[0].Rows[0]["USER_ID"];			//计划责任者
		twm41["PLAN_START_TIME"] = bcls_rec->Tables[0].Rows[0]["PLAN_START_TIME"];	//计划开始时刻
		twm41["PLAN_END_TIME"] = bcls_rec->Tables[0].Rows[0]["PLAN_END_TIME"];		//计划结束时刻
		twm41["STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["NOW_STORE"];			//当前库号
		twm41["AIM_STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["AIM_STORE"];			//目的库号
		twm41["MAT_KIND"] = bcls_rec->Tables[0].Rows[0]["MAT_KIND"];			//物料种类
		twm41["MAT_DESTION"] = bcls_rec->Tables[0].Rows[0]["MAT_DESTION"];		//物料去向
		//twm41["MAT_LINE_TYPE"] = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"];		//物料去向

		Log::Trace("", __FUNCTION__, "22222222222");
		twm41["TRANSFER_STATUS"] = "2";	//计划下发

		Log::Trace("", __FUNCTION__, "33333333333");
		twm41["REC_CREATE_TIME"] = datetime;
		twm41["REC_CREATOR"] = s.userid;

		Log::Trace("", __FUNCTION__, "44444444");
		twm41.TrimOrBlank();

		Log::Trace("", __FUNCTION__, "55555555555555555555");
		Log::Trace("", __FUNCTION__, "twm41.STOCK_NO ={0}", twm41["STOCK_NO"].ToString());
		twm01["STOCK_NO"] = twm41["STOCK_NO"];
		Log::Trace("", __FUNCTION__, "twm01.STOCK_NO= {0}", twm01["STOCK_NO"].ToString());
		if (!twm01.Query("STOCK_NO"))
		{
			CFormattable arguments[] = { twm01["STOCK_NO"].ToString() };// 定义参数列表的数组
			//printf(s.msg, _RES("YM00S0000750")/*库区号：[ {0}] 不存在!*/, arguments, 1);
			s_message = "库区号：" + twm01["STOCK_NO"].ToString() +"不存在!";
			sprintf(s.msg, s_message);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		twm41["MAT_LINE_TYPE"] = twm01["MAT_LINE_TYPE"];

		//插入twm41表数据

		if (twm41.QueryCount("TRANSFER_PLAN_NO") > 0)
		{
			twm41.Delete("TRANSFER_PLAN_NO");
		}
		twm41.Insert();

		/*设置系统返回参数*/
		sprintf(s.msg, _RES("GCRSS0000036")/*电文接收成功。*/);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.msg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
