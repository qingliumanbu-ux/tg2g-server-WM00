/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-12-20
Description: 表达式解析
**************************************************/

#include "stdafx.h"		// 框架头，不可删除 
#include "WM_AUTO.h"

BM2_FUNCTION_EXPORT
int f_rpn(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	bcls_ret->Tables[0].Columns.Clear();
	bcls_ret->Tables[0].Columns.Add(DT_BOOLEAN, "RESULT");

	try
	{
		if (bcls_rec->Tables.IndexOf("INFO_STRING") < 0 ||
			bcls_rec->Tables["INFO_STRING"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_rpn中找不到接收块名[INFO_STRING]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取传入参数
		CString info_str = bcls_rec->Tables["INFO_STRING"].Rows[0]["INFO_STR"].ToString().Trim();
		Log::Trace("", __FUNCTION__, "info_str  = [{0}]", info_str);

		if (info_str == "")
		{
			sprintf(s.msg, "传入数据有空值");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		RPN rpn(info_str);
		CString out = "";
		if (rpn.Parse(rpn.INFOstring))
		{			
			for (list<OperationBase>::iterator map1 = rpn.m_tokens.begin(); map1 != rpn.m_tokens.end(); map1++)
			{		
				out = out + (*map1).Info_String;
				Log::Trace("", __FUNCTION__, "Info_String【{0}】", (*map1).Info_String);
			}
		}
		else
		{
			Log::Trace("", __FUNCTION__, "表达式【{0}】解析失败", rpn.INFOstring);
		}
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["RESULT"] = rpn.Evaluate(1);
		Log::Trace("", __FUNCTION__, "计算【{0}】结果为：【{1}】", rpn.INFOstring, bcls_ret->Tables[0].Rows[0]["RESULT"].ToString());

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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}





