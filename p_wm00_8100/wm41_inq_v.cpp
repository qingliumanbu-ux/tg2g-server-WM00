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
BM2F_ENTERACE(wm41_inq_v)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_inq_v(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	int fetchRowCount = 0;
	int	ret = 0;
	int doFlag = 0;

	CString stock_no = "";
	CString factory_div = "";
	CDbCommand cmd_inq(conn);
	CString sqlstr;
	try
	{
		stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"].ToString().Trim();
		factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		/*获得传入参数*/
		sqlstr =
			"SELECT DISTINCT VEHICLE_NO,TD_TYPE,TRNP_MODE_CODE,TRANSFER_PLAN_NO,VEHICLE_SEQ_NO,CARRY_COMPANY_NAME,CARRY_COMPANY_CODE  FROM  TWMB5  "
			"WHERE STOCK_NO = @STOCK_NO  ";
		cmd_inq.Parameters.Set("STOCK_NO", stock_no);
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "stock_no = [{0}],sqlstr = [{1}]", stock_no, sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STATUS");
		if (factory_div == "A1" ||
			factory_div == "A2" ||
			factory_div == "A3" ||
			factory_div == "A6")
		{
			sqlstr =
				" SELECT STATUS "
				" FROM TWM00B1 "
				" WHERE 1=1 "
				" AND VEHICLE_NO = @VEHICLE_NO "
				" AND TD_TYPE = @TD_TYPE "
				" AND TRNP_MODE_CODE = @TRNP_MODE_CODE "
				" AND TRANSFER_PLAN_NO = @TRANSFER_PLAN_NO "
				" AND VEHICLE_SEQ_NO = @VEHICLE_SEQ_NO "
				" AND CARRY_COMPANY_NAME = @CARRY_COMPANY_NAME ";
			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
			{
				cmd_inq.Parameters.Set("VEHICLE_NO", bcls_ret->Tables[0].Rows[i]["VEHICLE_NO"].ToString());
				cmd_inq.Parameters.Set("TD_TYPE", bcls_ret->Tables[0].Rows[i]["TD_TYPE"].ToString());
				cmd_inq.Parameters.Set("TRNP_MODE_CODE", bcls_ret->Tables[0].Rows[i]["TRNP_MODE_CODE"].ToString());
				cmd_inq.Parameters.Set("TRANSFER_PLAN_NO", bcls_ret->Tables[0].Rows[i]["TRANSFER_PLAN_NO"].ToString());
				cmd_inq.Parameters.Set("VEHICLE_SEQ_NO", bcls_ret->Tables[0].Rows[i]["VEHICLE_SEQ_NO"].ToDecimal());
				cmd_inq.Parameters.Set("CARRY_COMPANY_NAME", bcls_ret->Tables[0].Rows[i]["CARRY_COMPANY_NAME"].ToString());
				cmd_inq.Parameters.Set("CARRY_COMPANY_CODE", bcls_ret->Tables[0].Rows[i]["CARRY_COMPANY_CODE"].ToString());
				//cmd_inq.Parameters.Set("LANE_NO", bcls_ret->Tables[0].Rows[i]["LANE_NO"].ToString());
				Log::Trace("", __FUNCTION__, "VEHICLE_NO[{0}]", bcls_ret->Tables[0].Rows[i]["VEHICLE_NO"].ToString());
				Log::Trace("", __FUNCTION__, "TD_TYPE[{0}]", bcls_ret->Tables[0].Rows[i]["TD_TYPE"].ToString());
				Log::Trace("", __FUNCTION__, "TRNP_MODE_CODE[{0}]", bcls_ret->Tables[0].Rows[i]["TRNP_MODE_CODE"].ToString());
				Log::Trace("", __FUNCTION__, "TRANSFER_PLAN_NO[{0}]", bcls_ret->Tables[0].Rows[i]["TRANSFER_PLAN_NO"].ToString());
				Log::Trace("", __FUNCTION__, "VEHICLE_SEQ_NO[{0}]", bcls_ret->Tables[0].Rows[i]["VEHICLE_SEQ_NO"].ToDecimal());
				Log::Trace("", __FUNCTION__, "CARRY_COMPANY_NAME[{0}]", bcls_ret->Tables[0].Rows[i]["CARRY_COMPANY_NAME"].ToString());
				Log::Trace("", __FUNCTION__, "CARRY_COMPANY_CODE[{0}]", bcls_ret->Tables[0].Rows[i]["CARRY_COMPANY_CODE"].ToString());
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					bcls_ret->Tables[0].Rows[i]["STATUS"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
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
	cmd_inq.Close();

	return doFlag;

}
