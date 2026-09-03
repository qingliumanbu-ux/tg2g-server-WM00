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
//#include "twm20a2.h"


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

BM2F_ENTERACE(wm20a2_del);

int f_wm20a2_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM20A2 twm20a2(conn);
	CModel twm20a2 = CModel("TWM20A2");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{

		twm20a2.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		if (twm20a2["STOCK_NO"].ToString().Trim() != "")
		{
			twm20a2["FACTORY_DIV"] = " ";
		}

		if (twm20a2["STOCK_NO"].ToString().Trim() == "" &&
			twm20a2["FACTORY_DIV"].ToString().Trim() == "")
		{
			sprintf(s.msg, "厂别/库区不能都为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//if (twm20a2.SEQ_NO == 0)
		//{
		//	sprintf(s.msg, "序号不能为0");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		twm20a2["SEQ_NO"] = 1;



		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
			sqlstr =
				" SELECT COUNT(1) FROM twm20a2"
				" WHERE FACTORY_DIV = @factory_div"
				" AND STOCK_NO = @stock_no "
				" AND SEQ_NO = @seq_no ";
			break;
		}
		sqlstr = sqlstr + sqlwhere;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("factory_div", twm20a2["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("stock_no", twm20a2["STOCK_NO"].ToString());
		cmd_inq.Parameters.Set("seq_no", twm20a2["SEQ_NO"].ToDecimal());
		Count = cmd_inq.ExecuteScalar();

		if (Count < 1)
		{
			sprintf(s.msg, "不存在，无需删除。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		twm20a2.Delete("FACTORY_DIV,STOCK_NO,SEQ_NO");

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