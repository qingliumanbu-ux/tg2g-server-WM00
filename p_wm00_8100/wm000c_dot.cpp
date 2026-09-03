/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         zcx
Version:		1.0
Date:			2016-09-27
Description:	业务步骤配置信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm000c.h"

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//函数申明

/*<remark>=========================================================
///<summary>
///业务步骤配置信息查询
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWM000C 业务步骤配置信息表；
///<returns>返回符合查询条件的配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm000c_dot);

int f_wm000c_dot(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM000C twm000c(conn);
	CModel twm000c = CModel("TWM000C");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 全局变量 */
	CString stock_no = "";
	CString stock_oper_order = "";
	CString unit_code = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand comm(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 999999999;  //每页记录数量
		}

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);

		//获取前台传入参数
		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"];
		stock_oper_order = bcls_rec->Tables[0].Rows[0]["STOCK_OPER_ORDER"];
		unit_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"];

		Log::Trace("", __FUNCTION__, "stock_no = [{0}]", stock_no);
		Log::Trace("", __FUNCTION__, "stock_oper_order = [{0}]", stock_oper_order);
		Log::Trace("", __FUNCTION__, "unit_code = [{0}]", unit_code);

		if (stock_no.Trim() != "")
		{
			sqlwhere += " AND STOCK_NO LIKE @stock_no ";
		}
		if (stock_oper_order.Trim() != "")
		{
			sqlwhere += " AND STOCK_OPER_ORDER = @stock_oper_order";
		}
		if (unit_code.Trim() != "")
		{
			sqlwhere += " AND UNIT_CODE = @unit_code";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM twm000c WHERE 1=1 ";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", "%" + stock_no + "%");
		cmd_inq.Parameters.Set("stock_oper_order", stock_oper_order);
		cmd_inq.Parameters.Set("unit_code", unit_code);

		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT * FROM twm000c WHERE 1=1 ";
			break;
		}
		sqlwhere += " ORDER BY STOCK_NO ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", "%" + stock_no + "%");
		cmd_inq.Parameters.Set("stock_oper_order", stock_oper_order);
		cmd_inq.Parameters.Set("unit_code", unit_code);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;

			if (fetchRowCount > (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				Log::Trace("", __FUNCTION__, "超上线，break");
				break;
			}
			if (!((fetchRowCount > pageInfo.RecordFrom) &&
				(fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{
				continue;
			}

			twm000c.Reset();
			cmd_inq.Fetch(twm000c);

			if (twm000c["STOCK_PLACE_TYPE_FROM"].ToString().Trim() != "")
			{
				sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TEP0002"
					" WHERE CODE_CLASS = 'WM03'"
					" AND CODE = @code";
				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("code", twm000c["STOCK_PLACE_TYPE_FROM"].ToString());
				comm.ExecuteReader();
				if (comm.Read())
				{
					twm000c["STOCK_PLACE_TYPE_FROM"] = comm.GetString(1);
				}
				comm.Close();
			}

			if (twm000c["STOCK_PLACE_TYPE_TO"].ToString().Trim() != "")
			{
				sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TEP0002"
					" WHERE CODE_CLASS = 'WM03'"
					" AND CODE = @code";
				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("code", twm000c["STOCK_PLACE_TYPE_TO"].ToString());
				comm.ExecuteReader();
				if (comm.Read())
				{
					twm000c["STOCK_PLACE_TYPE_TO"] = comm.GetString(1);
				}
				comm.Close();
			}

			if (twm000c["DEV_DIV_FROM"].ToString().Trim() != "")
			{
				sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TEP0002"
					" WHERE CODE_CLASS = 'WM11'"
					" AND CODE = @code";
				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("code", twm000c["DEV_DIV_FROM"].ToString());
				comm.ExecuteReader();
				if (comm.Read())
				{
					twm000c["DEV_DIV_FROM"] = comm.GetString(1);
				}
				comm.Close();
			}

			if (twm000c["DEV_DIV_TO"].ToString().Trim() != "")
			{
				sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TEP0002"
					" WHERE CODE_CLASS = 'WM11'"
					" AND CODE = @code";
				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("code", twm000c["DEV_DIV_TO"].ToString());
				comm.ExecuteReader();
				if (comm.Read())
				{
					twm000c["DEV_DIV_TO"] = comm.GetString(1);
				}
				comm.Close();
			}


			twm000c.MergeTo(bcls_ret->Tables[0], false);
		}
		cmd_inq.Close();

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
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
