/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2013
Author:     ljnie
Version:    1.0
Date:       2016/7/13 15:56:29
Description: 车辆来料信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 车辆来料信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(wma3_inq) 

int f_wma3_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString	datetime("");
	CDecimal totalCount = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CString s_userid("");

	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{

		// 获取前台传入参数
		s_userid = s.userid;
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		CString trans_tool = bcls_rec->Tables[0].Rows[0]["TRANS_TOOL"].ToString().Trim();
		CString vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();
		CString mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();

		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"]; //每页记录数
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];       //需查询的页号

		Log::Trace("", __FUNCTION__, "传入参数 trans_tool					= [{0}]", trans_tool);
		Log::Trace("", __FUNCTION__, "传入参数 vehicle_no					= [{0}]", vehicle_no);
		Log::Trace("", __FUNCTION__, "传入参数 mat_no				= [{0}]", mat_no);
		Log::Trace("", __FUNCTION__, "传入参数 record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "传入参数 current_page_no		= [{0}]", current_page_no);

		/* 检查输入参数合法性 */

		//设备、车号、材料号、车上位置、层号；厚宽长重
		sqlstr = " SELECT a.TRANS_TOOL,a.VEHICLE_NO,a.MAT_NO,a.STOCK_PLACE_POSITION,a.LAYERNO, "
			" b.MAT_THICK,b.MAT_WIDTH,b.MAT_LEN,b.MAT_ACT_WT "
			" FROM TWMA0 a,TWMA1 b WHERE a.MAT_NO = b.MAT_NO "
			;

		if (trans_tool != "")
		{
			sqlstr += " AND a.TRANS_TOOL = @trans_tool ";
		}
		if (vehicle_no != "")
		{
			sqlstr += " AND a.VEHICLE_NO LIKE @vehicle_no ";
		}
		if (mat_no != "")
		{
			sqlstr += " AND a.MAT_NO LIKE @mat_no ";
		}

		cmd_inq.Parameters.Set("userid", s_userid);
		cmd_inq.Parameters.Set("trans_tool", trans_tool);
		cmd_inq.Parameters.Set("vehicle_no", vehicle_no + "%");
		cmd_inq.Parameters.Set("mat_no", mat_no + "%");

		sqlstr_count = "SELECT COUNT(1) FROM (" + sqlstr + ") T ";
		Log::Trace("", __FUNCTION__, "sqlstr_count = [{0}]", sqlstr_count);
		cmd_inq.SetCommandText(sqlstr_count);
		totalCount = cmd_inq.ExecuteScalar();
		Log::Trace("", __FUNCTION__, "totalCount = [{0}]", totalCount);
		cmd_inq.Close();

		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > totalCount.ToDouble())
		{
			start_row = 0;
		}
		sqlstr += " ORDER BY TRANS_TOOL,VEHICLE_NO,STOCK_PLACE_POSITION,LAYERNO,MAT_NO ASC ";
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = totalCount.ToInt32();
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
		