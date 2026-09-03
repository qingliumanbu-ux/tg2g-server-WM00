/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2011
Author:			wuxin
Version:		1.0
Date:			2014-12-24
Description:	层号计算函数
**************************************************/

#include "stdafx.h"
//#include "twm04.h"
//#include "twma2.h"
 
BM2_FUNCTION_EXPORT
int f_wm00_cal_layerno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	CString stock_place_no = "", stock_no = " ";
	CDecimal layerno = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);
	//定义表实体对象
	//CTWMA2 twma2(conn);
	//CTWM04 twm04(conn);
	CModel twma2("TWMA2");
	CModel twm04("TWM04");

	bcls_ret->Tables[0].Clear();
	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "LAYERNO");

	try
	{
		//计算层数
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			sqlstr =
				" SELECT nvl(MAX(layerno),0) + 1"
				" FROM TWMA2"
				" WHERE STOCK_PLACE_NO = @stock_place_no";
				//" AND stock_no = @stock_no ";
			break;
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
			sqlstr =
				" SELECT isnull(MAX(layerno),0) + 1"
				" FROM TWMA2"
				" WHERE STOCK_PLACE_NO = @stock_place_no";
			//" AND stock_no = @stock_no ";
			break;
		case DB_KIND_ORACLE:	    // Oracle 数据库
			sqlstr =
				" SELECT nvl(MAX(layerno),0) + 1"
				" FROM TWMA2"
				" WHERE STOCK_PLACE_NO = @stock_place_no";
			//" AND stock_no = @stock_no ";
			break;
		default: // 所有数据库适用，通用SQL语句					
			sqlstr =
				" SELECT nvl(MAX(layerno),0) + 1"
				" FROM TWMA2 WHERE STOCK_PLACE_NO = @stock_place_no";
			//" AND stock_no = @stock_no ";
			break;
		}


		///////////////////////////////////////////////////获取输入参数///////////////////////////////////////////// 
		stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString();
		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString();

		twm04["STOCK_NO"] = stock_no;
		twm04["STOCK_PLACE_NO"] = stock_place_no;

		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			sprintf(s.msg, "库位号不存在");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//////////////////////////////////////////////////////////按垛管理, 不跟踪到层////////////////////////////
		if (twm04["MANAGE_ACCU"].ToString() == "2" ||
			twm04["MANAGE_ACCU"].ToString() == "4")
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LAYERNO"] = 1;
		}
		/////////////////////////////////////////////////////////一品一地/////////////////////////////////////////
		if (twm04["MANAGE_ACCU"].ToString() == "5")
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LAYERNO"] = twm04["LAYERNO"];
		}

		/////////////////////////////////////////////////////////按垛管理, 跟踪到层//////////////////////////////
		if (twm04["MANAGE_ACCU"].ToString() == "1")
		{
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_place_no", stock_place_no);
			cmd_inq.Parameters.Set("stock_no", stock_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				layerno = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();
			Log::Trace("", __FUNCTION__, "layerno=[{0}] ", layerno);
			Log::Trace("", __FUNCTION__, "twm04.MAX_LAYER_COUNT=[{0}] ", twm04["MAX_LAYER_COUNT"].ToDecimal());
			if (layerno > twm04["MAX_LAYER_COUNT"].ToDecimal())
			{
				sprintf(s.msg, "超过[%s]""最大堆放层号！", (const char*)twm04["STOCK_PLACE_NO"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LAYERNO"] = layerno;
		}
		/////////////////////////////////////////////////////////按列管理, 不跟踪到层//////////////////////////////
		if (twm04["MANAGE_ACCU"].ToString() == "3")
		{
			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[0]["LAYERNO"] = 1;
		}
	}
	/*捕获数据库操作异常*/
	catch (CDbException& ex)
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
	/*捕获应用错误*/
	catch (CApplicationException& ex)
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

	s.flag = doFlag;

	return(doFlag);
}

