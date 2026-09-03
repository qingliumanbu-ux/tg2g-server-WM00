/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      吴新
Version:     1.0
Date:        2016-03-28
Description: 计算库位的当前高度、当前重量等
**************************************************/
#include "stdafx.h"
//#include "twma1.h"
//#include "twm04.h"
//#include "twma7.h"

BM2_FUNCTION_EXPORT
int f_wm00_pileinfocal(CString stock_no, CString stock_place_no, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ", sqlstr1 = " ", sqlstr2 = " ", s_message = " ";

	CString v_table_name = "";

	CDbCommand	comm_inq(conn);
	//定义表实体对象
	//CTWM04 twm04(conn);
	//CTWMA1 twma1(conn);
	//CTWMA7 twma7(conn);

	CModel twm04("TWM04");
	CModel twma7("TWMA7");


	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		////获取库位信息
		//Log::Trace("", __FUNCTION__, "stock_place_no=[{0}]", stock_place_no);
		//Log::Trace("", __FUNCTION__, "stock_no=[{0}]", stock_no);
		//twm04["STOCK_PLACE_NO"] = stock_place_no;
		//twm04["STOCK_NO"] = stock_no;

		//if (twm04["STOCK_PLACE_NO"].ToString().Trim() == "")
		//{
		//	return doFlag;
		//}

		//if (!twm04.Query("STOCK_NO, STOCK_PLACE_NO"))
		//{
		//	return doFlag;
		//}

		//if (twm04["STOCK_STATUS"].ToString().Trim() == "1")
		//{
		//	s_message = "垛位号：" + twm04["STOCK_PLACE_NO"].ToString() + "为封锁垛位，请核对！";
		//	sprintf(s.msg, s_message);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//////////////////////////////////计算实际高度///////////////////////////////////////////////////
		//twm04["PILE_MAT_HEI_ACT"] = 0;
		//twm04["PILE_MAT_NUM_ACT"] = 0;
		//twm04["PILE_MAT_WT_ACT"] = 0;
		//twm04["PRE_MAT_NUM"] = 0;
		//twm04["PRE_COM_HEIGHT"] = 0;
		//twm04["STOCK_STATUS"] = "0";
		//twm04["PILE_MAT_TUBE"] = 0;
		//

		//if (twm04["MAT_KIND"].ToString().Trim() == "SM")
		//{
		//	v_table_name = "TMMSM01";
		//}
		//else if (twm04["MAT_KIND"].ToString().Trim() == "HR")
		//{
		//	v_table_name = "TMMHR01";
		//}
		//else if (twm04["MAT_KIND"].ToString().Trim() == "CR")
		//{
		//	v_table_name = "TMMCR01";
		//}
		//else if (twm04["MAT_KIND"].ToString().Trim() == "BW")
		//{
		//	v_table_name = "TMMBW01";
		//}
		//else if (twm04["MAT_KIND"].ToString().Trim() == "SF")
		//{
		//	v_table_name = "TMMSF01";
		//}
		//else if (twm04["MAT_KIND"].ToString().Trim() == "HP")
		//{
		//	v_table_name = "TMMHP01";
		//}
		//else
		//{
		//	sprintf(s.msg, "物料种类【%s】无法识别。", (const char*)twm04["MAT_KIND"]);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//CModel twma1(v_table_name);

		//sqlstr =
		//	" SELECT * FROM " + v_table_name +" T1 JOIN TWMA2 T2"
		//	" ON T1.MAT_NO = T2.MAT_NO"
		//	" WHERE T2.STOCK_PLACE_NO = @stock_place_no";
		//comm_inq.SetCommandText(sqlstr);
		//comm_inq.Parameters.Set("stock_place_no", stock_place_no);
		//comm_inq.Parameters.Set("stock_no", stock_no);

		//comm_inq.ExecuteReader();
		//while (comm_inq.Read())
		//{
		//	comm_inq.Fetch(twma1);

		//	twm04["PILE_MAT_HEI_ACT"] = twm04["PILE_MAT_HEI_ACT"].ToDecimal() + twma1["MAT_ACT_THICK"].ToDecimal();
		//	twm04["PILE_MAT_NUM_ACT"] = twm04["PILE_MAT_NUM_ACT"].ToDecimal() + 1;
		//	twm04["PILE_MAT_WT_ACT"] = twm04["PILE_MAT_WT_ACT"].ToDecimal() + twma1["MAT_WT"].ToDecimal();
		//	if (twma1.IndexOf("MAT_TUBE") > 0)
		//	{
		//		twm04["PILE_MAT_TUBE"] = twm04["PILE_MAT_TUBE"].ToDecimal() + twma1["MAT_TUBE"].ToDecimal();
		//	}
		//	else
		//	{
		//		twm04["PILE_MAT_TUBE"] = twm04["PILE_MAT_TUBE"].ToDecimal() + 1;
		//	}
		//}
		//comm_inq.Close();

		//////////////////////////////////计算预约(命令)高度///////////////////////////////////////////////////
		//sqlstr =
		//	" SELECT * FROM TWMA7 T1"
		//	" WHERE STOCK_PLACE_NO_TO = @stock_place_no_to"
		//	" OR STOCK_PLACE_NO_FIN = @stock_place_no_to";
		//comm_inq.SetCommandText(sqlstr);
		//comm_inq.Parameters.Set("stock_place_no_to", stock_place_no);
		//comm_inq.Parameters.Set("stock_no_to", stock_no);

		//comm_inq.ExecuteReader();
		//while (comm_inq.Read())
		//{
		//	comm_inq.Fetch(twma7);

		//	twm04["PRE_MAT_NUM"] = twm04["PRE_MAT_NUM"].ToDecimal() + 1;
		//	twm04["PRE_COM_HEIGHT"] = twm04["PRE_COM_HEIGHT"].ToDecimal() + twma1["MAT_ACT_THICK"].ToDecimal();

		//}
		//comm_inq.Close();


		//if ((twm04["PRE_MAT_NUM"].ToDecimal() + twm04["PILE_MAT_NUM_ACT"].ToDecimal() < twm04["MAX_LAYER_COUNT"].ToDecimal()) &&
		//	twm04["PRE_MAT_NUM"].ToDecimal() > 0 &&
		//	twm04["PILE_MAT_NUM_ACT"].ToDecimal() > 0)
		//{
		//	twm04["STOCK_STATUS"] = "3";
		//}

		//if (twm04["PRE_MAT_NUM"].ToDecimal() + twm04["PILE_MAT_NUM_ACT"].ToDecimal() < twm04["MAX_LAYER_COUNT"].ToDecimal() &&
		//	twm04["PILE_MAT_NUM_ACT"].ToDecimal() == 0 &&
		//	twm04["PRE_MAT_NUM"].ToDecimal() > 0)
		//{
		//	twm04["STOCK_STATUS"] = "4";
		//}

		//if (twm04["PRE_MAT_NUM"].ToDecimal() + twm04["PILE_MAT_NUM_ACT"].ToDecimal() < twm04["MAX_LAYER_COUNT"].ToDecimal() &&
		//	twm04["PRE_MAT_NUM"].ToDecimal() == 0 &&
		//	twm04["PILE_MAT_NUM_ACT"].ToDecimal() > 0)
		//{
		//	twm04["STOCK_STATUS"] = "5";
		//}

		//if (twm04["PRE_MAT_NUM"].ToDecimal() + twm04["PILE_MAT_NUM_ACT"].ToDecimal() >= twm04["MAX_LAYER_COUNT"].ToDecimal())
		//{
		//	twm04["STOCK_STATUS"] = "9";
		//}

		//if (twm04["PRE_MAT_NUM"].ToDecimal() + twm04["PILE_MAT_NUM_ACT"].ToDecimal() == 0)
		//{
		//	twm04["STOCK_STATUS"] = "0";
		//}

		//twm04["PILE_TIME_LATELY"] = datetime;
		//twm04.Update(
			/*"PILE_TIME_LATELY,"
			"PILE_MAT_HEI_ACT,"
			"PILE_MAT_NUM_ACT,"
			"PILE_MAT_WT_ACT,"
			"PRE_MAT_NUM,"
			"PRE_COM_HEIGHT,"
			"PILE_MAT_TUBE,"
			"STOCK_STATUS"
			,
			"STOCK_NO,STOCK_PLACE_NO");*/
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


