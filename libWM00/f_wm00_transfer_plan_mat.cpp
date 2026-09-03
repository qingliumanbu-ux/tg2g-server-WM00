/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2016-04-07
Description: 转库计划体电文接收
**************************************************/

/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** 头文件部分 *****/
//#include "twm41.h"
//#include "twm42.h"
//#include "twm01.h"
//#include "twma0.h"
//#include "twma1.h"

//调用外部函数
BM2_FUNCTION_IMPORT
int f_wm00_queue(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);	//转库出库队列

#if defined(_SYS_PES)
int f_mm0099(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
#endif

/* -EP_SYSTEM_HEAD_END */
BM2_FUNCTION_EXPORT
int f_wm00_transfer_plan_mat(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int	doFlag = 0;

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString mat_no = "";
	CString v_auto_confm = "";
	CString v_tablename = "";

	CString sqlstr = "", s_message = " ";
	int iRow = 0;

	//CTWM41 twm41(conn);
	//CTWM42 twm42(conn);
	//CTWM01 twm01(conn);
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	Log::Trace("", __FUNCTION__, "gggggggggggggg");
	CModel twm41("TWM41");
	Log::Trace("", __FUNCTION__, "hhhhhhhhhhhhh");
	CModel twm42("TWM42");
	CModel twm01("TWM01");
	CModel twma0("TWMA0");

	CDbCommand cmd_inq(conn);



	EIClass bcls_wm00;
	bcls_wm00.Tables[0].set_TableName("WM00QUE");
	bcls_wm00.Tables[0].Columns.Add(twma0);
	CDataRow& row_update = bcls_wm00.Tables["WM00QUE"].Rows.Add();


	EIClass bcls_rec_mm99;
	bcls_rec_mm99.Tables[0].set_TableName("MM0099");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_DESTION");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "AIM_STORE");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_KIND");
	bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");

	try
	{
		twm42["TRANSFER_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"];	//转库计划号
		//twm42.TRANSFER_BILL_NO = bcls_rec->Tables[0].Rows[0]["TRANSFER_BILL_NO"];	//转库单据号
		//twm42.ORDER_NO = bcls_rec->Tables[0].Rows[0]["ORDER_NO"];			//合同号
		twm42["ORDER_WT_TOLTAL"] = bcls_rec->Tables[0].Rows[0]["ORDER_WT_TOLTAL"];	//订货总重量
		twm42["DELIVY_DATE"] = bcls_rec->Tables[0].Rows[0]["DELIVY_DATE"];		//交货日期
		//twm42.SG_SIGN = bcls_rec->Tables[0].Rows[0]["SG_SIGN"];			//牌号
		//twm42.SG_STD = bcls_rec->Tables[0].Rows[0]["SG_STD"];			//标准
		twm42["BILL_MAT_NUM"] = bcls_rec->Tables[0].Rows[0]["BILL_MAT_NUM"];		//单据内材料个数
		twm42["BILL_MAT_WT"] = bcls_rec->Tables[0].Rows[0]["BILL_MAT_WT"];		//单据计划材料重量
		twm42["STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["NOW_STORE"];			//库号
		twm42["AIM_STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["AIM_STORE"];			//目标库区
		twm42["MAT_KIND"] = bcls_rec->Tables[0].Rows[0]["MAT_KIND"];			//物料种类


		if (twm42["MAT_KIND"].ToString() == "SM")
		{
			v_tablename = "TMMSM01";
		}
		else if (twm42["MAT_KIND"].ToString() == "HR")
		{
			v_tablename = "TMMHR01";
		}
		else if (twm42["MAT_KIND"].ToString() == "CR")
		{
			v_tablename = "TMMCR01";
		}
		else if (twm42["MAT_KIND"].ToString() == "BW")
		{
			v_tablename = "TMMBW01";
		}
		else if (twm42["MAT_KIND"].ToString() == "HP")
		{
			v_tablename = "TMMHP01";
		}
		else if (twm42["MAT_KIND"].ToString() == "SF")
		{
			v_tablename = "TMMSF01";
		}

		CModel twma1(v_tablename);

		twm01["STOCK_NO"] = twm42["STOCK_NO"];
		if (!twm01.Query("STOCK_NO"))
		{
			CFormattable arguments[] = { twm01["STOCK_NO"].ToString() };// 定义参数列表的数组
			//printf(s.msg, _RES("YM00S0000750")/*库区号：[ {0}] 不存在!*/, arguments, 1);
			s_message = "库区号：" + twm01["STOCK_NO"].ToString() + "不存在!";
			sprintf(s.msg, s_message);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		iRow = bcls_rec->Tables[0].Rows.get_Count();


		Log::Trace("", __FUNCTION__, "iRow【{0}】", iRow);


		//查询是否自动确认转库计划
		sqlstr =
			" SELECT * FROM TEP0002"
			" WHERE CODE_CLASS = 'WMZK'"
			" AND CODE = '1'";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			v_auto_confm = "1";
		}
		cmd_inq.Close();


		//for (int i = 1; i <= twm42.BILL_MAT_NUM; i++)
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm42["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];

			twm42["TRANSFER_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"];	//转库计划号
			twm42["TRANSFER_BILL_NO"] = bcls_rec->Tables[0].Rows[0]["TRANSFER_BILL_NO"];	//转库单据号
			twm42["ORDER_NO"] = bcls_rec->Tables[0].Rows[0]["ORDER_NO"];			//合同号
			twm42["SG_SIGN"] = bcls_rec->Tables[0].Rows[0]["SG_SIGN"];			//牌号
			twm42["SG_STD"] = bcls_rec->Tables[0].Rows[0]["SG_STD"];			//标准
			//twm42.MAT_NO = bcls_rec->Tables[0].Rows[i - 1]["MAT_NO"];
			Log::Debug("", __FUNCTION__, "MAT_NO = [{0}]", twm42["MAT_NO"].ToString());
			if (twm42["MAT_NO"].ToString().Trim() != "")
			{
				Log::Trace("", __FUNCTION__, "0000000000000000000000000000000000000000");
				twm42["AFFIRM_MARK"] = "2";
				twm42["REC_CREATE_TIME"] = datetime;
				twm42["REC_CREATOR"] = s.userid;
				twm42.TrimOrBlank();

				//插入twm42表数据
				Log::Trace("", __FUNCTION__, "22222222222222222222222222222222222");
				if (twm42.QueryCount("TRANSFER_PLAN_NO, MAT_NO") > 0)
				{
					twm42.Delete("TRANSFER_PLAN_NO, MAT_NO");
				}
				sqlstr = "insert into twm42";
				twm42.TrimOrBlank();
				twm42.Insert();



#if defined(_SYS_PES)

				twm41["TRANSFER_PLAN_NO"] = twm42["TRANSFER_PLAN_NO"];
				twm41.Query("TRANSFER_PLAN_NO");
				bcls_rec_mm99.Tables["MM0099"].Rows.Clear();
				bcls_rec_mm99.Tables["MM0099"].Rows.Add();
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["EVENT_ID"] = "PM06";
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "WM00";
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["TRANSFER_PLAN_NO"] = twm42["TRANSFER_PLAN_NO"].ToString();
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_DESTION"] = twm41["MAT_DESTION"].ToString();
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["AIM_STORE"] = twm42["AIM_STOCK_NO"].ToString();
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_KIND"] = twm42["MAT_KIND"].ToString();
				bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_NO"] = twm42["MAT_NO"].ToString();
				doFlag = f_mm0099(&bcls_rec_mm99, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
#endif



				if (v_auto_confm.Trim() == "1")
				{

					twma1["MAT_NO"] = twm42["MAT_NO"];
					if (!twma1.Query())
					{
						CFormattable arguments[] = { twm42["MAT_NO"].ToString() }; // 定义参数列表的数组
						CMessageFormat::Format(s.msg, _RES("YM00S0000278")/*[{0}]的材料信息不存在。*/, arguments, 1);
						sprintf(s.msg, "[%s]的材料信息不存在。", (const char*)twm42["MAT_NO"]);
						throw CApplicationException(-1, s.msg, log.Location);
					}


					twma0.CopyFrom(twma1);
					twma0["REC_CREATOR"] = s.userid;
					twma0["REC_CREATE_TIME"] = datetime;
					twma0["REC_REVISOR"] = s.userid;
					twma0["REC_REVISE_TIME"] = datetime;
					twma0["STOCK_OPER_ORDER"] = "2G";//出库操作指示 2G-转库出库
					twma0["MAT_NO"] = twma1["MAT_NO"];
					twma0["PLAN_NO"] = twm42["TRANSFER_PLAN_NO"];
					twma0["STOCK_NO"] = twm42["STOCK_NO"];
					twma0["FROM_STOCK_NO"] = twm42["STOCK_NO"];
					twma0["TO_STOCK_NO"] = twm42["AIM_STOCK_NO"];
					twma0["OPER_FLAG"] = "I";
					twma0["STOCK_CONFM_FLAG"] = " ";
					twma0["VEHICLE_NO"] = "";

					row_update.Merge(twma0);


					doFlag = f_wm00_queue(&bcls_wm00, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}


					//////////////////////////////////修改材料的确认状态/////////////////////////////
					Log::Trace("", __FUNCTION__, "修改材料的确认状态");
					twm42["AFFIRM_MARK"] = "3";
					twm42.Update("AFFIRM_MARK", "TRANSFER_PLAN_NO, MAT_NO");
					Log::Trace("", __FUNCTION__, "twm42.MAT_NO = [{0}] twm42.AFFIRM_MARK = [{1}]",
						twm42["MAT_NO"].ToString(), twm42["AFFIRM_MARK"].ToString());
					twm41["TRANSFER_PLAN_NO"] = twm42["TRANSFER_PLAN_NO"];
					twm41["TRANSFER_STATUS"] = "8";//转库计划状态 8-计划执行 2-计划下发 4-计划确认 A-计划红冲
					twm41.Update("TRANSFER_STATUS", "TRANSFER_PLAN_NO");
				}
			}
		}

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

