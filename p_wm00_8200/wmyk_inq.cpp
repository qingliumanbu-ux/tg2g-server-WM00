//框架公用头文件，勿删
#include "stdafx.h" 

/// <summary>
/// Description: 全厂库区图查询[twmg00]
/// Copyright: Baosight Software LTD.co Copyright (c) 2016
/// Company: 上海宝信软件股份有限公司
/// Author: 徐瞻
/// Version: 1.0
/// History:
/// 2016-09-27 徐瞻 新建
/// </summary> 

int f_wmg0_error_deal(CException& ex);

BM2F_ENTERACE(wmyk_inq)
int f_wmyk_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//返回值，勿删
	int doFlag = 0;
	//定义变量
	CString sql = "";
	//将业务代码包含在try-catch结构中
	CTracer log(__FUNCTION__);

	try
	{
		sql = "SELECT STOCK_PLACE_NO,STOCK_NO,HALL_NO,ROWNO,COLUMN_NO FROM TWM04 WHERE STOCK_NO='S01' AND STOCK_PLACE_TYPE='0'";
		Db::QueryTable(sql, bcls_ret->Tables[0]);

		bcls_ret->Tables.Add("MAX");
		sql = "SELECT max(INT(ROWNO)) as ROWNO,max(INT(COLUMN_NO)) as COLUMN_NO FROM TWM04 WHERE STOCK_NO='S01' AND STOCK_PLACE_TYPE='0'";
		Db::QueryTable(sql, bcls_ret->Tables[1]);

		bcls_ret->Tables.Add("");
		sql = "select stock_no,hall_no from twm04 where stock_no!='' and hall_no!='' group by stock_no,hall_no order by stock_no,hall_no";
		Db::QueryTable(sql, bcls_ret->Tables[2]);


	}
	catch (CException& ex)  //用于捕获数据库操作异常
	{
		//统一报错处理
		doFlag = f_wmg0_error_deal(ex);
	}

	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}
