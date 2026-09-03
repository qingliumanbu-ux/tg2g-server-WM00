/*=========================================================================
//程序名称:		sm0002_inq_m_v
//隶属子系统:		SM00
//产品名称:		已装车材料查询(按车辆)
//创建人员:		E84194
//创建时间:		2020-11-03
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"




//程序用头文件


// service入口
BM2F_ENTERACE(wm41_inq_m_v)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_inq_m_v(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	CString	blkName = "wm41_inq_m_v";
	int fetchRowCount = 0;
	int	ret = 0;
	int doFlag = 0;

	CString	datetime = "";
	CString	v_userid = "";
	CString	v_bill_of_lading_no = "";
	CString	vehicle_no = "";
	CString	stock_no = "";
	CString	v_inq_flag = "";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;

	CModel twmb5("TWMB5");
	CDbCommand cmd_inq(conn);
	CString sqlstr;

	try
	{
		/*获得传入参数*/
		v_userid = s.userid;
		/********************************
		*	读取传入的参数				*
		********************************/
		//v_bill_of_lading_no = bcls_rec->Tables[0].Rows[0]["bill_of_lading_no"].ToString().Trim();	//计划号
		vehicle_no = bcls_rec->Tables[0].Rows[0]["VEHICLE_NO"].ToString().Trim();	//仓库代码
		stock_no = bcls_rec->Tables[0].Rows[0]["STOCK_NO"].ToString().Trim();	//仓库代码

		//Log::Info("", __FUNCTION__, "v_bill_of_lading_no = [{0}]", v_bill_of_lading_no);
		Log::Info("", __FUNCTION__, "vehicle_no = [{0}]", vehicle_no);
		Log::Info("", __FUNCTION__, "stock_no = [{0}]", stock_no);

		/********************************
		*	动态显示定义字段			*
		********************************/


		sqlstr = " SELECT * FROM TWMB5 WHERE 1=1 "
			" AND VEHICLE_NO = @VEHICLE_NO  "
			" AND STOCK_NO = @STOCK_NO  ";


		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("VEHICLE_NO", vehicle_no);
		cmd_inq.Parameters.Set("STOCK_NO", stock_no);
		Log::Info("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);

		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		fetchRowCount = bcls_ret->Tables[0].Rows.get_Count();
		CFormattable arguments[] = { fetchRowCount };// 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到[{0}]条记录。", arguments, 1);//格式化字符串
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
	cmd_inq.Close();

	return doFlag;

}
