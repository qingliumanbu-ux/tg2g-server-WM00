/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	台车属性配置信息新增
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm09.h"


//函数申明

/*<remark>=========================================================
///<summary>
///台车属性配置信息新增
///<para>
///2.排序方式：TR_NO
///</para>
///<para>数据库表：TWM09 台车属性配置信息表；
///<returns>新增传入的行车属性配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm09_ins);

int f_wm09_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM09 twm09(conn);
	CModel twm09 = CModel("TWM09");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm09.Reset();
			twm09.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm09["TR_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "台车号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm09["TR_FROM_SPAN"].ToString().Trim() == "")
			{
				sprintf(s.msg, "出发跨不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm09["TR_TO_SPAN"].ToString().Trim() == "")
			{
				sprintf(s.msg, "到达跨不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM09 WHERE TR_NO =@tr_no and SEQ_NO =@seq_no";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tr_no", twm09["TR_NO"].ToString());
			cmd_inq.Parameters.Set("seq_no", twm09["SEQ_NO"].ToDecimal());
			Count = cmd_inq.ExecuteScalar();

			if (Count > 0)
			{
				sprintf(s.msg, "该台车[%s]及此序号已存在，无需新增。", (const char*)twm09["TR_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm09["REC_CREATE_TIME"] = datetime;
			twm09["REC_CREATOR"] = s.userid;
			twm09.Insert();
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