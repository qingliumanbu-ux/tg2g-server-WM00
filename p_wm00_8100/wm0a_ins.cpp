/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	仓库授权用户（按群组）信息新增
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm0a.h"


//函数申明

/*<remark>=========================================================
///<summary>
///仓库授权用户（按群组）信息新增
///<para>
///2.排序方式：GROUPID
///</para>
///<para>数据库表：TWM0A 仓库授权用户（按群组）信息表；
///<returns>新增传入的仓库库区对应群组信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm0a_ins);

int f_wm0a_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM0A twm0a(conn);
	CModel twm0a = CModel("TWM0A");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm0a.Reset();
			twm0a.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm0a["GROUPID"].ToDecimal() == 0)
			{
				sprintf(s.msg, "群组号不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm0a["STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM0A WHERE GROUPID =@groupid "
					" AND STOCK_NO = @stock_no ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("groupid", twm0a["GROUPID"].ToString());
			cmd_inq.Parameters.Set("stock_no", twm0a["STOCK_NO"].ToString());
			Count = cmd_inq.ExecuteScalar();
			Log::Trace("", __FUNCTION__, "Count[{0}]", Count);

			if (Count > 0)
			{
				sprintf(s.msg, "该逻辑库区[%s]对应的该库位已存在，无需新增。", (const char*)twm0a["STOCK_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm0a["REC_CREATE_TIME"] = datetime;
			twm0a["REC_CREATOR"] = s.userid;
			twm0a.Insert();
		}
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
