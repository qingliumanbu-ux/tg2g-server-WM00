/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2016-04-13 11:35:08
Description: 转库出库
**************************************************/

#include "stdafx.h"
//调用外部函数
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);	//转库出库队列
int f_wmbwsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//坯料出库主函数
int f_wmbwbw_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//成品出库主函数 
int f_7000a5_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//按量完成计划
int f_mm0099(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
// Service 入口
BM2F_ENTERACE(wm41_transfer_out)

int f_wm41_transfer_out(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString endtime = CDateTime::Now().AddHours(1).ToString("yyyyMMddHHmmss");
	CString mat_no = "";
	CString transfer_plan_no = "";
	CString aim_stock_no = "";
	CString sqlstr = "";
	CDecimal v_count = 0;
	CString v_table_name = "";
	CString vehicle_no = "";
	CString vehicle_seq_no = "";
	CString factory_div = "";
	CString stock_no = "";
	CString plan_type = "";
	CDecimal plan_wt = 0;
	CDecimal act_wt = 0;
	CDecimal act_count = 0;
	bool IsOrNO = false;
	// 定义表的实体对象
	CModel twma0 = CModel("TWMA0");
	CModel twma2 = CModel("TWMA2");
	CModel twm41 = CModel("TWM41");
	CModel twm42 = CModel("TWM42");
	CModel tsm00b1 = CModel("TSM00B1");
	CModel twmb5 = CModel("TWMB5");
	CModel tmmbw01 = CModel("TMMBW01");
	CModel tmmsm01 = CModel("TMMSM01");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);


	//调用仓库出库主函数
	EIClass bcls_stock_out;
	bcls_stock_out.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_out.Tables[0].Columns.Add(twma0);
	bcls_stock_out.Tables[0].Columns.Add(twma2);

	bcls_stock_out.Tables[0].Rows.Clear();
	//入库队列
	EIClass bcls_wm00;
	bcls_wm00.Tables[0].set_TableName("WM00QUE");
	bcls_wm00.Tables[0].Columns.Add(twma0);
	bcls_wm00.Tables[0].Rows.Add();
	/* 块定义 */
	EIClass bcls_rec_a5;
	bcls_rec_a5.Tables[0].set_TableName("7000A5");
	bcls_rec_a5.Tables["7000A5"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
	bcls_rec_a5.Tables["7000A5"].Columns.Add(DT_STRING, "FLAG");


	try
	{
		//前台传入参数检核
		if (bcls_rec->Tables[0].Rows.get_Count() == 0)
		{
			strcpy(s.msg, _RES("YM00S0000710")/*传入记录数不能为0！*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();
		vehicle_seq_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_SEQ_NO"].ToString().Trim();
		plan_type = bcls_rec->Tables[0].Rows[0]["PLAN_TYPE"].ToString().Trim();

		tsm00b1["VEHICLE_NO"] = vehicle_no;
		tsm00b1["VEHICLE_SEQ_NO"] = vehicle_seq_no;
		Log::Trace("", __FUNCTION__, "VEHICLE_NO=[{0}]", vehicle_no);
		IsOrNO = tsm00b1.Query("VEHICLE_NO,VEHICLE_SEQ_NO");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			transfer_plan_no = bcls_rec->Tables[0].Rows[i]["TRANSFER_PLAN_NO"].ToString().Trim();
			aim_stock_no = bcls_rec->Tables[0].Rows[i]["AIM_STOCK_NO"].ToString().Trim();
			stock_no = bcls_rec->Tables[0].Rows[i]["STOCK_NO"].ToString().Trim();

			Log::Trace("", __FUNCTION__, "传入参数 MAT_NO				= [{0}]", mat_no);
			Log::Trace("", __FUNCTION__, "传入参数 TRANSFER_PLAN_NO		= [{0}]", transfer_plan_no);
			Log::Trace("", __FUNCTION__, "传入参数 AIM_STOCK_NO		= [{0}]", aim_stock_no);
			Log::Trace("", __FUNCTION__, "传入参数 VEHICLE_NO				= [{0}]", vehicle_no);
			Log::Trace("", __FUNCTION__, "传入参数 FACTORY_DIV				= [{0}]", factory_div);

			sqlstr =
				" SELECT COUNT(1) FROM TMMSM01"
				" WHERE MAT_NO = @mat_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", mat_no);
			v_count = cmd_inq.ExecuteScalar();
			if (v_count == 1)
			{
				v_table_name = "TMMSM01";
			}

			sqlstr =
				" SELECT COUNT(1) FROM TMMBW01"
				" WHERE MAT_NO = @mat_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", mat_no);
			v_count = cmd_inq.ExecuteScalar();
			if (v_count == 1)
			{
				v_table_name = "TMMBW01";
			}

			Log::Trace("", __FUNCTION__, "v_table_name\t[{0}]", v_table_name);

			if (v_table_name.Trim() == "")//确认标志
			{
				sprintf(s.msg, "材料不在主档中，不允许确认 ");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			CModel twma1 = CModel(v_table_name);



			Log::Trace("", __FUNCTION__, "1111111111111111");
			//获取转库计划材料信息
			if (plan_type == "J")
			{
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
			}

			Log::Trace("", __FUNCTION__, "222222222");

			if (twm42["AFFIRM_MARK"].ToString() == "8")//确认标志
			{
				sprintf(s.msg, "转库计划号[%s]材料号[%s]在已经完成确认，不允许再次确认 ",
					(const char*)transfer_plan_no, (const char*)mat_no);
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
			if (plan_type == "L")  //这里按量计划转库出库的重量以MAT_WT为主 这里查询了主档表 顺便取主档表的重量
			{
				plan_wt = plan_wt + twma1["MAT_WT"].ToDecimal();
				Log::Trace("", "", "plan_wt-------------={0}", plan_wt);
			}
			else
			{

				act_wt = act_wt + twma1["MAT_WT"].ToDecimal();
				act_count = act_count + 1;
				Log::Trace("", "", "act_wt={0},act_count={1}", act_wt, act_count);

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
			if (plan_type == "J")
			{
				twma0["STOCK_NO"] = twm42["STOCK_NO"];
			}
			else
			{
				twma0["STOCK_NO"] = aim_stock_no;
			}
			twma0["FROM_STOCK_NO"] = twm42["STOCK_NO"];
			twma0["TO_STOCK_NO"] = twm42["AIM_STOCK_NO"];
			twma0["OPER_FLAG"] = "I";
			twma0["STOCK_CONFM_FLAG"] = " ";
			twma0["VEHICLE_NO"] = vehicle_no;

			CDataTable dt_temp;
			dt_temp.Clear();
			twma0.MergeTo(dt_temp);
			bcls_wm00.Tables["WM00QUE"].Rows[0].Merge(dt_temp.Rows[0]);

			Log::Trace("", __FUNCTION__, "44444444444444444");

			//调用仓库出库主函数
			bcls_stock_out.Tables["WM_STOCK"].Rows.Clear();
			bcls_stock_out.Tables["WM_STOCK"].Rows.Add();
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = mat_no;
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = "2G";
			if (plan_type == "J")
			{
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = twm42["AIM_STOCK_NO"];
			}
			else
			{
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = aim_stock_no;
			}

			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = "0";
			bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";
			if (v_table_name == "TMMSM01")
			{
				doFlag = f_wmbwsm_stock_out(&bcls_stock_out, bcls_ret, conn);
			}
			if (v_table_name == "TMMBW01")
			{
				doFlag = f_wmbwbw_stock_out(&bcls_stock_out, bcls_ret, conn);
			}
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
			twm41["TRANSFER_STATUS"] = "8";//转库计划状态 8-计划执行 2-计划下发 4-计划确认 A-计划红冲
			twm41.Update("TRANSFER_STATUS", "TRANSFER_PLAN_NO");

			//这里时候的材料也没什么用 就删掉了
			twm42["MAT_NO"] = mat_no;
			twmb5["MAT_NO"] = mat_no;
			twm42.Delete();
			twmb5.Delete();
		}
		Log::Trace("", __FUNCTION__, "修改转库计划信息");
		//修改转库计划信息
		twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
		twm42["TRANSFER_PLAN_NO"] = transfer_plan_no;
		twm42["AFFIRM_MARK"] = "2";
		if (twm41.Query("TRANSFER_PLAN_NO"))
		{
			if (twm41["TRANSFER_STATUS"].ToString().Trim() != "2" &&
				twm41["TRANSFER_STATUS"].ToString().Trim() != "8")//确认标志
			{
				sprintf(s.msg, "转库计划号[%s]材料号[%s]转库计划状态不是2或者8，不允许确认 ", (const char*)transfer_plan_no, (const char*)mat_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "计划删除......");
			if (plan_type == "J")
			{
				twm41["DELIVY_WT"] = twm41["DELIVY_WT"].ToDecimal() + act_wt;
				twm41["DELIVY_NUM"] = twm41["DELIVY_NUM"].ToDecimal() + act_count;
				if (twm41["DELIVY_NUM"].ToDecimal() > twm41["TOTAL_NUM"].ToDecimal())
				{
					sprintf(s.msg, "出厂材料个数已经大于计材料个数量，请检查!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				twm41.Update("DELIVY_WT,DELIVY_NUM", "TRANSFER_PLAN_NO");

				if (twm42.QueryCount("TRANSFER_PLAN_NO") == 0)
				{
					Log::Trace("", "", "删除计划");
					twm41.Delete();//这里按件 如果没有对应的材料计划就已经完成 在此删掉计划
				}
			}
			else if (plan_type == "L")
			{
				twm41["DELIVY_WT"] = twm41["DELIVY_WT"].ToDecimal() + plan_wt;
				if (twm41["DELIVY_WT"].ToDecimal() > twm41["TOTAL_WEI"].ToDecimal())
				{
					sprintf(s.msg, "出厂重量大于计划重量，请检查!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				twm41.Update("DELIVY_WT", "TRANSFER_PLAN_NO");
			}
			else
			{
				sprintf(s.msg, "计划类型不明确!");
				throw CApplicationException(-1, s.msg, log.Location);
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

