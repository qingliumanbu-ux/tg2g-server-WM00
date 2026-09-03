/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         ZCG
Version:		1.0
Date:			2021年10月13日
Description:	库区与出库运输方式对应关系表
**************************************************/

//框架头文件
#include "stdafx.h"




BM2F_ENTERACE(wm000n_ins);

int f_wm000n_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	int ii = 0;
	CString stock_no = "";
	CString aim_stock_no = "";
	CString trnp_mode_code = "";
	CString rule_type = "";
	CString checkout_mark = "";
	CString unit_code = ""; 
	CString tip = "";
	/* 实体类定义 */
	CModel twm000n = CModel("TWM000N");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm000n.Reset();
			twm000n.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			//RULE_TYPE 1-库区出库运输方式是否校验规则 0-库区出库运输方式校验规则
			if (twm000n["RULE_TYPE"].ToString().Trim() != "0" && twm000n["RULE_TYPE"].ToString().Trim() != "1")
			{
				sprintf(s.msg, "规则类型[RULE_TYPE]只能为0或1");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			

			if (twm000n["RULE_TYPE"].ToString().Trim() == "1") {
				//CHECKOUT_MARK  0-不校验  1-须校验
				if (twm000n["CHECKOUT_MARK"].ToString().Trim() != "0" && twm000n["CHECKOUT_MARK"].ToString().Trim() != "1")
				{
					sprintf(s.msg, "是否校验只能为0或1");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				sqlwhere = sqlwhere + " and CHECKOUT_MARK = @checkout_mark ";
			}
			if (twm000n["STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlwhere = sqlwhere+ " and RULE_TYPE = @rule_type and STOCK_NO =@stock_no ";

			//如果是库区出库方式校验规则，则目标库区、运输方式不能为空
			if (twm000n["RULE_TYPE"].ToString().Trim() == "0") {
				if (twm000n["AIM_STOCK_NO"].ToString().Trim() == "")
				{
					sprintf(s.msg, "目标库区号不能为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}

				if (twm000n["TRNP_MODE_CODE"].ToString().Trim() != "")
				{
					sprintf(s.msg, "运输方式不能为空");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				sqlwhere = sqlwhere + " and AIM_STOCK_NO =@aim_stock_no and TRNP_MODE_CODE=@trnp_mode_code ";
			}
			
			if (twm000n["UNIT_CODE"].ToString().Trim() !="") {
				sqlwhere = sqlwhere + " and UNIT_CODE = @unit_code ";
			}
			
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM000N WHERE 1=1 ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			Log::Trace("", __FUNCTION__, "sqlstr				= [{0}]", (const char*)sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_no", twm000n["STOCK_NO"].ToString());
			cmd_inq.Parameters.Set("aim_stock_no", twm000n["AIM_STOCK_NO"].ToString());
			cmd_inq.Parameters.Set("rule_type", twm000n["RULE_TYPE"].ToString());
			cmd_inq.Parameters.Set("checkout_mark", twm000n["CHECKOUT_MARK"].ToString());
			cmd_inq.Parameters.Set("trnp_mode_code", twm000n["TRNP_MODE_CODE"].ToString());
			cmd_inq.Parameters.Set("unit_code", twm000n["UNIT_CODE"].ToString());
			Count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			if (Count > 0)
			{
				tip = "规则记录[库区号=" + twm000n["STOCK_NO"].ToString() +
					"  机组号" + twm000n["UNIT_CODE"].ToString() +
					"  目标库区" + twm000n["AIM_STOCK_NO"].ToString() +
					"  规则类型" + twm000n["RULE_TYPE"].ToString() +
					"  校验标志" + twm000n["CHECKOUT_MARK"].ToString() +
					"  运输方式" + twm000n["TRNP_MODE_CODE"].ToString() + "]不存在，无法修改";
				sprintf(s.msg, tip);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm000n["REC_CREATE_TIME"] = datetime;
			twm000n["REC_CREATOR"] = s.userid;
			
			
			sqlstr = " INSERT INTO TWM000N";
			twm000n.Insert();
			Log::Trace("", __FUNCTION__, " INSERT INTO TWM000N");

		}
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