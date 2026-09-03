/*=========================================================================
//程序名称:		wm41_del
//隶属子系统:	WM
//产品名称:		装车材料减少
//创建人员:		
//创建时间:		2022-09-05
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(wm41_del)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_del(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	int	ret = 0;
	int	doFlag = 0;
	int rows = 0;
	CModel twmb5("TWMB5");

	/* ***** 数据库操作类定义 ***** */
	CString sqlstr, sql;
	CDbCommand cmd_inq(conn), cmd(conn);
	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twmb5["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
			twmb5.Delete("MAT_NO");
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
