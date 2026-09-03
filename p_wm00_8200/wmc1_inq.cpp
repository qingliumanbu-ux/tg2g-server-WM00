/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-06-30
Description:	请盘库实物库存信息查询
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmc1_inq);

int f_wmc1_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;

	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel twmb1 = CModel("TWMB1");

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

		twmb1.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"].ToString().Trim();

		Log::Trace("", "", "传入后台库区号={0}", stock_no);
	  
		if (stock_no.Trim() != "")
		{
			sqlwhere = sqlwhere+ " AND STOCK_NO = '" + stock_no + "'";
		}
		sqlstr = "SELECT STOCK_NO,MAT_NO,STOCK_PLACE_NO,LAYERNO,MAT_ACT_WT,MAT_NUM FROM TWMB1 WHERE 1=1 ";
	
		sqlwhere += " ORDER BY STOCK_NO ";
		sqlstr = sqlstr + sqlwhere;		
		Db::QueryTable(sqlstr, bcls_ret->Tables[0]);

		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = bcls_ret->Tables[0].Rows.get_Count();
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