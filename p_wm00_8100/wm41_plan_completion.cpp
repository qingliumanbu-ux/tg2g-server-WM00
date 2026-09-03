/*=========================================================================
//程序名称:		wm41_revoke
//隶属子系统:	WM00
//产品名称:		计划完成
//创建人员:
//创建时间:		2022-09-05
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

// service入口
int f_7000a5_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2F_ENTERACE(wm41_plan_completion)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_plan_completion(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	int	ret = 0;
	int	doFlag = 0;
	CString transfer_status = "";
	CString transfer_plan_no = "";
	CString plan_type = "";
	CString mat_kind = "";

	/*实体对象*/
	CModel twm41("TWM41");
	CModel twm42("TWM42");
	CModel twmb5("TWMB5");
	/* ***** 数据库操作类定义 ***** */
	CString sqlstr;
	CDbCommand cmd_inq(conn);
	CDataTable info;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		if (bcls_rec->Tables.IndexOf("7000A5") < 0)  //按量计划完成电文块
		{
			bcls_rec->Tables.Add("7000A5");
			bcls_rec->Tables["7000A5"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
			bcls_rec->Tables["7000A5"].Columns.Add(DT_STRING, "FLAG");
			bcls_rec->Tables["7000A5"].Rows.Add();
		}

		transfer_plan_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"].ToString().Trim();
		plan_type = bcls_rec->Tables[0].Rows[0]["PLAN_TYPE"].ToString().Trim();
		twmb5["TRANSFER_PLAN_NO"] = transfer_plan_no;
		if (twmb5.QueryCount("TRANSFER_PLAN_NO") > 0)  //查询是否有已装车的材料
		{
			sprintf(s.msg, "完成计划前请把已装车的材料卸车或者出库!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		else
		{
			if (plan_type == "L")  //按量转库计划不会自己结束（按件是可以的）所以在这里发送按量计划完成电文
			{
				//if (twm41["DELIVY_WT"].ToDecimal() <= twm41["TOTAL_WEI"].ToDecimal()-10)  //这里的10是随便写的 这里目的主要是不让转库的重量跟计划重量相差过大（可以依据厂别的需求可有可无）
				//{
				//	sprintf(s.msg, "[%s]的材料信息不存在。", (const char*)mat_no);
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}
				bcls_rec->Tables["7000A5"].Rows[0]["TRANSFER_PLAN_NO"] = transfer_plan_no;
				bcls_rec->Tables["7000A5"].Rows[0]["FLAG"] = "1";   //这里四级说先写固定值1 后期可能有调整...
				if (f_7000a5_snd(bcls_rec, bcls_ret, conn) != 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

			else
			{
				sprintf(s.msg, "计划类型为空,请检查!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
			twm41.Delete("TRANSFER_PLAN_NO");
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
