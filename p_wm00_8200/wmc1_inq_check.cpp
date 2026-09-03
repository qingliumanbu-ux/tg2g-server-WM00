/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-06-30
Description:	请盘库核对结果信息查询
**************************************************/

//框架头文件
#include "stdafx.h"
////程序用头文件
//#include "twmb3.h"
//#include "twmb1.h"
//#include "twma2.h"

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//函数申明

/*<remark>=========================================================
///<summary>
///请盘库核对
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWMB3 盘库结果差异表
///<returns>请盘库核对</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmc1_inq_check);

int f_wmc1_inq_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int recordFrom = 0;	//起始页
	int pageSize = 0;	//每页记录数
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 业务变量 */
	CString stock_no = "";
	CString mat_no = "";
	CString result_code = "";

	/* 实体类定义 */
	//CTWMB3 twmb3(conn);
	//CTWMB1 twmb1(conn);
	//CTWMA2 twma2(conn);
	CModel twmb3 = CModel("TWMB3");
	CModel twmb1 = CModel("TWMB1");
	CModel twma2 = CModel("TWMA2");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";
	CString sqlquery = "";
	CString sqlcount = "";
	CDecimal rowCount = 0;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//系统的分页类信息。
	CPageInfo pageInfo;

	try
	{

		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);

		twmb1.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//有实物无信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT COUNT(1) FROM   TWMB1 b"
				" WHERE  b.STOCK_NO = @stock_no "
				" AND   b.MAT_NO NOT IN (SELECT a.MAT_NO FROM TWMA2 a WHERE a.STOCK_NO = @stock_no) ";
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twmb1["STOCK_NO"].ToString());
		int mmm = cmd_inq.ExecuteReader();

		Log::Trace("", "", "有实物无信息:cmd_inq.ExecuteReader() ={0}", mmm);

		while (cmd_inq.Read())
		{
			twmb3.Reset();
			cmd_inq.Fetch(twmb1);
			twmb3["REC_CREATOR"] = s.userid;
			twmb3["REC_CREATE_TIME"] = datetime;
			twmb3["REC_REVISOR"] = " ";
			twmb3["REC_REVISE_TIME"] = " ";
			twmb3["MAT_NO"] = twmb1["MAT_NO"];
			if (twmb3.Query("MAT_NO"))
			{
				Log::Trace("", "", "有实物无信息update");

				twmb3["STOCK_NO"] = twmb1["STOCK_NO"];
				twmb3["STOCK_PLACE_NO"] = twmb1["STOCK_PLACE_NO"];
				twmb3["LAYERNO"] = twmb1["LAYERNO"];
				twmb3["MAT_ACT_WT"] = twmb1["MAT_ACT_WT"];
				twmb3["MAT_NUM"] = twmb1["MAT_NUM"];
				twmb3.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,STOCK_NO, STOCK_PLACE_NO, LAYERNO, MAT_ACT_WT, MAT_NUM", "MAT_NO,RESULT_CODE");
				twmb3.MergeTo(bcls_ret->Tables[0], false);
			}
			else
			{
				Log::Trace("", "", "有实物无信息insert");
				twmb3["RESULT_CODE"] = "1";
				twmb3["STOCK_NO"] = twmb1["STOCK_NO"];
				twmb3["STOCK_PLACE_NO"] = twmb1["STOCK_PLACE_NO"];
				twmb3["LAYERNO"] = twmb1["LAYERNO"];
				twmb3["MAT_ACT_WT"] = twmb1["MAT_ACT_WT"];
				twmb3["MAT_NUM"] = twmb1["MAT_NUM"];
				twmb3.Insert();
				twmb3.MergeTo(bcls_ret->Tables[0], false);
			}

		}
		cmd_inq.Close();

		//有信息无实物
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT * FROM  TWMA2 a WHERE  a.STOCK_NO = @stock_no"
				" AND a.MAT_NO NOT IN (SELECT b.MAT_NO FROM TWMB1 b WHERE  b.STOCK_NO = @stock_no) ";
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());
		mmm = cmd_inq.ExecuteReader();

		Log::Trace("", "", "有信息无实物:cmd_inq.ExecuteReader() ={0}", mmm);

		while (cmd_inq.Read())
		{

			twmb3.Reset();
			cmd_inq.Fetch(twma2);
			twmb3["REC_CREATOR"] = s.userid;
			twmb3["REC_CREATE_TIME"] = datetime;
			twmb3["REC_REVISOR"] = " ";
			twmb3["REC_REVISE_TIME"] = " ";
			twmb3["MAT_NO"] = twma2["MAT_NO"];
			if (twmb3.Query("MAT_NO"))
			{
				Log::Trace("", "", "有信息无实物update");
				twmb3["STOCK_NO"] = twma2["STOCK_NO"];
				twmb3["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twmb3["LAYERNO"] = twma2["LAYERNO"];
				twmb3["MAT_NUM"] = twma2["MAT_NUM"];
				twmb3.Update("REC_CREATOR, REC_CREATE_TIME, REC_REVISOR, REC_REVISE_TIME, STOCK_NO, STOCK_PLACE_NO, LAYERNO, MAT_NUM", "MAT_NO,RESULT_CODE");
				twmb3.MergeTo(bcls_ret->Tables[0], false);
			}
			else
			{
				Log::Trace("", "", "有信息无实物insert");
				twmb3["RESULT_CODE"] = "2";
				twmb3["STOCK_NO"] = twma2["STOCK_NO"];
				twmb3["STOCK_PLACE_NO"] = twma2["STOCK_PLACE_NO"];
				twmb3["LAYERNO"] = twma2["LAYERNO"];
				twmb3["MAT_NUM"] = twma2["MAT_NUM"];
				twmb3.Insert();
				twmb3.MergeTo(bcls_ret->Tables[0], false);
			}

		}
		cmd_inq.Close();

		//实物与信息不符
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = " SELECT *  FROM   TWMA2 a ,TWMB1 b WHERE  a.STOCK_NO = @stock_no "
				" AND a.STOCK_NO=b.STOCK_NO"
				" AND a.MAT_NO=b.MAT_NO"
				" AND (a.STOCK_PLACE_NO != b.STOCK_PLACE_NO OR a.LAYERNO != b.LAYERNO OR a.MAT_NUM != b.MAT_NUM)";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_no", twma2["STOCK_NO"].ToString());
		mmm = cmd_inq.ExecuteReader();

		Log::Trace("", "", "实物与信息不符:cmd_inq.ExecuteReader() ={0}", mmm);

		while (cmd_inq.Read())
		{
			//获取信息库存
			twma2["MAT_NO"] = twmb1["MAT_NO"];
			twma2.Query();
			

			if (twmb1["STOCK_PLACE_NO"].ToString().Trim() != twma2["STOCK_PLACE_NO"].ToString().Trim() ||
				twmb1["LAYERNO"].ToDecimal() != twma2["LAYERNO"].ToDecimal())
			{

				twmb3.Reset();
				cmd_inq.Fetch(twmb1);
				twmb3["REC_CREATOR"] = s.userid;
				twmb3["REC_CREATE_TIME"] = datetime;
				twmb3["MAT_NO"] = twmb1["MAT_NO"];
				if (twmb3.Query("MAT_NO"))
				{
					Log::Trace("", "", "实物与信息不符update");;
					twmb3["STOCK_NO"] = twmb1["STOCK_NO"];
					twmb3["STOCK_PLACE_NO"] = twmb1["STOCK_PLACE_NO"];
					twmb3["LAYERNO"] = twmb1["LAYERNO"];
					twmb3["MAT_ACT_WT"] = twmb1["MAT_ACT_WT"];
					twmb3["MAT_NUM"] = twmb1["MAT_NUM"];
					twmb3.Update("REC_CREATOR, REC_CREATE_TIME, REC_REVISOR, REC_REVISE_TIME, STOCK_NO, STOCK_PLACE_NO, LAYERNO, MAT_NUM", "MAT_NO,RESULT_CODE");
					twmb3.MergeTo(bcls_ret->Tables[0], false);
				}
				else
				{
					Log::Trace("", "", "实物与信息不符insert");
					twmb3["RESULT_CODE"] = "3";
					twmb3["STOCK_NO"] = twmb1["STOCK_NO"];
					twmb3["STOCK_PLACE_NO"] = twmb1["STOCK_PLACE_NO"];
					twmb3["LAYERNO"] = twmb1["LAYERNO"];
					twmb3["MAT_ACT_WT"] = twmb1["MAT_ACT_WT"];
					twmb3["MAT_NUM"] = twmb1["MAT_NUM"];
					twmb3.Insert();
					twmb3.MergeTo(bcls_ret->Tables[0], false);
				}

			}
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