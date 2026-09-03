/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      E84103
Version:     1.0
Date:        2020-11-11 09:39:03
Description: 车辆信息表查询
**************************************************/
#include "stdafx.h"

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//程序用头文件
// service入口
BM2F_ENTERACE(wm41_inq_v_z)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_inq_v_z(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	int fetchRowCount = 0;
	int	ret = 0;
	int doFlag = 0;

	CString	v_vehicle_no = "";
	CString	v_transfer_plan_no = "";
	CString	v_trnp_mode_code = "";
	CString	v_delivy_plan_type = "";
	CString factory_div = "";
	CString	stock_no = "";
	CString	in_stock_code = "";
	CString	out_stock_code = "";
	CString	carry_company_code = "";
	CString	carry_company_name = "";
	CString	v_order_no = "";
	CDbCommand cmd_inq(conn), cmd(conn);
	CString sqlstr, sql;
	try
	{

		/*获得传入参数*/
		v_trnp_mode_code = bcls_rec->Tables[0].Rows[0]["trnp_mode_code"].ToString().Trim();		 //运输方式
		//v_delivy_plan_type = bcls_rec->Tables[0].Rows[0]["delivy_plan_type"].ToString().Trim();	 //发货类型
		v_transfer_plan_no = bcls_rec->Tables[0].Rows[0]["transfer_plan_no"].ToString().Trim();//转库计划号
		factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
		stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"].ToString().Trim();
		//in_stock_code = bcls_rec->Tables[0].Rows[0]["in_stock_code"].ToString().Trim();
		//out_stock_code = bcls_rec->Tables[0].Rows[0]["out_stock_code"].ToString().Trim();
		//carry_company_code = bcls_rec->Tables[0].Rows[0]["carry_company_code"].ToString().Trim();
		//carry_company_name = bcls_rec->Tables[0].Rows[0]["carry_company_name"].ToString().Trim();
		//v_order_no = bcls_rec->Tables[0].Rows[0]["order_no"].ToString().Trim();//合同号
		if (v_trnp_mode_code == "11" || v_trnp_mode_code == "21" || v_trnp_mode_code == "1")//尾号为1的为汽运
		{
			v_trnp_mode_code = "1";
			sqlstr =
				" SELECT VEHICLE_NO, TD_TYPE, TRNP_MODE_CODE, IN_PLAN_NO, VEHICLE_SEQ_NO, STATUS,CARRY_COMPANY_CODE,CARRY_COMPANY_NAME,CARRY_WT,CARRY_SUM FROM TWM00B1 WHERE 1 = 1 "
				" AND td_type='ZT' "
				" AND trnp_mode_code = @trnp_mode_code "
				" AND in_plan_no = @transfer_plan_no "
				" AND status != '0' "//没有确认的车辆不能查询
				;
			if (v_delivy_plan_type != "1")
			{
				//sqlstr += "AND order_no = @order_no ";
			}

			cmd_inq.Parameters.Set("trnp_mode_code", v_trnp_mode_code);
			cmd_inq.Parameters.Set("transfer_plan_no", v_transfer_plan_no);
			//cmd_inq.Parameters.Set("order_no", v_order_no);
		}
		else if (v_trnp_mode_code == "12" || v_trnp_mode_code == "22")//尾号为2的为铁运
		{
			v_trnp_mode_code = "2";
			sqlstr =
				" SELECT VEHICLE_NO,TD_TYPE,TRNP_MODE_CODE,LANE_NO,STATUS  FROM TWM00B1 WHERE 1=1  "
				" AND STATUS >= '2'  AND (NO_LOAD_FLAG = '' OR NO_LOAD_FLAG = '0') "
				" AND USE_MARK = '0'  AND VEHICLE_KEY!= 'N' "
				" AND TRNP_MODE_CODE = @TRNP_MODE_CODE "
				" AND STOCK_NO = @STOCK_NO ";
			cmd_inq.Parameters.Set("STOCK_NO", stock_no);
			cmd_inq.Parameters.Set("TRNP_MODE_CODE", v_trnp_mode_code);
		}
		else
		{
			v_trnp_mode_code = "1";
			sqlstr =
				" SELECT VEHICLE_NO,TD_TYPE,TRNP_MODE_CODE,CARRY_COMPANY_CODE,CARRY_COMPANY_NAME,STATUS,CARRY_WT,CARRY_SUM  FROM TWM00B1 WHERE 1=1 "
				" AND TD_TYPE='WL' "
				//" AND STOCK_NO=@STOCK_NO "
				" AND TRNP_MODE_CODE = @TRNP_MODE_CODE ";
				//" AND CARRY_COMPANY_CODE = @CARRY_COMPANY_CODE "
				//" AND CARRY_COMPANY_NAME = @CARRY_COMPANY_NAME ";
			cmd_inq.Parameters.Set("CARRY_COMPANY_CODE", carry_company_code);
			cmd_inq.Parameters.Set("CARRY_COMPANY_NAME", carry_company_name);
			//cmd_inq.Parameters.Set("STOCK_NO", stock_no);
			cmd_inq.Parameters.Set("TRNP_MODE_CODE", v_trnp_mode_code);
		}
		Log::Info("", __FUNCTION__, "sqlstr = [{0}],v_trnp_mode_code = [{1}],transfer_plan_no= [{2}]", sqlstr, v_trnp_mode_code, v_transfer_plan_no);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
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
	cmd_inq.Close();

	return doFlag;

}
