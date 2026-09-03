/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 炼钢板坯垛位分区属性计算
**************************************************/

#include "stdafx.h"
#include "tep0002.h"
#include "tmmsm01.h"
#include "twm04.h"
#include "twm0e.h"
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */
int f_wm00_pre_deal(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_wm00_pile_recom(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	CString c_pile_judge_type  = " ";
	CString c_pile_judge_code  = "XX";//初始化
	CString c_end_mark  = "00";
	CString c_pile_value  = "99";//初始化
	CString A1  = " ";
	CString A2  = " ";
	CString A3  = " ";
	CString A4 = " ";
	CString c_code  = " ";
	CString c_code_desc_2_content = " ";
	int v_value = 0;
	//定义实体类
	CTMMSM01 tmmsm01(conn);

	CTWM0E twm0e(conn);

	CDbCommand cmd_inq(conn);
	
	try
	{
		tmmsm01.MAT_NO = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		c_pile_judge_type = bcls_rec->Tables[0].Rows[0]["PILE_JUDGE_TYPE"];
		Log::Trace("", __FUNCTION__, "c_pile_judge_type = [{0}]", c_pile_judge_type);
		Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", tmmsm01.MAT_NO);
		
		bcls_rec->AddColName(1, "mat_no");
		bcls_rec->SetColVal(1, 1, "mat_no", tmmsm01.MAT_NO);
		Log::Debug("", __FUNCTION__, "f_ymsm_pre_deal------------------------------开始");
		doFlag = f_wm00_pre_deal(bcls_rec, bcls_ret,conn);
		if (0 != doFlag)
		{
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(doFlag, s.msg, s.svc_name);
		}
		Log::Debug("", __FUNCTION__, "f_ymsm_pre_deal------------------------------结束");
		tmmsm01.MergeFrom(bcls_ret->Tables["PREDEAL"].Rows[0]);
		//bcls_ret->GetColVal("PreDeal", 1, (T_INFO *)&tmmsm01_info);
		Log::Trace("", __FUNCTION__, "mat_act_len = [{0}]", tmmsm01.MAT_ACT_LEN);
		Log::Trace("", __FUNCTION__, "mat_destion = [{0}]", tmmsm01.MAT_DESTION);

		for (int i = 1; i <= 20; i++)//最多循环20次，避免死循环
		{
			twm0e.Reset();
			Log::Trace("", __FUNCTION__, "c_pile_judge_type = [{0}]", c_pile_judge_type);
			Log::Trace("", __FUNCTION__, "c_pile_judge_code = [{0}]", c_pile_judge_code);
			twm0e.PILE_JUDGE_TYPE = c_pile_judge_type;
			twm0e.PILE_JUDGE_CODE = c_pile_judge_code;
			if (!twm0e.Query("PILE_JUDGE_TYPE,PILE_JUDGE_CODE")){
				// 当指定的取不到值时考虑取默认为ZZ的
				twm0e.PILE_JUDGE_CODE = "ZZ";
				twm0e.Query("PILE_JUDGE_TYPE,PILE_JUDGE_CODE");
			}
			A1 = twm0e.NEXT_JUDGE_TYPE;
			A2 = twm0e.QUERY_CODE;
			A3 = twm0e.END_MARK;
			A4 = twm0e.PILE_VALUE;
			Log::Trace("", __FUNCTION__, "c_pile_value = [{0}]", c_pile_value);
			Log::Trace("", __FUNCTION__, "A1 = [{0}]", A1);
			Log::Trace("", __FUNCTION__, "A2 = [{0}]", A2);
			Log::Trace("", __FUNCTION__, "A3 = [{0}]", A3);
			Log::Trace("", __FUNCTION__, "A4 = [{0}]", A4);
			if (atoi(A4)<atoi(c_pile_value))
			{
				c_pile_value = A4;

			}
		
			//判断终止或者继续循环
			if (0 == strcmp(A3, "10") || 0 == strcmp(A3, " "))//静态表设置为终止
			{
				Log::Trace("", __FUNCTION__, "板坯垛位属性为 = [{0}]", c_pile_value);
				break;
			}
			else
			{
				c_pile_judge_type = A1;//将取出的值存入下一次需要查询的条件中
				//bcls_rec->SetColVal(1, 1, (T_INFO *)&tmmsm01_info);
				bcls_rec->Tables[0].Clone(tmmsm01);
				if (bcls_rec->Tables[0].Rows.get_Count() == 0){
					bcls_rec->Tables[0].Rows.Add();
				}
				bcls_rec->Tables[0].Rows[0].Merge(tmmsm01);
				c_pile_judge_code = bcls_rec->Tables[0].Rows[0][A2];
				//bcls_rec->GetColVal(1, 1, A2, c_pile_judge_code);//根据取出来的字段动态取表中的数据到A2
				//c_pile_judge_code若为空赋默认值XX
				if (0 == strcmp(c_pile_judge_code, " "))
				{
					c_pile_judge_code = "XX";
				}
			//	EDLog(1, 1, "c_pile_judge_type = [%s]", c_pile_judge_type);
				//EDLog(1, 1, "c_pile_judge_code = [%s]", c_pile_judge_code);
			}
		}
		//EDLog(1, 1, "板坯垛位属性为【%s】", c_pile_value);

		//返回计算的分区值
		if (!bcls_ret->Tables[0].Columns.Contains("PILE_VALUE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "PILE_VALUE");//
		}
		Log::Trace("", __FUNCTION__, "c_pile_value = [{0}]", c_pile_value);
		if (bcls_ret->Tables[0].Rows.get_Count() == 0){
			bcls_ret->Tables[0].Rows.Add();
		}
		bcls_ret->Tables[0].Rows[0]["PILE_VALUE"] = c_pile_value;



	
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


