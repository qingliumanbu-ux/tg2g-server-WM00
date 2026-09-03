/*=========================================================================
//程序名称:		wm41_add
//隶属子系统:	WM
//产品名称:		装车材料增加
//创建人员:
//创建时间:		2022-09-05
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//程序用头文件

// service入口
BM2F_ENTERACE(wm41_confirm)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_confirm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	int	ret = 0;
	int	doFlag = 0;
	CString transfer_status = "";
	CString transfer_plan_no = "";

	CString datetime = "";
	/*实体对象*/
	CModel twm41("TWM41");
	/* ***** 数据库操作类定义 ***** */
	CString sqlstr;
	CDbCommand cmd_inq(conn);

	try
	{

		
		transfer_status = bcls_rec->Tables[1].Rows[0]["TRANSFER_STATUS"].ToString().Trim();
		transfer_plan_no = bcls_rec->Tables[1].Rows[0]["TRANSFER_PLAN_NO"].ToString().Trim();
		twm41["TRANSFER_STATUS"] = transfer_status;
		twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
		twm41.Update("TRANSFER_STATUS","TRANSFER_PLAN_NO");
		//后续确定给四级发送电文


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
