/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	行车属性配置信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm06.h"


//函数申明

/*<remark>=========================================================
///<summary>
///库区定义信息查询
///<para>
///2.排序方式：CRANE_NO
///</para>
///<para>数据库表：TWM06 行车属性配置信息表；
///<returns>返回符合查询条件的行车属性配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm06_inq);

int f_wm06_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06("TWM06");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlstr_count;
	CString sqlstr_temp;

		/* 业务变量 */
	CString	datetime("");	
	CString	cs_ladle_no("");	
	CString	cs_sm_unit_no("");	
	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    

	/* 全局变量 */
	CString crane_no = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//分页信息
		//CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");//
		//table.Columns.Add(DT_DECIMAL, "recordsum");
		crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"].ToString().Trim();	//炼钢单元号
		record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
		current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];

		Log::Trace("", __FUNCTION__, "record_count_per_page	= [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "current_page_no			= [{0}]", current_page_no);

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


	/*	crane_no = bcls_rec->Tables[0].Rows[0]["CRANE_NO"];

		if (crane_no.Trim() != "")
		{
			sqlwhere += " AND CRANE_NO LIKE @crane_no ";
		}*/

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库

			sqlstr_count = "SELECT COUNT(1) FROM TWM06 WHERE 1=1 ";
			
			sqlstr = "SELECT * FROM TWM06 WHERE 1=1 ";

			if (crane_no.Trim() != "")
			{
				sqlstr_temp += " AND CRANE_NO LIKE @crane_no ||'%' ";
			}

			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY CRANE_NO ASC";
			sqlstr = sqlstr + sqlstr_temp;

			break;
		}


		Log::Trace("", __FUNCTION__, "sqlstr_temp			= [{0}]", (const char*)sqlstr_temp);
		Log::Trace("", __FUNCTION__, "sqlstr_count		= [{0}]", (const char*)sqlstr_count);
		Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if (crane_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("crane_no", crane_no);
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