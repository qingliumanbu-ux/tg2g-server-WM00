/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 炼钢板坯垛位推荐预处理
**************************************************/

#include "stdafx.h"
#include "tep0002.h"

#include "twm04.h"
#include "twm0e.h"
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */


int f_wm00_pre_deal(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	
	//定义实体类
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);
	
	try
	{
		tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		Log::Trace("", __FUNCTION__, "MAT_NO = [{0}]", tmmsm01["MAT_NO"].ToString());

		if (!tmmsm01.Query("MAT_NO")){
			s.flag = -1;
			strcpy(s.msg, "板坯" + tmmsm01["MAT_NO"].ToString() + "查询失败！");
			return -1;
		}
		Log::Trace("", __FUNCTION__, "mat_acl_len = [{0}]", tmmsm01["MAT_ACT_LEN"].ToDecimal());
		//bcls_rec->GetColVal(1, 1,(T_INFO *)&tmmsm01_info);
		//仅对板坯实际长度做预处理，分析结果替换原先长度值
		if (tmmsm01["MAT_ACT_LEN"].ToDecimal() > 6990)
		{
			if (tmmsm01["MAT_ACT_LEN"].ToDecimal() > 12000)
			{
				tmmsm01["MAT_ACT_LEN"] = 3;
			}
			else
			{
				tmmsm01["MAT_ACT_LEN"] = 2;
			}
		}
		else
		{
			tmmsm01["MAT_ACT_LEN"] = 1;
		}

		//板坯去向预处理
		if (0 == strcmp(tmmsm01["MAT_DESTION"].ToString(), "00") || 0 == strcmp(tmmsm01["MAT_DESTION"].ToString(), "01"))
		{
			if (0 == strcmp(tmmsm01["SURFACE_DECIDE_CODE"].ToString(), "1"))
			{
				if ((tmmsm01["MAT_ACT_LEN"].ToDecimal() < 8000 && tmmsm01["MAT_ACT_LEN"].ToDecimal() >= 7000) || (tmmsm01["MAT_ACT_LEN"].ToDecimal() < 4500 && tmmsm01["MAT_ACT_LEN"].ToDecimal() >= 4200))
				{
					tmmsm01["MAT_DESTION"] = "00";
					//strcpy(tmmsm01["MAT_DESTION"].ToString(), "00");
				}
				if (tmmsm01["MAT_ACT_LEN"].ToDecimal() > 9600 || (tmmsm01["MAT_ACT_LEN"].ToDecimal() <= 5300 && tmmsm01["MAT_ACT_LEN"].ToDecimal() > 4600))
				{
					tmmsm01["MAT_DESTION"] = "01";
					//strcpy(tmmsm01["MAT_DESTION"].ToString(), "01");
				}
			}
		}
		else
		{
			tmmsm01["MAT_DESTION"] = "10";
			//strcpy(tmmsm01["MAT_DESTION"].ToString(), "10");
		}
		Log::Trace("", __FUNCTION__, "tmmsm01[MAT_DESTION] = [{0}]", tmmsm01["MAT_DESTION"].ToString());
		if (bcls_ret->Tables.Contains("PREDEAL")){
			Log::Trace("", __FUNCTION__, "AA");
			bcls_ret->Tables["PREDEAL"].Clone(tmmsm01);
			if (bcls_ret->Tables["PREDEAL"].Rows.get_Count()==0){
				bcls_ret->Tables["PREDEAL"].Rows.Add();
			}
			bcls_ret->Tables["PREDEAL"].Rows[0].Merge(tmmsm01);
		}
		else{
			Log::Trace("", __FUNCTION__, "BB");
			bcls_ret->Tables.Add();
			int kk = bcls_ret->Tables.get_Count() - 1;
			bcls_ret->Tables[kk].set_TableName("PREDEAL");
			bcls_ret->Tables["PREDEAL"].Clone(tmmsm01);
			bcls_ret->Tables["PREDEAL"].Rows.Add();
			bcls_ret->Tables["PREDEAL"].Rows[0].Merge(tmmsm01);
		}
		Log::Trace("", __FUNCTION__, "tmmsm01[MAT_DESTION] = [{0}]", bcls_ret->Tables["PREDEAL"].Rows[0]["MAT_DESTION"].ToString());
		/*blckNum = bcls_ret->AtBlkName("PreDeal");
		if (-1 != blckNum)
		{
			bcls_ret->SetColVal(blckNum, 1, (T_INFO *)&tmmsm01_info);
		}
		else
		{
			blckNum = bcls_ret->AddBlock();
			bcls_ret->SetBlkName(blckNum, "PreDeal");
			bcls_ret->SetColVal(blckNum, 1, (T_INFO *)&tmmsm01_info);
		}*/

	//	EDLog(1, 1, "mat_act_len = %lf", tmmsm01.mat_act_len);
	//	EDLog(1, 1, "%s", tmmsm01.mat_destion);



	
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


