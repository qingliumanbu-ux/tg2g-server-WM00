/*<remark>=========================================================
/// <summary>
///  库区号查询
/// <para>查询库区号、库区描述。
/// </para>
/// <para>数据库表：(库号定义表)</para>
/// </summary>
/// <param name=""> </param>
/// <returns>返回参数：库区号、库区描述</returns>
===========================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"

//函数申明
int f_epes_get_auth_other(const char *iuser, int irestype, EIClass *bcls_ret, CDbConnection * conn);

// Service 入口
BM2F_ENTERACE(wm00_stockNo)
int f_wm00_stockNo(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	/* 业务变量 */
	CString s_userid("");
	CString v_mat_line_type = "";
	CString v_mat_kind = "";
	CString v_stock_type_code = "";
	CString v_factory_div = "";
	int auto_stock_flag = 0;//是否库区授权  1--是  0--否

	/* 数据库SQL操作字符串 */
	CString sqlstr(" ");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		Log::Trace("","","222222");
		// 获取前台传入参数
		s_userid = s.userid;
		//获取库区授权
		CString stock_no_auth = "' '";
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

		if (bcls_rec->Tables[0].Columns.Contains("MAT_LINE_TYPE"))
		{
			v_mat_line_type = bcls_rec->Tables[0].Rows[0]["MAT_LINE_TYPE"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_KIND"))
		{
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("STOCK_TYPE_CODE"))
		{
			v_stock_type_code = bcls_rec->Tables[0].Rows[0]["STOCK_TYPE_CODE"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		}
		
		//bcls_ret->Tables[0].set_TableName("WM_DYN_SQL");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_DESC");

		/* ***** 打印输入参数 ***** */
		Log::Debug("", __FUNCTION__, "传入参数 USERID = [{0}]", s_userid);
		Log::Debug("", __FUNCTION__, "传入参数 v_mat_line_type = [{0}]", v_mat_line_type);
		Log::Debug("", __FUNCTION__, "传入参数 v_mat_kind = [{0}]", v_mat_kind);
		Log::Debug("", __FUNCTION__, "传入参数 v_stock_type_code = [{0}]", v_stock_type_code);
		Log::Debug("", __FUNCTION__, "传入参数 v_factory_div = [{0}]", v_factory_div);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:			// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:			// MS SQL Server数据库
		case DB_KIND_ORACLE:		// Oracle 数据库
			sqlstr =
				" SELECT ' ' AS STOCK_NO, ' ' AS STOCK_DESC FROM SYSIBM.SYSDUMMY1 UNION ALL" //  oracle DUAL
				" SELECT T2.STOCK_NO, T2.STOCK_DESC"
				" FROM TWM01 T2";
			;

			break;
		default:					// 所有数据库适用，通用SQL语句
			sqlstr =
				" SELECT ' ' AS STOCK_NO, ' ' AS STOCK_DESC FROM SYSIBM.SYSDUMMY1 UNION ALL"
				" SELECT T2.STOCK_NO, T2.STOCK_DESC"
				" FROM TWM01 T2";
			
			;

			break;
		}
		if (auto_stock_flag == 1)
		{
			//sqlstr +=
			//	" WHERE STOCK_NO IN (" + stock_no_auth + ")";
		}
		else
		{
			sqlstr +=
				" WHERE 1=1 ";
		}

		if (v_mat_line_type.Trim() != "")
		{
			sqlstr += " AND T2.MAT_LINE_TYPE = @mat_line_type";
		}

		if (v_mat_kind.Trim() != "")
		{
			sqlstr += " AND T2.MAT_KIND = @mat_kind";
		}

		if (v_factory_div.Trim() != "")
		{
			sqlstr += " AND T2.FACTORY_DIV = @factory_div";
		}
		
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("userid", s_userid);
		cmd_inq.Parameters.Set("mat_line_type", v_mat_line_type);
		cmd_inq.Parameters.Set("mat_kind", v_mat_kind); 
		cmd_inq.Parameters.Set("factory_div", v_factory_div);
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);  //0,-1：非翻页查询
		Log::Trace("", "", "222222={0}", bcls_ret->Tables[0].Rows.get_Count());
		cmd_inq.Close();
	}
	catch (CDbException& ex)					//捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)	//捕获应用错误
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

	return(doFlag);
}
