/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	仓库库位信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm20a3.h"

//函数申明

/*<remark>=========================================================
///<summary>
///库区定义信息查询
///<para>
///2.排序方式：STOCK_NO，STOCK_PLACE_NO
///</para>
///<para>数据库表：twm20a3 库区库位定义表；
///<returns>返回符合查询条件的库位信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm30a1_inq);

int f_wm30a1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;
	CString v_date_from = "";
	CString v_date_to = "";
	CString v_date = "";
	CDecimal v_end_wt = 0;

	CDateTime v_date_time_from;
	CDateTime v_date_time_to;


	/* 实体类定义 */
	//CTWM20A3 twm20a3(conn);
	CModel twm20a3 = CModel("TWM20A3");
	CModel twm01 = CModel("TWM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString s_userid("");
	CDecimal i_count = 0;
	CDecimal i_count1 = 0;

	/* 数据库操作类定义 */
	CDbCommand comm(conn);
	CDbCommand comm1(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;


		bcls_ret->Tables[0].set_TableName("库存量");
		bcls_ret->Tables["库存量"].Columns.Add(DT_STRING, "WORK_DATE");
		bcls_ret->Tables["库存量"].Columns.Add(DT_DECIMAL, "DAY_OUTPUT");
		bcls_ret->Tables["库存量"].Rows.Clear();


		bcls_ret->Tables.Add("最低库存量");
		bcls_ret->Tables["最低库存量"].Columns.Add(DT_STRING, "WORK_DATE");
		bcls_ret->Tables["最低库存量"].Columns.Add(DT_DECIMAL, "DAY_OUTPUT");
		bcls_ret->Tables["最低库存量"].Rows.Clear();

		bcls_ret->Tables.Add("最高库存量");
		bcls_ret->Tables["最高库存量"].Columns.Add(DT_STRING, "WORK_DATE");
		bcls_ret->Tables["最高库存量"].Columns.Add(DT_DECIMAL, "DAY_OUTPUT");
		bcls_ret->Tables["最高库存量"].Rows.Clear();


		twm01["STOCK_NO"] = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();
		v_date_from = bcls_rec->Tables[0].Rows[0]["DATE_FROM"].ToString();
		v_date_to = bcls_rec->Tables[0].Rows[0]["DATE_TO"].ToString();
		Log::Trace("", __FUNCTION__, "传入参数 twm01.STOCK_NO\t[{0}]", twm01["STOCK_NO"].ToString());
		Log::Trace("", __FUNCTION__, "传入参数 v_date_from\t[{0}]", v_date_from);
		Log::Trace("", __FUNCTION__, "传入参数 v_date_to\t[{0}]", v_date_to);

		v_date_time_from = CDateTime::Parse(v_date_from + "000000");
		v_date_time_to = CDateTime::Parse(v_date_to + "000000");

		Log::Trace("", __FUNCTION__, "v_date_time_from\t[{0}]", v_date_time_from.ToString("yyyyMMdd"));
		Log::Trace("", __FUNCTION__, "v_date_time_to\t[{0}]", v_date_time_to.ToString("yyyyMMdd"));
		twm01.Query("STOCK_NO");


		Log::Trace("", __FUNCTION__, "twm01.STOCK_WGT_STD\t[{0}]", twm01["STOCK_WGT_STD"].ToDecimal());
		Log::Trace("", __FUNCTION__, "twm01.STOCK_WGT_MAX\t[{0}]", twm01["STOCK_WGT_MAX"].ToDecimal());
		Log::Trace("", __FUNCTION__, "twm01.STOCK_WGT_MIN\t[{0}]", twm01["STOCK_WGT_MIN"].ToDecimal());


		i_count = 0;
		while (v_date < v_date_time_to.ToString("yyyyMMdd"))
		{
			v_date = v_date_time_from.AddDays(i_count.ToDouble()).ToString("yyyyMMdd");
			Log::Trace("", __FUNCTION__, "v_date\t[{0}]", v_date);

			v_end_wt = 0;

			sqlstr = " SELECT END_WT FROM TWM20BD"
				" WHERE STOCK_NO = @stock_no"
				" AND REPORT_DATE = @report_date";
			comm1.SetCommandText(sqlstr);
			comm1.Parameters.Set("stock_no", twm01["STOCK_NO"].ToString());
			comm1.Parameters.Set("report_date", v_date);
			v_end_wt = comm1.ExecuteScalar();

			bcls_ret->Tables["库存量"].Rows.Add();
			bcls_ret->Tables["库存量"].Rows[i_count.ToInt32()]["WORK_DATE"] = v_date;
			bcls_ret->Tables["库存量"].Rows[i_count.ToInt32()]["DAY_OUTPUT"] = i_count1;

			bcls_ret->Tables["最低库存量"].Rows.Add();
			bcls_ret->Tables["最低库存量"].Rows[i_count.ToInt32()]["WORK_DATE"] = v_date;
			bcls_ret->Tables["最低库存量"].Rows[i_count.ToInt32()]["DAY_OUTPUT"] = twm01["STOCK_WGT_MIN"].ToDecimal();

			bcls_ret->Tables["最高库存量"].Rows.Add();
			bcls_ret->Tables["最高库存量"].Rows[i_count.ToInt32()]["WORK_DATE"] = v_date;
			bcls_ret->Tables["最高库存量"].Rows[i_count.ToInt32()]["DAY_OUTPUT"] = twm01["STOCK_WGT_MAX"].ToDecimal();

			i_count = i_count + 1;

			if (i_count > 50)
			{
				break;
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
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