/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-06-30
Description:	实物信息修改
**************************************************/

//框架头文件
#include "stdafx.h"

//函数申明

/*<remark>=========================================================
///<summary>
///实物信息修改
///<para>
///2.排序方式：MAT_NO
///</para>
///<para>数据库表：TWMB1 盘库实物物料表；
///<returns>修改传入的实物信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wmc1_upt);

int f_wmc1_upt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";

	/* 实体类定义 */
	CModel twmb1=CModel("TWMB1");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twmb1.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twmb1["STOCK_NO"].ToString() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twmb1["STOCK_PLACE_NO"].ToString() == "")
			{
				sprintf(s.msg, "库位号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			
			sqlstr = "SELECT COUNT(1) FROM TWMB1 WHERE MAT_NO =@mat_no ";
			
			
			if (twmb1.QueryCount("MAT_NO") < 1)
			{
				sprintf(s.msg, "该实物不存在，无法修改。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/*if (Count < 1)
			{
				sprintf(s.msg, "该实物不存在，无法修改。");
				throw CApplicationException(-1, s.msg, log.Location);
			}*/

			twmb1["REC_REVISE_TIME"] = datetime;
			twmb1["REC_REVISOR"] = s.userid;
			v_update = "FACTORY_DIV,MAT_NO,STOCK_NO,STOCK_PLACE_NO,LAYERNO,MAT_ACT_WT,"
		    "MAT_NUM,REC_REVISE_TIME,REC_REVISOR";
			twmb1.Update(v_update, "MAT_NO");
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