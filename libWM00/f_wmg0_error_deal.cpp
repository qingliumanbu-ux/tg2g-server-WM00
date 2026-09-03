#include "stdafx.h"

/// <summary>
/// Description: 根据异常对象进行统一报错处理
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company: 上海宝信软件股份有限公司
/// Author: 李子阳
/// Version: 1.0
/// History:
/// 2011-12-16 李子阳 新建
/// </summary> 

BM2_FUNCTION_EXPORT
int f_wmg0_error_deal(CException& ex)
{	
	int sqlcode = ex.GetCode(); 
	CString sqlmsg = ex.GetMsg();
	CString sqlsource = ex.GetSource();	
	
	//BM2::BM2_EXCEPTION_TYPE::BM2ET_APPLICATION_EXCEPTION = 1
	if(ex.GetExceptionType() == 1)
	{
		//如果是应用异常，由应用部分负责s.msg和s.sysmsg的设置
		;  
	}
	//BM2::BM2_EXCEPTION_TYPE::BM2ET_DB_EXCEPTION = 3
	else if(ex.GetExceptionType() == 3)
	{
		//DB2版本
		//如果是数据库异常，根据sqlcode进行统一报错处理
		if( sqlcode == 100)
			sprintf(s.msg,"数据库操作失败,数据不存在!");
		else if( sqlcode == -803 )
			sprintf(s.msg,"数据库操作失败,主键重复!");
		else if( sqlcode == -1438 )
			sprintf(s.msg,"数据库操作失败,数据类型或长度错误!");
		else if( sqlcode == -1722 )
			sprintf(s.msg,"数据库操作失败,无效的数据");
		else if( sqlcode == -1480 )
			sprintf(s.msg,"数据库操作失败,字符串超长!");
		else if( sqlcode == -99999 && ex.GetMsg().Trim() == "[IBM][CLI Driver] CLI0109E  String data right truncation. SQLSTATE=22001")			
			sprintf(s.msg,"数据库操作失败,字符串超长!");
		else
			sprintf(s.msg,"数据库操作失败 sqlcode = [%d] ,sqlmsg = [%s]", sqlcode ,(const char*)sqlmsg); 	

		//ORACLE版本
		//如果是数据库异常，根据sqlcode进行统一报错处理
		/*if( sqlcode == 1403)
			sprintf(s.msg,"数据库操作失败,数据不存在!");
		if( sqlcode == -1405)
			sprintf(s.msg,"数据库操作失败,当前表找不到数据!");
		else if( sqlcode == -1 )
			sprintf(s.msg,"数据库操作失败,主键重复!");
		else if( sqlcode == -1438 )
			sprintf(s.msg,"数据库操作失败,数据类型或长度错误!");
		else if( sqlcode == -1722 )
			sprintf(s.msg,"数据库操作失败,无效的数据");
		else if( sqlcode == -1480 )
			sprintf(s.msg,"数据库操作失败,字符串超长!");
		else
			sprintf(s.msg,"数据库操作失败 sqlcode = [%d] ,sqlmsg = [%s]", sqlcode ,(const char*)sqlmsg); */
	}
	//BM2::BM2_EXCEPTION_TYPE::BM2ET_EXCEPTION = 0
	//BM2::BM2_EXCEPTION_TYPE::BM2ET_SYSTEM_EXCEPTION = 2
	else  
	{
		//如果是系统异常或普通异常，一并直接报出
		strncpy(s.msg, (const char*)sqlmsg, sizeof(s.msg)-1);
	}

	s.flag = ex.GetCode();

	EDLog(1, 1, "Service处理失败,错误信息[%s]",s.msg);

	return -1;
}