/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	入库管理页面库位信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "smhs.h"

//函数申明

/*<remark>=========================================================
///<summary>
///入库管理页面库位信息查询
///<para>
///2.排序方式：GROUPID
///</para>
///<para>数据库表：TWMA1 物料主档表、TWM04仓库库位定义表；
///<returns>返回符合查询条件的仓库库位信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm_stockplace_search);

int f_wm_stockplace_search(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "", sql_order_by = " order by a.STOCK_PLACE_NO ";//a.column_no,a.layerno,a.pile_time_lately desc";
	CString stock_no = "";
	CString stock_place_no = "";
	CString mat_line_type = "";
	CString mat_kind = ""; 
	CString factory_div = "";
	CString same_custom = "";
	CString same_order = "";
	CString same_spec = "";
	CString empty_stack = "";
	CString hall_no = "";
	CString rowno = "";
	CString column_no = "";
	CString stock_place_no1 = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
		stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
		{
			mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))
		{
			mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SAME_CUSTOM"))
		{
			same_custom = bcls_rec->Tables[0].Rows[0]["SAME_CUSTOM"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SAME_ORDER"))
		{
			same_order = bcls_rec->Tables[0].Rows[0]["SAME_ORDER"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SAME_SPEC"))
		{
			same_spec = bcls_rec->Tables[0].Rows[0]["SAME_SPEC"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("EMPTY_STACK"))
		{
			empty_stack = bcls_rec->Tables[0].Rows[0]["EMPTY_STACK"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("HALL_NO"))
		{
			hall_no = bcls_rec->Tables[0].Rows[0]["HALL_NO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ROWNO"))
		{
			rowno = bcls_rec->Tables[0].Rows[0]["ROWNO"].ToString().Trim();
		}
		if (bcls_rec->Tables[0].Columns.Contains("COLUMN_NO"))
		{
			column_no = bcls_rec->Tables[0].Rows[0]["COLUMN_NO"].ToString().Trim();
		}

		if (stock_no.Trim() != "")
		{
			//			sqlwhere += " AND a.STOCK_NO like @stock_no||'%' ";
			sqlwhere += " AND a.STOCK_NO = @stock_no ";
		}
		if (stock_place_no.Trim() != "")
		{
			//	sqlwhere += " AND a.STOCK_PLACE_NO like a.STOCK_NO||@stock_place_no||'%' ";
			sqlwhere += " AND a.STOCK_PLACE_NO like @stock_place_no||'%' ";
		}
		if (mat_line_type.Trim() != "")
		{
			sqlwhere += " AND b.MAT_LINE_TYPE LIKE @mat_line_type ";
		}
		if (mat_kind.Trim() != "")
		{
			sqlwhere += " AND b.MAT_KIND LIKE @mat_kind ";
		}
		if ((mat_line_type.Trim() == "HP") && (mat_kind.Trim() == "HP") && (stock_no.Trim() == ""))
		{
			sqlstr += " AND A.STOCK_NO != 'J12'";
		}
		/*if (factory_div.Trim() != "")
		{
			sqlwhere += " AND b.FACTORY_DIV LIKE @factory_div";
		}*/
		if (same_custom.Trim() == "1")    //同客户
		{
			sqlwhere += "  ";
		}
		if (same_order.Trim() == "1")    //同订单
		{
			sqlwhere += "  ";
		}
		if (same_spec.Trim() == "1")    //同规格
		{
			sqlwhere += "  ";
		}
		if (empty_stack.Trim() == "1")    //空垛位
		{
			sqlwhere += " AND A.STOCK_STATUS = '0' ";
		}

		stock_place_no1 = stock_no;
		if (hall_no != "")
		{
			stock_place_no1 += hall_no;

			if (rowno != "")
			{
				stock_place_no1 += rowno;
				if (column_no != "")
				{
					stock_place_no1 += column_no;
				}
				else
				{
					stock_place_no1 += "%";
				}

			}
			else
			{
				stock_place_no1 += "%";
				if (column_no != "")
				{
					stock_place_no1 += column_no;
				}
				else
				{
					stock_place_no1 += "%";
				}
			}
		}
		else
		{
			stock_place_no1 += "%";
			if (rowno != "")
			{
				stock_place_no1 += rowno;
				if (column_no != "")
				{
					stock_place_no1 += column_no;
				}
				else
				{
					stock_place_no1 += "%";
				}
			}
			else
			{
				stock_place_no1 += "%";
				if (column_no != "")
				{
					stock_place_no1 += column_no;
				}
				else
				{
					stock_place_no1 += "%";
				}
			}
		}
		if (((hall_no != "") || (rowno != "") || (column_no != "")) && (stock_place_no.Trim() == ""))
			sqlwhere += " AND A.STOCK_PLACE_NO LIKE @stock_place_no1 ";
		
		Log::Debug("", __FUNCTION__, "传入参数stock_no			= [{0}]", stock_no);
		Log::Debug("", __FUNCTION__, "传入参数stock_place_no			= [{0}]", stock_place_no);
		Log::Debug("", __FUNCTION__, "传入参数mat_line_type			= [{0}]", mat_line_type);
		Log::Debug("", __FUNCTION__, "传入参数mat_kind			= [{0}]", mat_kind);
		Log::Debug("", __FUNCTION__, "传入参数factory_div			= [{0}]", factory_div);
		Log::Debug("", __FUNCTION__, "传入参数same_custom			= [{0}]", same_custom);
		Log::Debug("", __FUNCTION__, "传入参数same_order			= [{0}]", same_order);
		Log::Debug("", __FUNCTION__, "传入参数same_spec			= [{0}]", same_spec);
		Log::Debug("", __FUNCTION__, "传入参数empty_stack			= [{0}]", empty_stack);
		Log::Debug("", __FUNCTION__, "传入参数stock_place_no1			= [{0}]", stock_place_no1);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
			sqlstr = "  SELECT a.rowno rownum,a.stock_no,                    "
				"  a.STOCK_PLACE_NO,  b.STOCK_DESC, a.MAX_LAYER_COUNT,a.MAX_ROW_COUNT,a.MAX_POSITION_COUNT,A.STOCK_STATUS,     "
				"  a.PILE_MAT_NUM_ACT MAT_NUM,           "
				"  a.PILE_MAT_WT_ACT MAT_ACT_WT,          "
				"  a.HALL_NO,a.rowno,a.column_no,a.LAYERNO              "
				"  ,a.CURRENT_LAYERNO,a.CURRENT_LAYER_NUM  "
				"  FROM TWM04   a, TWM01 b                       "
				" WHERE 1 = 1                          "
				"  AND a.STOCK_PLACE_TYPE != 'D' "//STOCK_PLACE_TYPE库位类型是0
				"  AND a.STOCK_NO = b.STOCK_NO "
				//"  AND ((a.MANAGE_ACCU != '5' and a.STOCK_STATUS in ('0', '3', '5') ) "
				//"		OR (a.MANAGE_ACCU = '5' and a.STOCK_STATUS = '0' ))"
				"  AND a.STOCK_STATUS not in ( '3', '4','9') "
				;
			break;
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "  SELECT a.rowno ,a.stock_no,        "
				"  a.STOCK_PLACE_NO, b.STOCK_DESC, a.MAX_LAYER_COUNT,a.MAX_ROW_COUNT,a.MAX_POSITION_COUNT,A.STOCK_STATUS,      "
				"  a.PILE_MAT_NUM_ACT MAT_NUM,           "
				"  a.PILE_MAT_WT_ACT MAT_ACT_WT,          "
				"  a.HALL_NO,a.column_no,a.LAYERNO              "
				"  ,a.CURRENT_LAYERNO,a.CURRENT_LAYER_NUM  "
				"  FROM TWM04   a, TWM01 b                       "
				" WHERE 1 = 1                          "
				"  AND a.STOCK_PLACE_TYPE != 'D' "//STOCK_PLACE_TYPE库位类型是0
				"  AND a.STOCK_NO = b.STOCK_NO "
				//"  AND ((a.MANAGE_ACCU != '5' and a.STOCK_STATUS in ('0', '3', '5') ) "
				//"		OR (a.MANAGE_ACCU = '5' and a.STOCK_STATUS = '0' ))"
				"  AND a.STOCK_STATUS not in ( '3', '4','9') "
				;
			break;
		}

		cmd_inq.SetCommandText(sqlstr + sqlwhere + sql_order_by);

		Log::Trace("", __FUNCTION__, "传入参数sqlstr			= [{0}]", sqlstr + sqlwhere + sql_order_by);
		
		cmd_inq.Parameters.Set("stock_no", stock_no);
		cmd_inq.Parameters.Set("stock_place_no", stock_place_no);
		cmd_inq.Parameters.Set("mat_line_type", mat_line_type + "%");
		cmd_inq.Parameters.Set("mat_kind", mat_kind + "%");
		cmd_inq.Parameters.Set("factory_div", factory_div + "%");
		cmd_inq.Parameters.Set("stock_place_no1", stock_place_no1);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

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
