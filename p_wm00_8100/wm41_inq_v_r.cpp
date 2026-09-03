/*=========================================================================
//程序名称:		sm0002_inq_v_r
//隶属子系统:		SM00
//产品名称:		按量发货出厂计划材料查询
//创建人员:		E84194
//创建时间:		2020-11-03
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

//程序用头文件
//#include "tsm0002.h"
//#include "tsm0003.h"

// service入口
BM2F_ENTERACE(wm41_inq_v_r)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_inq_v_r(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	CString	blkName = "sm0002_inq_v_r";
	int fetchRowCount = 0;
	int	ret = 0;
	int doFlag = 0;
	int k = 0;

	CString	v_factory_div = "";
	CString	v_stock_no = "";
	CString	vehicle_no = "";



	CDbCommand cmd_inq(conn);
	CString sqlstr;
	try
	{
		/********************************
		*	读取传入的参数				*
		********************************/
		if (bcls_rec->Tables[0].Columns.Contains("stock_no"))		v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];	//仓库代码
		if (bcls_rec->Tables[0].Columns.Contains("factory_div"))		v_factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"];	//厂别
		if (bcls_rec->Tables[0].Columns.Contains("vehicle_no"))		vehicle_no = bcls_rec->Tables[0].Rows[0]["vehicle_no"];	//车号

		Log::Info("", __FUNCTION__, "STOCK_NO=[{0}]", v_stock_no);
		Log::Info("", __FUNCTION__, "FACTORY_DIV=[{0}]", v_factory_div);
		Log::Info("", __FUNCTION__, "vehicle_no=[{0}]", vehicle_no);

		/********************************
		*	动态显示定义字段			*
		********************************/
		sqlstr =

			//xhg20211020 改为补显示汇总，按材料显示明细，汇总用grid的汇总功能
			" SELECT  TRANSFER_PLAN_NO,BATCH_NO,HEAT_NO,MAT_NO,PROD_CNAME,SG_SIGN,MAT_THICK,MAT_LEN,"
			" 1 MAT_NUM,MAT_ACT_WT,MAT_THEORY_WT,MAT_WT "
			" FROM	TWMB5 "
			" WHERE	1=1 "
			" AND STOCK_NO	=	@v_stock_no "
			" AND vehicle_no	=	@vehicle_no ";
		cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("v_factory_div", v_factory_div);
		cmd_inq.Parameters.Set("v_stock_no", v_stock_no);
		cmd_inq.Parameters.Set("vehicle_no", vehicle_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		Log::Info("", __FUNCTION__, "sqlstr=[{0}],count=[{1}]", sqlstr, bcls_ret->Tables[0].Rows.get_Count());

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.msg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
