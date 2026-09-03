#include "stdafx.h"

/// <summary>
/// Description: 获得系统环境代码
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company: 上海宝信软件股份有限公司
/// Author: 李子阳
/// Version: 1.0
/// History:
/// 2015-09-20 李子阳 新建
/// </summary> 

BM2_FUNCTION_EXPORT
CString  f_wmg0_get_appame(CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	CString OUT_STR = "未找到";
	try
	{
		CDbCommand comm_inq(conn);
		comm_inq.SetCommandText(" SELECT ENAME FROM ES.TESAPPINFO ");		
		comm_inq.ExecuteReader();
		if(comm_inq.Read())
		{
			OUT_STR = comm_inq.GetString(1);	
		}
		comm_inq.Close();
	}
	catch (CException& ex)
	{
		EDLog(1, 1, "数据库操作失败 sqlcode = [%d] ,sqlmsg = [%s]", ex.GetCode(), (const char*)(ex.GetMsg()));
	}

	EDLog(1, 1, "当前系统环境代码是[%s] ", (const char*)OUT_STR);
	Log::Trace("", __FUNCTION__, "当前系统环境代码是 = [{0}]", OUT_STR);

	return OUT_STR;
}
