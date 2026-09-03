/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   吴新
Version:
Date:     2016-04-13
Description: 查询转库计划
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 查询转库计划
/// <para>
/// 根据传入的库号、材料号，查询转库计划。

/// </summary>
/// <param name="STOCK_NO">库号    </param>
/// <param name="MAT_NO">材料号    </param>
/// <param name="STOCK_NO_CLASS">库号分类	</param>
/// <param name="TRANSFER_PLAN_NO">转库计划号    </param>
/// <returns>转库计划</returns>
===========================================================</remark>*/

#include "stdafx.h" //框架头
//程序头文件
//#include "twm41.h"

// service入口
BM2F_ENTERACE(wm41mg_inq)

int f_wm41mg_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM41 twm41(conn);
	CModel twm41("TWM41");

	/* 业务变量 */
	CString stock_no("");
	CString transfer_plan_no("");
	CString mat_no("");
	CString transfer_status("");
	CString mat_kind("");
	CString mat_line_type = "";
	CString prg_send_time_from("");
	CString prg_send_time_to("");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlorderby = "";
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

		if (bcls_rec->Tables[0].Columns.Contains("STOCK_NO"))
		{
			stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRANSFER_PLAN_NO"))
		{
			transfer_plan_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("TRANSFER_STATUS"))
		{
			transfer_status = bcls_rec->Tables[0].Rows[0]["TRANSFER_STATUS"].ToString().Trim();
		}

		if (bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
		{
			mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		}

		/* ***** 打印输入参数 ***** */
		Log::Debug("", __FUNCTION__, "传入参数STOCK_NO = [{0}]", stock_no);
		Log::Debug("", __FUNCTION__, "传入参数TRANSFER_PLAN_NO = [{0}]", transfer_plan_no);
		Log::Debug("", __FUNCTION__, "传入参数MAT_NO = [{0}]", mat_no);
		Log::Debug("", __FUNCTION__, "传入参数TRANSFER_STATUS = [{0}]", transfer_status);
		Log::Debug("", __FUNCTION__, "传入参数MAT_KIND = [{0}]", mat_kind);
		Log::Debug("", __FUNCTION__, "传入参数PRG_SEND_TIME_FROM = [{0}]", prg_send_time_from);
		Log::Debug("", __FUNCTION__, "传入参数PRG_SEND_TIME_TO = [{0}]", prg_send_time_to);
		Log::Debug("", __FUNCTION__, "传入参数mat_line_type = [{0}]", mat_line_type);


		if (stock_no.Trim() != "")
		{
			sqlwhere += " AND A.STOCK_NO = @stock_no";
		}
		if (transfer_plan_no.Trim() != "")
		{
			sqlwhere += " AND A.TRANSFER_PLAN_NO LIKE @transfer_plan_no";
		}
		if (transfer_status.Trim() == "")
		{
			sqlwhere += " AND A.TRANSFER_STATUS in ('2','8')";
		}
		if (transfer_status.Trim() != "")
		{
			sqlwhere += " AND A.TRANSFER_STATUS = @transfer_status";
		}
		if (mat_kind.Trim() != "")
		{
			sqlwhere += " AND A.MAT_KIND = @mat_kind";
		}
		if (mat_line_type.Trim() != "")
		{
			sqlwhere += " AND A.mat_line_type = @mat_line_type";
		}
		if (prg_send_time_from.Trim() != "")
		{
			sqlwhere += " AND substr(A.PRG_SEND_TIME,1,8) >= @prg_send_time_from ";
		}
		if (prg_send_time_to.Trim() != "")
		{
			sqlwhere += " AND substr(A.PRG_SEND_TIME,1,8) <= @prg_send_time_to ";
		}
		if (mat_no.Trim() != "")
		{
			sqlwhere += " AND B.MAT_NO = @mat_no";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT COUNT(*)"
				" FROM TWM41 A, TWM42 B"
				" WHERE A.TRANSFER_PLAN_NO = B.TRANSFER_PLAN_NO"
				" AND EXISTS(SELECT NULL FROM TWM42 C"
				" WHERE A.TRANSFER_PLAN_NO = C.TRANSFER_PLAN_NO"
				" AND C.AFFIRM_MARK < '4')";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		Log::Debug("", __FUNCTION__, "1111111111111111111111111sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("transfer_plan_no", transfer_plan_no + "%");
		cmd_inq.Parameters.Set("transfer_status", transfer_status);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("mat_kind", mat_kind);
		cmd_inq.Parameters.Set("mat_line_type", mat_line_type);
		cmd_inq.Parameters.Set("prg_send_time_from", prg_send_time_from);
		cmd_inq.Parameters.Set("prg_send_time_to", prg_send_time_to);
		rowCount = cmd_inq.ExecuteScalar();




		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT DISTINCT a.* FROM TWM41 A ,TWM42 B WHERE A.TRANSFER_PLAN_NO = B.TRANSFER_PLAN_NO   ";
			break;
		}
		sqlorderby = " ORDER BY A.PRG_SEND_TIME DESC";


		sqlstr = sqlstr + sqlwhere + sqlorderby;

		Log::Debug("", __FUNCTION__, "222222222222222222222222sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("transfer_plan_no", transfer_plan_no + "%");
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("transfer_status", transfer_status);
		cmd_inq.Parameters.Set("mat_kind", mat_kind);
		cmd_inq.Parameters.Set("mat_line_type", mat_line_type);
		cmd_inq.Parameters.Set("prg_send_time_from", prg_send_time_from);
		cmd_inq.Parameters.Set("prg_send_time_to", prg_send_time_to);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			fetchRowCount++;

			if (fetchRowCount > (pageInfo.RecordFrom + pageInfo.PageSize))
			{
				Log::Trace("", __FUNCTION__, "超上限，break");
				break;
			}
			if (!((fetchRowCount > pageInfo.RecordFrom) && (fetchRowCount <= (pageInfo.RecordFrom + pageInfo.PageSize))))
			{
				continue;
			}

			twm41.Reset();
			cmd_inq.Fetch(twm41);
			twm41.MergeTo(bcls_ret->Tables[0], false);
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

