/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	仓库库位定义信息新增
**************************************************/

//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm04.h"


//函数申明

/*<remark>=========================================================
///<summary>
///仓库库位定义信息新增
///<para>
///2.排序方式：STOCK_NO,STOCK_PLACE_NO
///</para>
///<para>数据库表：TWM04 库区库位定义表；
///<returns>新增传入的库位信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm04_ins);

int f_wm04_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	CModel twm04 = CModel("TWM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twm04.Reset();
			twm04.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm04["STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["STOCK_PLACE_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库位号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MANAGE_ACCU"].ToString().Trim() == "")
			{
				sprintf(s.msg, "管理精度不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm04["MAX_LAYER_COUNT"].ToDecimal() == 0)
			{
				sprintf(s.msg, "最大堆放层数不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MAX_HEIGHT"].ToDecimal() == 0)
			{
				sprintf(s.msg, "最大高度不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MAX_WIDTH"].ToDecimal() == 0)
			{
				sprintf(s.msg, "最大宽度不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MAX_LEN"].ToDecimal() == 0)
			{
				sprintf(s.msg, "最大长度不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MAX_WT"].ToDecimal() == 0)//
			{
				sprintf(s.msg, "最大重量不能为0");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["STOCK_PLACE_TYPE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库位类型不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MAT_SHAPE_FLAG"].ToString() == " ")
			{
				sprintf(s.msg, "材料形态标志不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["STOCK_PLACE_TYPE"].ToString() == "D" &&
				twm04["DEV_DIV"].ToString() == " ")
			{
				sprintf(s.msg, "库位类型为设备时，设备区分代码不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["MAT_PILE_MODE"].ToString() == " ")
			{
				twm04["MAT_PILE_MODE"] = "H1";
			}
			if (twm04["STOCK_PLACE_NO_ACT"].ToString() == " ")
			{
				twm04["STOCK_PLACE_NO_ACT"] = twm04["STOCK_PLACE_NO"];
			}
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = "SELECT COUNT(1) FROM TWM04 WHERE STOCK_PLACE_NO =@stock_place_no ";
				break;
			}
			sqlstr = sqlstr + sqlwhere;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("stock_place_no", twm04["STOCK_PLACE_NO"].ToString());
			Count = cmd_inq.ExecuteScalar();
			Log::Trace("", __FUNCTION__, "11111111111111111");
			if (Count > 0)
			{
				sprintf(s.msg, "该库区[%s]该库位已存在，无需新增。", (const char*)twm04["STOCK_PLACE_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "twm04.STOCK_PLACE_NO");
			twm04["REC_CREATE_TIME"] = datetime;
			twm04["REC_CREATOR"] = s.userid;
			if (twm04["STOCK_PLACE_TYPE"].ToString() == "0")
			{
				if (twm04["ROWNO"].ToString().Trim() != "" &&
					twm04["ROWNO"].ToString().Trim() >= "0" &&
					twm04["ROWNO"].ToString().Trim() <= "99999")
				{
					twm04["STOCK_ROW_NO"] = atoi(twm04["ROWNO"]);
				}
				if (twm04["COLUMN_NO"].ToString().Trim() != "" &&
					twm04["COLUMN_NO"].ToString().Trim() >= "0" &&
					twm04["COLUMN_NO"].ToString().Trim() <= "99999")
				{
					twm04["STOCK_COL_NO"] = atoi(twm04["COLUMN_NO"]);
				}
				twm04["STOCK_LAYER_NO"] = twm04["LAYERNO"];
			}
			twm04["STOCK_STATUS"] = "0";
			sqlstr = " INSERT INTO TWM04";
			twm04.Insert();
			Log::Trace("", __FUNCTION__, "aaaaaa");
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