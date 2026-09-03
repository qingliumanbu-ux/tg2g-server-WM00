/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         JHZHAO
Version:		1.0
Date:			2016-03-05
Description:	板坯入库功能
**************************************************/

//框架头文件
#include "stdafx.h"
////程序用头文件
//#include "twma1.h"
//#include "tmmhp01.h"
//#include "twma2.h"

//函数申明
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); //调用入库队列

/*<remark>=========================================================
///<summary>
///板坯入库功能
///<para>
///2.排序方式：
///</para>
///<para>数据库表：TWMA0 倒躲队列；TWMA1 物料主档表
///<returns>执行预材料预入库功能</returns>
===========================================================</remark>*/

BM2F_ENTERACE(wm11b_confirm)

int f_wm11b_confirm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal rowCount = 0;

	/*程序用变量*/
	CString stock_no = "", to_stock_no = " ";
	CString stock_oper_order = "";
	CString v_mat_kind = "";
	CString v_mat_no = "";
	CString v_table_name = "";
	/* 实体类定义 */
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	CModel twma2 = CModel("TWMA2");
	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlwhere = "";

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);



	//调用移动函数
	EIClass bcls_wm03;
	if (!bcls_wm03.Tables.Contains("WM00QUE"))
	{
		bcls_wm03.Tables[0].set_TableName("WM00QUE");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "MAT_NUM");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "PLAN_NO");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "STOCK_NO");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "TRANS_TOOL");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "UNIT_CODE");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "MAT_DESTION");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "OPER_FLAG");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
		bcls_wm03.Tables[0].Columns.Add(DT_STRING, "TO_STOCK_NO");
		bcls_wm03.Tables["WM00QUE"].Rows.Add();
	}

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			//twma1.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
			v_mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
			stock_oper_order = bcls_rec->Tables[0].Rows[0]["STOCK_OPER_ORDER"].ToString().Trim();
			//stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();
			to_stock_no = bcls_rec->Tables[0].Rows[0]["TO_STOCK_NO"].ToString().Trim();

			Log::Trace("", __FUNCTION__, "v_mat_no = [{0}]", v_mat_no);
			Log::Trace("", __FUNCTION__, "v_mat_kind = [{0}]", v_mat_kind);
			Log::Trace("", __FUNCTION__, "stock_oper_order = [{0}]", stock_oper_order);
			Log::Trace("", __FUNCTION__, "stock_no = [{0}]", stock_no);


			if (v_mat_kind == "SM")
			{
				v_table_name = "TMMSM01";
			}
			else if (v_mat_kind == "HR")
			{
				v_table_name = "TMMHR01";
			}
			else if (v_mat_kind == "CR")
			{
				v_table_name = "TMMCR01";
			}
			else if (v_mat_kind == "HP")
			{
				v_table_name = "TMMHP01";
			}
			else if (v_mat_kind == "BW")
			{
				v_table_name = "TMMBW01";
			}
			else if (v_mat_kind == "SF")
			{
				v_table_name = "TMMSF01";
			}
			else
			{
				sprintf(s.msg, "v_mat_kind[%s]", (const char*)v_mat_kind);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			Log::Trace("", __FUNCTION__, "v_mat_kind\t[{0}]", v_mat_kind);

			CModel twma1 = CModel(v_table_name);

			twma1["MAT_NO"] = v_mat_no;


			if (!twma1.Query("MAT_NO"))
			{
				sprintf(s.msg, "主档表中不存在材料信息");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (stock_no.Trim() == ""){
				//sprintf(s.msg, "库区号不能为空，请选择库区号！！！");
				//sprintf(s.msg, "库区号不能为空，请选择库区号！！！");
				//throw CApplicationException(-1, s.msg, log.Location);
				stock_no = to_stock_no;
			}

			Log::Trace("", __FUNCTION__, "--------------------------调用库队列函数开始------------------------------");
			Log::Trace("", __FUNCTION__, "twma1.MAT_NO = {0}", twma1["MAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "twma1.UNIT_CODE = {0}", twma1["UNIT_CODE"].ToString());
			Log::Trace("", __FUNCTION__, "twma1.NEXT_UNIT_CODE = {0}", twma1["NEXT_UNIT_CODE"].ToString());

			bcls_wm03.Tables["WM00QUE"].Rows[0]["MAT_NO"] = twma1["MAT_NO"].ToString();
			bcls_wm03.Tables["WM00QUE"].Rows[0]["MAT_NUM"] = twma1["MAT_NUM"].ToDecimal();
			bcls_wm03.Tables["WM00QUE"].Rows[0]["TRANS_TOOL"] = " ";
			bcls_wm03.Tables["WM00QUE"].Rows[0]["UNIT_CODE"] = twma1["UNIT_CODE"].ToString();
			bcls_wm03.Tables["WM00QUE"].Rows[0]["NEXT_UNIT_CODE"] = twma1["NEXT_UNIT_CODE"].ToString();
			bcls_wm03.Tables["WM00QUE"].Rows[0]["STOCK_NO"] = stock_no;
			bcls_wm03.Tables["WM00QUE"].Rows[0]["TO_STOCK_NO"] = to_stock_no;

			bcls_wm03.Tables["WM00QUE"].Rows[0]["OPER_FLAG"] = "I";
			bcls_wm03.Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = stock_oper_order;
			bcls_wm03.Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER_DIV"] = " ";

			doFlag = f_wm00_queue(&bcls_wm03, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", __FUNCTION__, "--------------------------调用库队列函数结束------------------------------");
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