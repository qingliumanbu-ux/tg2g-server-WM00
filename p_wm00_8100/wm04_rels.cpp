/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-06-17
Description:	库位释放
**************************************************/


//框架头文件
#include "stdafx.h"
//程序用头文件
//#include "twm04.h"


//函数申明

/*<remark>=========================================================
///<summary>
///库位释放
///<para>
///2.排序方式：STOCK_PLACE_NO
///</para>
///<para>数据库表：TWM04 仓库库位定义表；
///<returns>修改传入的库区信息</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm04_rels);

int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn);

int f_wm04_rels(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)

{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal Count = 0;
	CString v_update = "";

	/* 实体类定义 */
	//CTWM04 twm04(conn);
	CModel twm04("TWM04");

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);


	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			twm04.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (twm04["STOCK_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "库区号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["STOCK_PLACE_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "仓库库位号不能为空");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (twm04["STOCK_PLACE_TYPE"].ToString() != "B")
			{
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
			}
			twm04.Query("STOCK_PLACE_NO");
			if (twm04.QueryCount("STOCK_PLACE_NO") < 1)
			{
				sprintf(s.msg, "该库区该库位不存在，无法修改。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm04["STOCK_STATUS"].ToString() == "1")
			{
				twm04["STOCK_STATUS"] = "0";
				twm04.Update("STOCK_STATUS", "STOCK_PLACE_NO");
				doFlag = f_wm00_pileinfocal(twm04["STOCK_NO"].ToString(), twm04["STOCK_PLACE_NO"].ToString(), bcls_ret, conn);
				if (doFlag != 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else
			{
				sprintf(s.msg, "该库位未被封锁，无法释放。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			twm04["REC_REVISE_TIME"] = datetime;
			twm04["REC_REVISOR"] = s.userid;

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

