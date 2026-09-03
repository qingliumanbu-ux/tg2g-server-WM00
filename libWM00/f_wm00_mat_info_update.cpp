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
//#include "twma1.h"
//#include "twma2.h"
//#include "twma4.h"




BM2_FUNCTION_IMPORT
int f_wmxx_stock_log(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);


BM2_FUNCTION_EXPORT
int f_wm00_mat_info_update(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	int iRowCount_old = 0;
	int iRowCount_new = 0;

	CString v_eventid = "";
	CString table_name = "";
	CString v_mat_kind = "";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);

	/* ***** 定义表实体对象 ***** */
	//CTWMA1 twma1_old(conn);
	//CTWMA1 twma1_new(conn);
	//CTWMA2 twma2(conn);
	//CTWMA4 twma4(conn);

	CModel twma2("TWMA2");
	CModel twma4("TWMA4");
	//CModel twma1_new("TWMA2");
	//CModel twma1_old("TWMA2");



	//记录履历
	EIClass bcls_rec_stock_log;
	bcls_rec_stock_log.Tables[0].set_TableName("WM_STOCK_LOG");
	bcls_rec_stock_log.Tables[0].Rows.Clear();


	/* ***** 应用程序开始处理 ***** */
	try
	{

		if (!bcls_rec->Tables.Contains("NEWMM_TABLE"))
		{
			sprintf(s.msg, "函数f_wm00_mat_info_update中找不到接收块名[NEWMM_TABLE]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		iRowCount_old = bcls_rec->Tables["OLDMM_TABLE"].Rows.get_Count();
		iRowCount_new = bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count();



		for (int i = 0; i < iRowCount_new; i++)
		{
			if (bcls_rec->Tables["NEWMM_TABLE"].Columns.Contains("MAT_KIND"))
			{
				v_mat_kind = bcls_rec->Tables["NEWMM_TABLE"].Rows[i]["MAT_KIND"].ToString().Trim();
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

			CModel twma1_old(table_name);
			CModel twma1_new(table_name);

			if (!bcls_rec->Tables.Contains("OLDMM_TABLE"))
			{
				bcls_rec->Tables.Add("OLDMM_TABLE");
				bcls_rec->Tables["OLDMM_TABLE"].Columns.Add(twma1_old);
				bcls_rec->Tables["OLDMM_TABLE"].Rows.Clear();
			}

			twma1_new.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[i]);

			for (int j = 0; j < iRowCount_old; j++)
			{
				if (twma1_new["MAT_NO"].ToString() == bcls_rec->Tables["OLDMM_TABLE"].Rows[j]["MAT_NO"].ToString())
				{
					//twma1_old(table_name);
					twma1_old.Reset();
					twma1_old.MergeFrom(bcls_rec->Tables["OLDMM_TABLE"].Rows[j]);
				}
			}
			if (twma1_old["MAT_NO"].ToString().Trim() == "")
			{
				twma1_old["MAT_NO"] = twma1_new["MAT_NO"];
				twma1_old.Query("MAT_NO");
			}

			Log::Trace("", __FUNCTION__, "5");
			if (bcls_rec->Tables["NEWMM_TABLE"].Columns.Contains("EVENT_ID"))
			{
				v_eventid = bcls_rec->Tables["NEWMM_TABLE"].Rows[i]["EVENT_ID"].ToString().Trim();
			}
			Log::Trace("", __FUNCTION__, "传入参数 v_eventid\t[{0}]", v_eventid);



			if (twma1_old["MAT_WT"] == twma1_new["MAT_WT"] &&
				twma1_old["MAT_ACT_WT"] == twma1_new["MAT_ACT_WT"] &&
				twma1_old["MAT_THEORY_WT"] == twma1_new["MAT_THEORY_WT"] &&
				twma1_old["MAT_THICK"] == twma1_new["MAT_THICK"] &&
				twma1_old["MAT_WIDTH"] == twma1_new["MAT_WIDTH"] &&
				twma1_old["MAT_ACT_THICK"] == twma1_new["MAT_ACT_THICK"] &&
				twma1_old["MAT_ACT_WIDTH"] == twma1_new["MAT_ACT_WIDTH"] &&
				twma1_old["MAT_LEN"] == twma1_new["MAT_LEN"] &&
				twma1_old["MAT_ACT_LEN"] == twma1_new["MAT_ACT_LEN"] &&
				twma1_old["SG_SIGN"] == twma1_new["SG_SIGN"] &&
				twma1_old["SG_STD"] == twma1_new["SG_STD"] &&
				twma1_old["ST_NO"] == twma1_new["ST_NO"])
			{
				Log::Trace("", __FUNCTION__, "没有变化。");
				continue;
			}
			Log::Trace("", __FUNCTION__, "aaaaaaaaaaaaaaaaaaaaa");

			twma2["MAT_NO"] = twma1_new["MAT_NO"];
			if (!twma2.Query("MAT_NO"))
			{
				Log::Trace("", __FUNCTION__, "材料未入库。");
				continue;
			}
			Log::Trace("", __FUNCTION__, "aaaaaaaaaaaaaaaaaaaaa");

			if (twma1_old["MAT_WT"] != twma1_new["MAT_WT"] ||
				twma1_old["MAT_ACT_WT"] != twma1_new["MAT_ACT_WT"] ||
				twma1_old["MAT_THEORY_WT"] != twma1_new["MAT_THEORY_WT"])
			{
				Log::Trace("", __FUNCTION__, "aaaaaaaaaaaaaaaaaaaaa");
				twma4.Reset();
				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = "2J";
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["VEHICLE_NO"] = twma2["VEHICLE_NO"];

				twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);

				twma1_old["MAT_WT"] = twma1_new["MAT_WT"];
				twma1_old["MAT_ACT_WT"] = twma1_new["MAT_ACT_WT"];
				twma1_old["MAT_THEORY_WT"] = twma1_new["MAT_THEORY_WT"];


				twma4.Reset();
				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = "1J";
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["VEHICLE_NO"] = twma2["VEHICLE_NO"];

				twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			}

			if (twma1_old["MAT_THICK"] != twma1_new["MAT_THICK"] ||
				twma1_old["MAT_WIDTH"] != twma1_new["MAT_WIDTH"] ||
				twma1_old["MAT_ACT_THICK"] != twma1_new["MAT_ACT_THICK"] ||
				twma1_old["MAT_ACT_WIDTH"] != twma1_new["MAT_ACT_WIDTH"] ||
				twma1_old["MAT_LEN"] != twma1_new["MAT_LEN"] ||
				twma1_old["MAT_ACT_LEN"] != twma1_new["MAT_ACT_LEN"])
			{
				twma4.Reset();
				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = "2I";
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["VEHICLE_NO"] = twma2["VEHICLE_NO"];

				twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);

				twma1_old["MAT_THICK"] = twma1_new["MAT_THICK"];
				twma1_old["MAT_WIDTH"] = twma1_new["MAT_WIDTH"];
				twma1_old["MAT_ACT_THICK"] = twma1_new["MAT_ACT_THICK"];
				twma1_old["MAT_ACT_WIDTH"] = twma1_new["MAT_ACT_WIDTH"];
				twma1_old["MAT_LEN"] = twma1_new["MAT_LEN"];
				twma1_old["MAT_ACT_LEN"] = twma1_new["MAT_ACT_LEN"];


				twma4.Reset();
				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = "1I";
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["VEHICLE_NO"] = twma2["VEHICLE_NO"];

				twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			}

			if (twma1_old["SG_SIGN"] != twma1_new["SG_SIGN"] ||
				twma1_old["SG_STD"] != twma1_new["SG_STD"] ||
				twma1_old["ST_NO"] != twma1_new["ST_NO"])
			{
				twma4.Reset();
				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = "2H";
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["VEHICLE_NO"] = twma2["VEHICLE_NO"];

				twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);

				twma1_old["SG_SIGN"] = twma1_new["SG_SIGN"];
				twma1_old["SG_STD"] = twma1_new["SG_STD"];
				twma1_old["ST_NO"] = twma1_new["ST_NO"];

				twma4.Reset();
				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = "1H";
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["VEHICLE_NO"] = twma2["VEHICLE_NO"];

				twma4.MergeTo(bcls_rec_stock_log.Tables["WM_STOCK_LOG"], false);
			}



			//更新TWMA1
#if !defined(_SYS_PES) && !defined(_SYS_MES) && !defined(_SYS_MMS)
			twma1_new.Update(
				"MAT_WT,"
				"MAT_ACT_WT,"
				"MAT_THEORY_WT,"
				"MAT_THICK,"
				"MAT_WIDTH,"
				"MAT_ACT_THICK,"
				"MAT_ACT_WIDTH,"
				"MAT_LEN,"
				"MAT_ACT_LEN,"
				"SG_SIGN,"
				"SG_STD,"
				"ST_NO"
				,
				"MAT_NO");
#endif
		}

		//调用履历函数
		if (bcls_rec_stock_log.Tables["WM_STOCK_LOG"].Rows.get_Count() > 0)
		{
			doFlag = f_wmxx_stock_log(&bcls_rec_stock_log, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
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
