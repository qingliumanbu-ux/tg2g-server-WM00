/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 炼钢板坯垛位最终确认推荐
**************************************************/

#include "stdafx.h"
#include "tep0002.h"
#include "tmmsm01.h"
#include "twm04.h"
#include "twm0e.h"
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */
int f_wm00_pile_comf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_wm00_pile_final_recom(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	int		flag = 0;
	int		a = 0;
	CString	b = " ";
	CString	a_flag = "0";//用于区别垛位判断时是否是板坯的原始属性值，0：板坯属性，1:扩展推荐	
	CString	b1 = " ";
	int		b3 = 0;
	CString	v_pos = "000";//用于参数传递
	CString v_mat_no = " ";
	CString v_stock_place_no = " ";
	//定义实体类
	CTMMSM01 tmmsm01(conn);

	CTWM0E twm0e(conn);

	CDbCommand cmd_inq(conn);
	
	EIClass bcls_pile_comf;
	bcls_pile_comf.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
	bcls_pile_comf.Tables[0].Columns.Add(DT_STRING, "A");//
	bcls_pile_comf.Tables[0].Columns.Add(DT_STRING, "A_FLAG");//
	bcls_pile_comf.Tables[0].Columns.Add(DT_STRING, "POS");//
	bcls_pile_comf.Tables[0].Rows.Add();
	try
	{
		v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		b = bcls_rec->Tables[0].Rows[0]["B"];
		v_pos = bcls_rec->Tables[0].Rows[0]["POS"];

		//增加判断，如果传入的值是空值则不处理
		if (0 == strcmp(b, " ") || 0 == strcmp(b, "0"))
		{
			s.flag = -1;
			strcpy(s.msg, "**传入的属性值为空值或0，直接退出(第二层)**！");
			return -1;
		}

		//不在读取tmmsm01表
		a = atoi(b); 
		a_flag = "0";//2016-10-15：推荐的垛位属性值是否是扩展的，0：没有扩展，1：扩展了
		//strcpy(a_flag, "0");//2016-10-15：推荐的垛位属性值是否是扩展的，0：没有扩展，1：扩展了
		//	正常情况下的垛位推荐，推荐板坯分区属性对应的垛位
		flag = 0;
		Log::Debug("", __FUNCTION__, "板坯号为 = [{0}]", v_mat_no);
		Log::Debug("", __FUNCTION__, "板坯垛位属性值为 = [{0}]", b);
		Log::Debug("", __FUNCTION__, "判断垛位属性值标记 = [{0}]", a_flag);

		
		bcls_pile_comf.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
		bcls_pile_comf.Tables[0].Rows[0]["A"] = b3;
		bcls_pile_comf.Tables[0].Rows[0]["A_FLAG"] = a_flag;
		bcls_pile_comf.Tables[0].Rows[0]["POS"] = v_pos;

		//垛位分区推荐
		doFlag = f_wm00_pile_comf(&bcls_pile_comf, bcls_ret, conn);
		if (0 != doFlag)
		{
			Log::Debug("", __FUNCTION__, "**调用垛位判断失败(第二层)！**", a_flag);
			//EDLog(1, 1, "**调用垛位判断失败(第二层)！**", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(doFlag, s.msg, s.svc_name);
		}

		EDLog(1, 1, "**调用垛位分区判断程序(第二层)！**");
		v_stock_place_no = bcls_ret->Tables[0].Rows[0]["COMF_STOCK"];
	//	bcls_ret->GetColVal(1, 1, "comf_stock", v_stock_place_no);

		if (0 != strcmp(v_stock_place_no, "ZZZ"))//当该分区推荐失败时会返回ZZZ，成功则返回计算的垛位
		{
			flag = 1;
			Log::Debug("", __FUNCTION__, "**推荐确认垛位为【%s】(第二层)**");
			//EDLog(1, 1, "**推荐确认垛位为【%s】(第二层)**", v_stock_place_no);
		}
		else
		{
			//EDLog(1, 1, "**垛位推荐失败，赋予默认值X01(第二层)**");
			v_stock_place_no = "X01";
			//strcpy(v_stock_place_no, "X01");
		}
		//增加返回值
		if (!bcls_ret->Tables[0].Columns.Contains("TARG_POS"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "TARG_POS");
		}
		if (bcls_ret->Tables[0].Rows.get_Count() == 0){
			bcls_ret->Tables[0].Rows.Add();
		}
		bcls_ret->Tables[0].Rows[0]["TARG_POS"] = v_stock_place_no;
		//bcls_ret->SetColName(1, 1, "targ_pos");
		//bcls_ret->SetColVal(1, 1, "targ_pos", v_stock_place_no);
		
		



	
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
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


