/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   吴新
Version:
Date:     2016-04-13
Description: 后备录入信息查询
**************************************************/

/*<remark>=========================================================
/// <summary>
/// 出入库履历查询
/// <para>
/// 根据传入的库号、材料号，查询履历

/// </summary>
/// <param name="STOCK_NO">库号    </param>
/// <param name="MAT_NO">材料号    </param>
/// <param name="STOCK_NO_CLASS">库号分类	</param>
/// <param name="TRANSFER_PLAN_NO">转库计划号    </param>
/// <returns>转库计划</returns>
===========================================================</remark>*/

#include "stdafx.h" //框架头
//程序头文件
//#include "twma1.h"

// service入口
BM2F_ENTERACE(wm11b_inq)
int f_wm11b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWMA1 twma1(conn);
	//CModel twma1 = CModel("TMMSM01");

	/* 业务变量 */
	CString stock_no("");
	CString mat_no("");
	CString stock_oper_order("");
	CString order_no = "";
	CString heat_no = "";
	CString pono = "";

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqltable = "";

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

		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();

		/* ***** 打印输入参数 ***** */
		Log::Debug("", __FUNCTION__, "传入参数STOCK_NO			= [{0}]", stock_no);
		Log::Debug("", __FUNCTION__, "传入参数MAT_NO			= [{0}]", mat_no);
		Log::Debug("", __FUNCTION__, "传入参数STOCK_OPER_ORDER	= [{0}]", stock_oper_order);

		sqltable = "(";

#if defined _LINE_SM
		if (sqltable.Trim() != "(")
		{
			sqltable +=
				" UNION ALL";
		}
		sqltable +=
			" SELECT MAT_NO, STOCK_NO, MAT_LINE_TYPE, MAT_KIND, MAT_ACT_THICK, MAT_ACT_WIDTH, MAT_ACT_LEN,"
			" MAT_ACT_WT, SG_SIGN, PONO, HEAT_NO, ST_NO, ORDER_NO, PROD_TIME"
			" FROM TMMSM01";
#endif
#if defined _LINE_HR
		if (sqltable.Trim() != "(")
		{
			sqltable +=
				" UNION ALL";
		}
		sqltable +=
			" SELECT MAT_NO, STOCK_NO, MAT_LINE_TYPE, MAT_KIND, MAT_ACT_THICK, MAT_ACT_WIDTH, MAT_ACT_LEN,"
			" MAT_ACT_WT, SG_SIGN, PONO, HEAT_NO, ST_NO, ORDER_NO, PROD_TIME"
			" FROM TMMHR01";
#endif
#if defined _LINE_CR
		if (sqltable.Trim() != "(")
		{
			sqltable +=
				" UNION ALL";
		}
		sqltable +=
			" SELECT MAT_NO, STOCK_NO, MAT_LINE_TYPE, MAT_KIND, MAT_ACT_THICK, MAT_ACT_WIDTH, MAT_ACT_LEN,"
			" MAT_ACT_WT, SG_SIGN, PONO, HEAT_NO, ST_NO, ORDER_NO, PROD_TIME"
			" FROM TMMCR01";
#endif
#if defined _LINE_HP
		if (sqltable.Trim() != "(")
		{
			sqltable +=
				" UNION ALL";
		}
		sqltable +=
			" SELECT MAT_NO, STOCK_NO, MAT_LINE_TYPE, MAT_KIND, MAT_ACT_THICK, MAT_ACT_WIDTH, MAT_ACT_LEN,"
			" MAT_ACT_WT, SG_SIGN, PONO, HEAT_NO, ST_NO, ORDER_NO, PROD_TIME"
			" FROM TMMHP01";
#endif
#if defined _LINE_BW
		if (sqltable.Trim() != "(")
		{
			sqltable +=
				" UNION ALL";
		}
		sqltable +=
			" SELECT MAT_NO, STOCK_NO, MAT_LINE_TYPE, MAT_KIND, MAT_ACT_THICK, MAT_ACT_WIDTH, MAT_ACT_LEN,"
			" MAT_ACT_WT, SG_SIGN, PONO, HEAT_NO, ST_NO, ORDER_NO, PROD_TIME"
			" FROM TMMBW01";
#endif
#if defined _LINE_SF
		if (sqltable.Trim() != "(")
		{
			sqltable +=
				" UNION ALL";
		}
		sqltable +=
			" SELECT MAT_NO, STOCK_NO, MAT_LINE_TYPE, MAT_KIND, MAT_ACT_THICK, MAT_ACT_WIDTH, MAT_ACT_LEN,"
			" MAT_ACT_WT, SG_SIGN, PONO, HEAT_NO, ST_NO, ORDER_NO, PROD_TIME"
			" FROM TMMSF01";
#endif

		sqltable += ")";


		if (stock_no.Trim() != "")
		{
			sqlwhere += " AND STOCK_NO = @stock_no";
		}
		if (mat_no.Trim() != "")
		{
			sqlwhere += " AND MAT_NO IN (" + mat_no + ")";
		}
		if (stock_oper_order.Trim() != "")
		{
			sqlwhere += " AND STOCK_OPER_ORDER = @stock_oper_order";
		}
		if (order_no.Trim() != "")
		{
			sqlwhere += " AND ORDER_NO = @order_no";
		}
		if (heat_no.Trim() != "")
		{
			sqlwhere += " AND HEAT_NO = @heat_no";
		}
		if (pono.Trim() != "")
		{
			sqlwhere += " AND PONO = @pono";
		}

		//#if defined _LINE_SM
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT COUNT(1) FROM " + sqltable + " T1"
				" WHERE NOT EXISTS (SELECT NULL FROM TWMA0 T2"
				" WHERE T1.MAT_NO = T2.MAT_NO"
				" AND T2.STOCK_OPER_ORDER LIKE '1%')"
				" AND NOT EXISTS (SELECT NULL FROM TWMA2 T2"
				" WHERE T1.MAT_NO = T2.MAT_NO)";
			break;
		}

		sqlstr = sqlstr + sqlwhere;
		Log::Debug("", __FUNCTION__, "1111111111111111111111111sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("stock_oper_order", stock_oper_order);
		cmd_inq.Parameters.Set("order_no", order_no);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.Parameters.Set("pono", pono);
		rowCount = cmd_inq.ExecuteScalar();

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT * FROM " + sqltable + "T1"
				" WHERE NOT EXISTS (SELECT NULL FROM TWMA0 T2"
				" WHERE T1.MAT_NO = T2.MAT_NO"
				" AND T2.STOCK_OPER_ORDER LIKE '1%')"
				" AND NOT EXISTS (SELECT NULL FROM TWMA2 T2"
				" WHERE T1.MAT_NO = T2.MAT_NO)";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		sqlstr += " ORDER BY MAT_NO";
		Log::Debug("", __FUNCTION__, "222222222222222222222222sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("mat_no", mat_no);
		cmd_inq.Parameters.Set("stock_oper_order", stock_oper_order);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);

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

