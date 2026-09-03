/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	仓库逻辑库区定义信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm02.h"

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

/*<remark>=========================================================
///<summary>
///库区定义信息查询
///<para>
///2.排序方式：LOGIC_STOCK_NO
///</para>
///<para>数据库表：TWM02 库区定义表；
///<returns>返回符合查询条件的逻辑库区信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm02_inq);

int f_wm02_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM02 twm02(conn);
	CModel twm02("TWM02");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString s_userid("");

	CString sqlstr_count;
	CString sqlstr_temp;

	/* 业务变量 */
	CString logic_stock_no = "";
	//CString	datetime("");
	CString	cs_ladle_no("");
	CString	cs_sm_unit_no("");
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		////获取库区授权
		//CString stock_no_auth = "' '";
		//EIClass *bcls_auth = new EIClass;
		//if (f_epes_get_auth_other(s.userid, 5, bcls_auth, conn) != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		//for (int fetchRowCount = 0; fetchRowCount < bcls_auth->Tables[0].Rows.get_Count(); fetchRowCount++)
		//{
		//	stock_no_auth += ", '" + bcls_auth->Tables[0].Rows[fetchRowCount]["name"].ToString() + "' ";
		//}
		//delete bcls_auth;
		//// 获取前台传入参数
		//s_userid = s.userid;

		////分页信息
		//CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		//table.Columns.Add(DT_DECIMAL, "recordsum");

		////2)获取分页信息
		//if (bcls_rec->Tables.Contains("PageInfo"))
		//{
		//	pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		//}
		//else
		//{
		//	pageInfo.RecordFrom = 0;
		//	pageInfo.PageSize = -1;  //每页记录数量
		//}

		//Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);


		//twm02.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//if (twm02["LOGIC_STOCK_NO"].ToString().Trim() != "")
		//{
		//	sqlwhere += " AND LOGIC_STOCK_NO = @logic_stock_no ";
		//}

		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//	sqlstr = "SELECT COUNT(1) FROM TWM02 WHERE 1=1 ";
		//	break;
		//}
		////sqlwhere += " AND STOCK_NO IN"
		////	"( select STOCK_NO"
		////	" from   twm0a "
		////	" where   groupid in ( "
		////	" select groupid from TESGROUPMEMBER "
		////	"  where memberid in ( "
		////	" select id from tesuserinfo where ename = @userid "
		////	" ) "
		////	"  ) "
		////	"  ) ";
		//sqlwhere +=
		//	" AND STOCK_NO IN (" + stock_no_auth + ")";
		//sqlstr = sqlstr + sqlwhere;
		//cmd_inq.Parameters.Set("userid", s_userid);
		//Log::Trace("", __FUNCTION__, "userid = [{0}]", s_userid);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("logic_stock_no", twm02["LOGIC_STOCK_NO"].ToString());
		//rowCount = cmd_inq.ExecuteScalar();

		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//	sqlstr = "SELECT * FROM TWM02 WHERE 1=1 ";
		//	break;
		//}
		//sqlwhere += " ORDER BY LOGIC_STOCK_NO ";
		//sqlstr = sqlstr + sqlwhere;
		//cmd_inq.Parameters.Set("userid", s_userid);
		//Log::Trace("", __FUNCTION__, "userid = [{0}]", s_userid);
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("logic_stock_no", twm02["LOGIC_STOCK_NO"].ToString());
		//cmd_inq.ExecuteReader();
		//while (cmd_inq.Read())
		//{
		//	fetchRowCount++;
		//	Log::Trace("", __FUNCTION__, "55");
		//	if (fetchRowCount >    (pageInfo.RecordFrom + pageInfo.PageSize))
		//	{
		//		Log::Trace("", __FUNCTION__, "超上线，break");
		//		break;
		//	}
		//	if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
		//	{
		//		continue;
		//	}

		//	twm02.Reset();
		//	cmd_inq.Fetch(twm02);
		//	twm02.MergeTo(bcls_ret->Tables[0], false);
		//}
		//cmd_inq.Close();

		////返回记录总数
		//CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		//row1["recordsum"] = rowCount.ToInt32();






		///////SG
		//datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//分页信息
		//CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");//
		//table.Columns.Add(DT_DECIMAL, "recordsum");
		logic_stock_no = bcls_rec->Tables[0].Rows[0]["LOGIC_STOCK_NO"].ToString().Trim();
		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库

			sqlstr_count = "SELECT COUNT(1) FROM TWM02 WHERE 1=1 ";

			sqlstr = "SELECT * FROM TWM02 WHERE 1=1 ";

			if (logic_stock_no.Trim() != "")
			{
				sqlstr_temp += " AND LOGIC_STOCK_NO LIKE @logic_stock_no ||'%' ";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY LOGIC_STOCK_NO ASC";
			sqlstr = sqlstr + sqlstr_temp;

			break;
		}


		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (logic_stock_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("logic_stock_no", logic_stock_no);
		}
		cmd_inq.SetCommandText(sqlstr_count);

		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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