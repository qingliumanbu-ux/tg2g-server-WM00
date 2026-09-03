/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-06-30
Description:	请盘库信息库存信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twma2.h"

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//函数申明

/*<remark>=========================================================
///<summary>
///仓库跨号定义信息查询
///<para>
///2.排序方式：STOCK_NO,MAT_NO
///</para>
///<para>数据库表：TWMB1，TWMA2 信息查询；
///<returns>返回符合查询条件的信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmc1_inq_sys);

int f_wmc1_inq_sys(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA2 twma2(conn);
	CModel twma2("TWMA2");

	/* 业务变量 */
	CString stock_no("");
	CString mat_line_type("");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString s_userid("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		// 获取前台传入参数
		s_userid = s.userid;

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

		twma2.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();

		Log::Trace("", "", "传入后台参数为= {0}", stock_no);

		if (stock_no.Trim() != "")
		{
			Log::Trace("", "", "111");
			sqlwhere += " AND STOCK_NO = @stock_no";
		}
		sqlwhere += " AND STOCK_NO IN"
			"( select STOCK_NO"
			" from   twm0a "
			" where   groupid in ( "
			" select groupid from TESGROUPMEMBER "
			"  where memberid in ( "
			" select id from tesuserinfo where ename = @userid "
			" ) "
			"  ) "
			"  ) ";
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TWMA2 WHERE 1=1 ";
			break;
		}
		sqlstr = sqlstr + sqlwhere;		
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("userid", s_userid);			
		cmd_inq.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());	
		rowCount = cmd_inq.ExecuteScalar();			

		Log::Trace("", "", "rowCount = {0}", rowCount);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT * FROM TWMA2 WHERE 1=1 ";
			break;
		}
		sqlwhere += " ORDER BY STOCK_NO ";
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("userid", s_userid);
		cmd_inq.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());
		int mmm=cmd_inq.ExecuteReader();

		Log::Trace("", "", "ExecuteReader() ={0}", mmm);

		while (cmd_inq.Read())
		{
			fetchRowCount++;

			if (fetchRowCount >    (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				Log::Trace("", __FUNCTION__, "超上线，break");
				break;
			}
			Log::Trace("", "", "fetchRowCount ={0}", fetchRowCount);

			if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{
				continue;
			}

			twma2.Reset();
			cmd_inq.Fetch(twma2);
			twma2.MergeTo(bcls_ret->Tables[0], false);
		}
		cmd_inq.Close();


		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = rowCount.ToInt32();
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