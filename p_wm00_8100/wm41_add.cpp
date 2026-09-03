/*=========================================================================
//程序名称:		wm41_add
//隶属子系统:	WM
//产品名称:		装车材料增加
//创建人员:
//创建时间:		2022-09-05
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//程序用头文件

// service入口
BM2F_ENTERACE(wm41_add)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_add(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	int	ret = 0;
	int	doFlag = 0;
	CString mat_no = "";
	CString plan_status = "";
	CString transfer_plan_no = "";
	CString delivy_plan_type = "";
	CString trnp_mode_code = "";
	CString vehicle_no = "";
	CString td_type = "";
	CDecimal load_theory_wt = 0;
	CDecimal load_act_wt = 0;
	CDecimal load_num = 0;

	CString datetime = "";
	/*实体对象*/
	CModel twmb5  ("TWMB5");
	/* ***** 数据库操作类定义 ***** */
	CString sqlstr;
	CDbCommand cmd_inq(conn);

	try
	{

		//考虑到是所有记录是同一个提单号，用第一个即可
		vehicle_no = bcls_rec->Tables[1].Rows[0]["VEHICLE_NO"].ToString().Trim();
		transfer_plan_no = bcls_rec->Tables[1].Rows[0]["TRANSFER_PLAN_NO"].ToString().Trim();
		sqlstr =
			" SELECT VEHICLE_NO,TD_TYPE,TRNP_MODE_CODE FROM TWM00B1 WHERE VEHICLE_NO=@VEHICLE_NO  ";//AND TD_TYPE = 'WL'
		cmd_inq.Parameters.Set("VEHICLE_NO", vehicle_no);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			trnp_mode_code = cmd_inq.GetString(3);
		}
		else
		{
			sprintf(s.msg, "该车辆不在车辆表中");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();

		
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmb5["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
			if (twmb5.Query("MAT_NO"))
			{
				sprintf(s.msg, "此材料号已装车[{0}]", mat_no);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			twmb5.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twmb5.MergeFrom(bcls_rec->Tables[1].Rows[0]);
			twmb5["VEHICLE_NO"] = vehicle_no;
			twmb5["REC_CREATOR"] = s.userid;
			twmb5["REC_CREATE_TIME"] = datetime;
			twmb5["REC_REVISOR"] = s.userid;
			twmb5["REC_REVISE_TIME"] = datetime;
			twmb5.Insert();
			
		}
		twmb5["VEHICLE_NO"] = vehicle_no;
		Log::Trace("","","count=[{0}]",twmb5.QueryCount("VEHICLE_NO"));
		if (trnp_mode_code == "1")
		{
			
			Log::Trace("", "", "111");
			sqlstr =
				" SELECT  NVL(SUM(MAT_THEORY_WT),0) LOAD_THEORY_WT,NVL(SUM(MAT_ACT_WT),0) LOAD_ACT_WT,NVL(COUNT(*),0) LOAD_NUM "
				" FROM	TWMB5 "
				" WHERE	1=1 "
				//" AND order_no	=	@v_order_no "
				" AND transfer_plan_no	=	@v_transfer_plan_no ";
			//cmd_inq.Parameters.Set("v_order_no", tsm0003["ORDER_NO"].ToString());
			cmd_inq.Parameters.Set("v_transfer_plan_no", transfer_plan_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();
			Log::Trace("", "", "1={0},2={0}", bcls_ret->Tables[0].Rows.get_Count(), transfer_plan_no);
			Log::Trace("", "", "LOAD_THEORY_WT={0},{1},{2}", bcls_ret->Tables[0].Rows[0]["LOAD_THEORY_WT"].ToString(), bcls_ret->Tables[0].Rows[0]["LOAD_ACT_WT"].ToString(), bcls_ret->Tables[0].Rows[0]["LOAD_NUM"].ToString());

			//bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DELIVY_WT");
			//if (bcls_ret->Tables[0].Rows.get_Count() > 0)
			//{
			//	bcls_ret->Tables[0].Rows[0]["DELIVY_WT"] = tsm0003["DELIVY_WT"];
			//}
		}
		else
		{
			CDecimal load_theory_wt = 0;
			CDecimal load_act_wt = 0;
			CDecimal load_num = 0;
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "LOAD_THEORY_WT");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "LOAD_ACT_WT");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "LOAD_NUM");
			bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "DELIVY_WT");
			bcls_ret->Tables[0].Rows.Add();
			sqlstr =
				" SELECT  NVL(SUM(MAT_THEORY_WT),0) LOAD_THEORY_WT,NVL(SUM(MAT_ACT_WT),0) LOAD_ACT_WT,NVL(COUNT(*),0) LOAD_NUM"
				" FROM	TWMB5 "
				" WHERE	1=1 "
				//" AND order_no	=	@v_order_no "
				" AND transfer_plan_no	=	@v_transfer_plan_no ";
				//" UNION ALL "
				//" SELECT  NVL(SUM(MAT_THEORY_WT),0) LOAD_THEORY_WT,NVL(SUM(MAT_ACT_WT),0) LOAD_ACT_WT,NVL(COUNT(*),0) LOAD_NUM"
				//" FROM	TSM00B3 "
				//" WHERE	1=1 "
				//" AND order_no	=	@v_order_no "
				//" AND bill_of_lading_no	=	@v_bill_of_lading_no ";
			//cmd_inq.Parameters.Set("v_order_no", tsm0003["ORDER_NO"].ToString());
			cmd_inq.Parameters.Set("v_transfer_plan_no", transfer_plan_no);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				load_theory_wt = load_theory_wt + cmd_inq.GetDecimal(1);
				load_act_wt = load_act_wt + cmd_inq.GetDecimal(2);
				load_num = load_num + cmd_inq.GetDecimal(3);
			}
			cmd_inq.Close();
			bcls_ret->Tables[0].Rows[0]["LOAD_THEORY_WT"] = load_theory_wt;
			bcls_ret->Tables[0].Rows[0]["LOAD_ACT_WT"] = load_act_wt;
			bcls_ret->Tables[0].Rows[0]["LOAD_NUM"] = load_num;
			//bcls_ret->Tables[0].Rows[0]["DELIVY_WT"] = tsm0003["DELIVY_WT"];
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
