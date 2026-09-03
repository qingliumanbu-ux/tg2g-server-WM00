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
//#include "twma0.h"
//#include "twma1.h"
//#include "twma2.h"
//#include "twma4.h"


//BM2_FUNCTION_IMPORT
int f_wmxx_stock_out_mmdel(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);
BM2_FUNCTION_IMPORT
int f_wmxx_stock_log(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);


BM2_FUNCTION_EXPORT
int f_wm00_mat_info_delete(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	int iRowCount_old = 0;
	int iRowCount_new = 0;

	CString v_eventid = "";
	CString v_stock_oper_order = "";
	CString s_shift_group = " ", s_shift_no = " ", s_operate_time = " ";

	CString v_mat_kind = "";
	CString table_name = "";

	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);

	/* ***** 定义表实体对象 ***** */
	//CTWMA1 twma1_old(conn);
	//CTWMA1 twma1_new(conn);
	//CTWMA2 twma2(conn);
	//CTWMA0 twma0(conn);
	CModel twma2("TWMA2");
	CModel twma0("TWMA0");
	CModel twma4("TWMA4");


	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	//调用仓库入库主函数
	EIClass bcls_stock_out;
	bcls_stock_out.Tables[0].set_TableName("WM_STOCK");
	bcls_stock_out.Tables[0].Columns.Add(twma0);
	bcls_stock_out.Tables[0].Columns.Add(twma2);
	bcls_stock_out.Tables[0].Rows.Clear();
	bcls_stock_out.Tables["WM_STOCK"].Rows.Add();

	//记录履历
	EIClass bcls_rec_stock_log;
	bcls_rec_stock_log.Tables[0].set_TableName("WM_STOCK_LOG");
	bcls_rec_stock_log.Tables[0].Rows.Clear();


	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("NEWMM_TABLE"))
		{
			sprintf(s.msg, "函数f_wm00_mat_info_delete中找不到接收块名[OLDMM_TABLE]");
			throw CApplicationException(-1, s.msg, log.Location);
		}


		for (int i = 0; i < bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count(); i++)
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

			twma1_old.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[i]);

			if (bcls_rec->Tables["NEWMM_TABLE"].Columns.Contains("EVENT_ID"))
			{
				v_eventid = bcls_rec->Tables["NEWMM_TABLE"].Rows[i]["EVENT_ID"].ToString().Trim();
			}

			Log::Trace("", __FUNCTION__, "传入参数 v_eventid\t[{0}]", v_eventid);
			Log::Trace("", __FUNCTION__, "传入参数 twma1_old.MAT_NO\t[{0}]", twma1_old["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "传入参数 twma1_old.MAT_LINE_TYPE\t[{0}]", twma1_old["MAT_LINE_TYPE"].ToString());

			//if (twma1_old.QueryCount("MAT_NO") != 1)
			//{
			//	sprintf(s.msg, "材料【%s】不存在。", (const char*)twma1_old["MAT_NO"]);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			//如果材料有库位，调用出库。删除
			twma2["MAT_NO"] = twma1_old["MAT_NO"];

			/*物料保证在所有删除之前，材料都已经做过出库，以下内容注释*/
			/*
			if (twma2.Query("MAT_NO"))
			{
				if (v_eventid.Trim() == "MM04")
				{
					v_stock_oper_order = "2M";
				}
				else if (v_eventid.Trim() == "MM05")
				{
					v_stock_oper_order = "2R";
				}
				else if (v_eventid.Trim() == "MM06" ||
					v_eventid.Trim() == "MM11" ||
					v_eventid.Trim() == "MM1A")
				{
					v_stock_oper_order = "2B";
				}
				else if (v_eventid.Trim() == "MM06" ||
					v_eventid.Trim() == "MM11" ||
					v_eventid.Trim() == "MM1A")
				{
					v_stock_oper_order = "2B";
				}
				else if (v_eventid.Trim() == "QM05")
				{
					v_stock_oper_order = "2R";
				}
				else
				{
					v_stock_oper_order = "2M";
				}

				//调用仓库出库主函数
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["MAT_KIND"] = twma1_old["MAT_KIND"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["MAT_LINE_TYPE"] = twma1_old["MAT_LINE_TYPE"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["MAT_NO"] = twma2["MAT_NO"];
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_OPER_ORDER"] = v_stock_oper_order;
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["ROWNO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["COLUMN_NO"] = " ";
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["LAYERNO"] = 0;
				bcls_stock_out.Tables["WM_STOCK"].Rows[0]["STOCK_PLACE_POSITION"] = " ";

				doFlag = f_wmxx_stock_out_mmdel(&bcls_stock_out, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

				twma2.Delete("MAT_NO");




				//获取班组班次信息/////////////////////////////////
				f_epep_get_shift_group("DEFAULT", datetime, s_shift_no, s_shift_group, conn);


				twma4.CopyFrom(twma1_old);
				twma4["STOCK_OPER_ORDER"] = v_stock_oper_order;
				twma4["STOCK_NO"] = twma2["STOCK_NO"];
				twma4["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["LAYERNO"] = twma2["LAYERNO"];
				twma4["FROM_STOCK_NO"] = twma2["STOCK_NO"];
				twma4["FROM_STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twma4["CRANE_NO"] = " ";
				twma4["VEHICLE_NO"] = " ";


				twma4["REC_CREATOR"] = s.userid;
				twma4["REC_CREATE_TIME"] = datetime;
				twma4["RESUME_SEQ_NO"] = CDateTime::Now().ToString("yyyyMMddHHmmssff6");
				twma4["EVENT_TIME"] = datetime;
				twma4["SHIFT_GROUP"] = s_shift_group;
				twma4["SHIFT_NO"] = s_shift_no;
				twma4["FUNC_ID"] = s.svc_name;
				twma4["CLIENT_IP"] = s.fore_ip;

				twma4.Insert();  ///写库位移动履历
			}
			*/

			//删除队列
			sqlstr =
				" DELETE FROM TWMA0"
				" WHERE MAT_NO = @mat_no";
			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("mat_no", twma1_old["MAT_NO"].ToString());
			comm.ExecuteNonQuery();

			//删除命令
			sqlstr =
				" DELETE FROM TWMA7"
				" WHERE MAT_NO = @mat_no";
			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("mat_no", twma1_old["MAT_NO"].ToString());
			comm.ExecuteNonQuery();

#if !defined(_SYS_PES) && !defined (_SYS_MES) && !defined (_SYS_MMS)
			twma1_old.Delete("MAT_NO");
#endif
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
