/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	行车属性配置信息修改
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm06.h"


//函数申明

/*<remark>=========================================================
///<summary>
///行车属性配置信息修改
///<para>
///2.排序方式：CRANE_NO
///</para>
///<para>数据库表：TWM06 行车属性配置信息表；
///<returns>修改传入的行车属性配置信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm06_upt);

int f_wm06_upt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";

	/* 实体类定义 */
	//CTWM06 twm06(conn);
	CModel twm06 = CModel("TWM06");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twm06.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm06["CRANE_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "吊车号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm06["CRA_WEI_MAX"].ToDecimal() == 0)
			{
				sprintf(s.msg, "吊具最大重量不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm06["CRA_NUM_MAX"].ToDecimal() == 0)
			{
				sprintf(s.msg, "吊具最大块数不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM06 WHERE CRANE_NO =@crane_no ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("crane_no", twm06["CRANE_NO"].ToString());
			Count = cmd_inq.ExecuteScalar();

			if (Count < 1)
			{
				sprintf(s.msg, "该吊车不存在，无法修改。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm06["REC_REVISE_TIME"] = datetime;
			twm06["REC_REVISOR"] = s.userid;
			v_update = "SEQ_NO,HALL_NO,CRANE_STATUS,CRA_WEI_MAX,CRA_NUM_MAX,CRA_DEEPTH,CRA_WID_DIFF,"
				"MAT_THICK_FR,MAT_THICK_TO,MAT_LENTH_FR,MAT_LENTH_TO,REC_REVISE_TIME,REC_REVISOR";
			twm06.Update(v_update, "CRANE_NO");
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
