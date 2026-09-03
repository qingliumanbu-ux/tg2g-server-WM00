/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	库区定义信息新增
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm20a3.h"

//函数申明

/*<remark>=========================================================
///<summary>
///库区定义信息新增
///<para>
///2.排序方式：LOGIC_STOCK_NO，SEQ_NO，STOCK_PLACE_NO
///</para>
///<para>数据库表：TWM01 库区定义表；
///<returns>新增传入的库区信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm20a3_ins);

int f_wm20a3_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM20A3 twm20a3(conn);
	CModel twm20a3 = CModel("TWM20A3");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twm20a3.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm20a3["STOCK_NO"].ToString().Trim() != "")
			{
				twm20a3["FACTORY_DIV"] = " ";
			}

			if (twm20a3["STOCK_NO"].ToString().Trim() == "" &&
				twm20a3["FACTORY_DIV"].ToString().Trim() == "")
			{
				sprintf(s.msg, "厂别/库区不能都为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm20a3["CND_RELATION"].ToString().Trim() == "")
			{
				sprintf(s.msg, "字段名不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm20a3["ITEM_CNAME"].ToString().Trim() == "")
			{
				sprintf(s.msg, "字段中文不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm20a3["IN_OUT_DIV"].ToString().Trim() == "")
			{
				sprintf(s.msg, "入出库区分不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm20a3["SEQ_NO"].ToDecimal() <= 0)
			{
				sprintf(s.msg, "序号不能小于0");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr =
					" SELECT COUNT(1) FROM twm20a3"
					" WHERE FACTORY_DIV = @factory_div"
					" AND STOCK_NO = @stock_no "
					" AND IN_OUT_DIV = @in_out_div "
					" AND SEQ_NO = @seq_no ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("factory_div", twm20a3["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("stock_no", twm20a3["STOCK_NO"].ToString());
			cmd_inq.Parameters.Set("in_out_div", twm20a3["IN_OUT_DIV"].ToString());
			cmd_inq.Parameters.Set("seq_no", twm20a3["SEQ_NO"].ToDecimal());
			Count = cmd_inq.ExecuteScalar();

			if (Count > 0)
			{
				sprintf(s.msg, "已存在，无需新增。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm20a3["REC_CREATE_TIME"] = datetime;
			twm20a3["REC_CREATOR"] = s.userid;
			twm20a3.Insert();
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