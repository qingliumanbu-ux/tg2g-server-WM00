/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2017-12-20
Description: 垛位推荐
**************************************************/

#include "stdafx.h"		// 框架头，不可删除 
#include "WM_AUTO.h"

BM2_FUNCTION_EXPORT
int f_auto(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	CString sql(""), rpn("");

	CDataTable tab_area, tab_rules;  //存放区域列表,存放搜索规则集

	tab_area.Columns.Add(DT_DECIMAL, "AREA_NO");

	bcls_ret->Tables[0].Columns.Clear();
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");

	try
	{
		if (bcls_rec->Tables.IndexOf("AUTO_INFO_IN") < 0 ||
			bcls_rec->Tables["AUTO_INFO_IN"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_auto中找不到接收块名[AUTO_INFO_IN]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取传入参数
		CString mat_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["MAT_NO"].ToString();
		CString stock_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_NO"].ToString();
		CString stock_oper_order=bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["STOCK_OPER_ORDER"].ToString();
		Log::Trace("", __FUNCTION__, "MAT_NO={0}", mat_no);
		Log::Trace("", __FUNCTION__, "STOCK_NO={0}", stock_no);
		Log::Trace("", __FUNCTION__, "STOCK_OPER_ORDER={0}", stock_oper_order);

		WmsMat INFOMAT;
		INFOMAT.Setinfo(mat_no);
		Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO={0}", INFOMAT.STOCK_PLACE.STOCK_PLACE_NO);

		WM_tree tree_rule;
		tree_rule.bulid_tree(stock_no);
		CString area= tree_rule.search_tree();
		Log::Trace("", __FUNCTION__, "area={0}", area);

		//执行搜索规则表达式，获取区域
		list<WmsArea> list_area;
		
		//执行搜索规则表达式，获取区域
		for (list<WmsArea>::iterator map1 = list_area.begin(); map1 != list_area.end(); map1++)
		{
			if ((*map1).search(bcls_ret) > 0)
			{
				break;
			}
			else
			{
				continue;
			}
		}
		
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