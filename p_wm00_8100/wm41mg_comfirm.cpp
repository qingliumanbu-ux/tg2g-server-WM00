/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2016-04-13 11:35:08
Description: 转库出库
**************************************************/

#include "stdafx.h"
//#include "smhs.h"
//#include "twm41.h"//转库计划表
//#include "twm42.h"//转库材料表
//#include "twma0.h"//倒垛队列
//#include "twma1.h" 
//#include "twma2.h" 
//#include "twm00.h"//当前环境配置表
//#include "twm01.h"


//调用外部函数


BM2_FUNCTION_IMPORT
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	//转库出库队列


// Service 入口
BM2F_ENTERACE(wm41mg_comfirm)

int f_wm41mg_comfirm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString mat_no = "";
	CString transfer_plan_no = "";
	CString if_vehicle = "";
	CString aim_stock_no = "";
	CString sqlstr = "";
	CString prod_shift_no = "";
	CString date = "";
	CString tel_no = "";
	CString remark = "";
	CString mat_no22 = "";
	CString company_code1 = "";
	CString carry_pos_name1 = "";
	CString company_code = "";
	CString carry_pos_name = "";
	CString v_auto_confm = "";
	CDecimal v_count = 0;
	CString v_table_name = "";

	// 定义表的实体对象
	CModel twma0("TWMA0");
	CModel twma2("TWMA2");
	CModel twm41("TWM41");
	CModel twm41dj("TWM41DJ");
	CModel twm42("TWM42");
	CModel twma1("TMMSM01");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	EIClass bcls_wm00;
	bcls_wm00.Tables[0].set_TableName("WM00QUE");
	bcls_wm00.Tables[0].Columns.Add(twma0);
	bcls_wm00.Tables[0].Rows.Add();

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
			mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			transfer_plan_no = bcls_rec->Tables[0].Rows[i]["TRANSFER_PLAN_NO"].ToString().Trim();
			aim_stock_no = bcls_rec->Tables[0].Rows[i]["AIM_STOCK_NO"].ToString().Trim();

			Log::Trace("", __FUNCTION__, "传入参数 MAT_NO				= [{0}]", mat_no);
			Log::Trace("", __FUNCTION__, "传入参数 TRANSFER_PLAN_NO		= [{0}]", transfer_plan_no);
			Log::Trace("", __FUNCTION__, "传入参数 AIM_STOCK_NO		= [{0}]", aim_stock_no);

		
			//获取转库计划材料信息
			twm42["TRANSFER_PLAN_NO"] = transfer_plan_no;
			twm42["MAT_NO"] = mat_no;
			if (!twm42.Query("TRANSFER_PLAN_NO, MAT_NO"))
			{
				CFormattable arguments[] = { mat_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("YM00S0000313")/*[{0}]没有转库计划，请确认接收到计划以后再操作。*/, arguments, 1);
				sprintf(s.msg, "转库计划号[%s]材料号[%s]在转库计划明细表中不存在。",
					(const char*)transfer_plan_no, (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			Log::Trace("", __FUNCTION__, "222222222");

			if (twm42["AFFIRM_MARK"].ToString() != "2")//确认标志
			{
				sprintf(s.msg, "转库计划号[%s]材料号[%s]在已经完成确认，不允许再次确认 ",
					(const char*)transfer_plan_no, (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm41dj["TRANSFER_PLAN_NO"] = transfer_plan_no;
			twm41dj["TRANSFER_BILL_NO"] = twm42["TRANSFER_BILL_NO"];
			if (!twm41dj.Query("TRANSFER_PLAN_NO, TRANSFER_BILL_NO"))
			{
				sprintf(s.msg, "转库计划号[%s]单据号[%s]在转库单据表中不存在。",
					(const char*)transfer_plan_no, (const char*)twm41dj["TRANSFER_BILL_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "AAAA");
			if (twm41dj["MOVE_BILL_STATUS"].ToString() != "2")//状态
			{
				sprintf(s.msg, "转库单据号[%s]状态有误！ ", (const char*)twm41dj["TRANSFER_BILL_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
			if (!twm41.Query("TRANSFER_PLAN_NO"))
			{
				sprintf(s.msg, "转库计划号[%s]在转库计划表中不存在。",(const char*)transfer_plan_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "BBBB");
			if (twm41["TRANSFER_STATUS"].ToString() != "2")//状态
			{
				sprintf(s.msg, "转库计划号[%s]状态有误！ ", (const char*)transfer_plan_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "333333333333333");
			twma1["MAT_NO"] = mat_no;
			if (!twma1.Query("MAT_NO"))
			{
				CFormattable arguments[] = { mat_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("YM00S0000278")/*[{0}]的材料信息不存在。*/, arguments, 1);
				sprintf(s.msg, "[%s]的材料信息不存在。", (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			Log::Trace("", __FUNCTION__, "444444444444444");

			twma0.CopyFrom(twma1);
			twma0["REC_CREATOR"] = s.userid;
			twma0["REC_CREATE_TIME"] = datetime;
			twma0["REC_REVISOR"] = s.userid;
			twma0["REC_REVISE_TIME"] = datetime;
			twma0["STOCK_OPER_ORDER"] = "2G";//出库操作指示 2G-转库出库
			twma0["MAT_NO"] = mat_no;
			twma0["PLAN_NO"] = twm42["TRANSFER_PLAN_NO"];
			twma0["STOCK_NO"] = twm42["STOCK_NO"];
			twma0["FROM_STOCK_NO"] = twm42["STOCK_NO"];
			twma0["TO_STOCK_NO"] = twm42["AIM_STOCK_NO"];
			twma0["OPER_FLAG"] = "I";
			twma0["STOCK_CONFM_FLAG"] = " ";
			twma0["VEHICLE_NO"] = "";

			//row_update.Merge(twma0);
			CDataTable dt_temp;
			dt_temp.Clear();
			twma0.MergeTo(dt_temp);
			bcls_wm00.Tables["WM00QUE"].Rows[0].Merge(dt_temp.Rows[0]);

			Log::Trace("", __FUNCTION__, "44444444444444444");
			//	twma0.MergeTo(bcls_wm00.Tables["WM00QUE"], false);

			doFlag = f_wm00_queue(&bcls_wm00, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//////////////////////////////////修改材料的确认状态/////////////////////////////
			Log::Trace("", __FUNCTION__, "修改材料的确认状态");
			twm42["AFFIRM_MARK"] = "8";
			twm42.Update("AFFIRM_MARK", "TRANSFER_PLAN_NO, MAT_NO");
			Log::Trace("", __FUNCTION__, "twm42.MAT_NO = [{0}] twm42.AFFIRM_MARK = [{1}]",
				twm42["MAT_NO"].ToString(), twm42["AFFIRM_MARK"].ToString());
			twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
			//单据
			Log::Trace("", __FUNCTION__, "单据");
			if (twm42.QueryCount("TRANSFER_PLAN_NO,TRANSFER_BILL_NO,AFFIRM_MARK") == twm41dj["BILL_MAT_NUM"].ToDecimal()){
				twm41dj["MOVE_BILL_STATUS"] = "4";//确认
				twm41dj.Update("MOVE_BILL_STATUS", "TRANSFER_PLAN_NO, TRANSFER_BILL_NO");
			}
			Log::Trace("", __FUNCTION__, "计划");
			//计划
			if (twm42.QueryCount("TRANSFER_PLAN_NO,AFFIRM_MARK") == twm41["TOTAL_NUM"].ToDecimal()){
				twm41["TRANSFER_STATUS"] = "4";//确认
				twm41.Update("TRANSFER_STATUS", "TRANSFER_PLAN_NO");
			}

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

