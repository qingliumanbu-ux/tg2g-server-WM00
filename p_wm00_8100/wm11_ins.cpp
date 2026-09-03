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
//#include "twm01.h"

#if defined _SYS_PES || defined _SYS_MES
//#include "tsi0021.h"
#endif

using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//函数申明

/*<remark>=========================================================
///<summary>
///车号顺序定义信息新增
///<para>
///2.排序方式：STOCK_NO
///</para>
///<para>数据库表：TWM11 库区定义表；
///<returns>新增传入的库区信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm11_ins);

int f_wm11_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM01 twm01(conn);
	CModel twm01 = CModel("TWM11");
#if defined _SYS_PES || defined _SYS_MES
	//CTSI0021 tsi0021(conn);
	//CModel tsi0021 = CModel("TSI0021");
#endif

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm01.Reset();
			twm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", __FUNCTION__, "twm11.VEHICLE_NO[{0}]", twm01["VEHICLE_NO"].ToString());
			Log::Trace("", __FUNCTION__, "i[{0}]", i);

			if (twm01["VEHICLE_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "车号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm01["SEQ_NO"].ToDecimal() <= 0)
			{
				sprintf(s.msg, "序号必须大于0");
				throw CApplicationException(-1, s.msg, log.Location);
			}




			twm01["VEHICLE_NO"] = twm01["VEHICLE_NO"].ToString().ToUpper();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM11 WHERE VEHICLE_NO =@vehicle_no ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("vehicle_no", twm01["VEHICLE_NO"].ToString());
			Log::Trace("", "", "VEHICLE_NO=[{0}]", twm01["VEHICLE_NO"].ToString());
			Count = cmd_inq.ExecuteScalar();
			if (Count > 0)
			{
				sprintf(s.msg, "该车号[%s]已存在，无需新增。", (const char*)twm01["VEHICLE_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "", "Count[{0}]", Count);
			twm01["REC_CREATE_TIME"] = datetime;
			twm01["REC_CREATOR"] = s.userid;
			//twm01.Print();
			sqlstr = " INSERT INTO TWM11";
			twm01.Insert();

//#if defined _SYS_PES || defined _SYS_MES
//			tsi0021["STOCK_NO"] = twm01["STOCK_NO"];
//			if (!tsi0021.Query("STOCK_NO"))
//			{
//				tsi0021.CopyFrom(twm01);
//
//				sqlstr = "INSERT INTO TSI0021";
//				tsi0021["REC_CREATOR"] = s.userid;
//				tsi0021["REC_CREATE_TIME"] = datetime;
//				tsi0021.Insert();
//			}
//			else
//			{
//				tsi0021.CopyFrom(twm01);
//
//				sqlstr = "UPDATE TSI0021";
//				tsi0021.Update(
//					" REC_REVISOR, REC_REVISE_TIME,"
//					" COMPANY_CODE, COMPANY_NAME,"
//					" STOCK_DESC, FACTORY_DIV,"
//					" STOCK_TYPE_CODE, STOCK_NO_CLASS,"
//					" STOCK_ADDR, PHONE_NO,"
//					"STOCK_CAPACITY_NUM,"
//					"STOCK_CAPACITY_WT,"
//					" MAT_LINE_TYPE, MAT_KIND,"
//					" REMARK",
//					"STOCK_NO");
//			}
//#endif
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

