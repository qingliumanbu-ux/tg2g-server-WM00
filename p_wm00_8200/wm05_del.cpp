/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	库区定义信息删除
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm05.h"

//函数申明

/*<remark>=========================================================
///<summary>
///库区定义信息删除
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWM01 库区定义表；
///<returns>修改传入的库区信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm05_del);

int f_wm05_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM05 twm05(conn);
	CModel twm05 = CModel("TWM05");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{

		twm05.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (twm05["STOCK_PLACE_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "库位号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twm05["SEQ_NO"].ToDecimal() == 0)
		{
			sprintf(s.msg, "序号不能为0");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (twm05["LOGIC_STOCK_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "逻辑库区号不能为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr = "SELECT COUNT(1) FROM TWM05 WHERE STOCK_PLACE_NO =@stock_place_no "
				" AND SEQ_NO = @seq_no "
				" AND LOGIC_STOCK_NO = @logic_stock_no ";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("stock_place_no", twm05["STOCK_PLACE_NO"].ToString());
		cmd_inq.Parameters.Set("seq_no", twm05["SEQ_NO"].ToDecimal());
		cmd_inq.Parameters.Set("logic_stock_no", twm05["LOGIC_STOCK_NO"].ToString());
		Count = cmd_inq.ExecuteScalar();

		if (Count < 1)
		{
			sprintf(s.msg, "该逻辑库区对应的该库位不存在，无需删除。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		twm05.Delete("STOCK_PLACE_NO,SEQ_NO,LOGIC_STOCK_NO");

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