/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         zcx
Version:		1.0
Date:			2016-09-27
Description:	物料调拨（流转）电文号配置信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm0b.h"


//函数申明

/*<remark>=========================================================
///<summary>
///物料调拨（流转）电文号配置信息查询
///<para>
///2.排序方式：FROM_STOCK_NO
///</para>
///<para>数据库表：TWM0B 物料调拨（流转）电文号配置表；
///<returns>返回符合查询条件的物料调拨（流转）电文号配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm0b_inq);

int f_wm0b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM0B twm0b(conn);
	CModel twm0b = CModel("TWM0B");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 全局变量 */
	CString from_stockNo = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

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
			pageInfo.PageSize = -1;  //每页记录数量
		}

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		from_stockNo = bcls_rec->Tables[0].Rows[0]["FROM_STOCK_NO"];

		if (from_stockNo.Trim() != "")
		{
			sqlwhere += " AND FROM_STOCK_NO LIKE @from_stockNo ";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TWM0B WHERE 1=1 ";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("from_stockNo", twm0b["FROM_STOCK_NO"].ToString());
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT * FROM TWM0B WHERE 1=1 ";
			break;
		}
		sqlwhere += " ORDER BY FROM_STOCK_NO ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("from_stockNo", "%" + from_stockNo + "%");
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;

			if (fetchRowCount >    (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				Log::Trace("", __FUNCTION__, "超上线，break");
				break;
			}
			if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{
				continue;
			}

			twm0b.Reset();
			cmd_inq.Fetch(twm0b);
			twm0b.MergeTo(bcls_ret->Tables[0], false);
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