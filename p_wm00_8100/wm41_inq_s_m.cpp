/*=========================================================================
//程序名称:		wm41_inq_s_m
//隶属子系统:	WM00
//产品名称:		点击库位号时查询对应库位号的材料
//创建人员:		KE2017
//创建时间:		2022-09-27
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"
// service入口
BM2F_ENTERACE(wm41_inq_s_m)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_inq_s_m(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	CString	blkName = "wm41_inq_s_m";
	int fetchRowCount = 0;
	int	ret = 0;
	int doFlag = 0;
	int k = 0;

	CString	datetime = "";
	CString	date = "";
	CString	time = "";
	CString	factory_div = "";
	CString	v_userid = "";
	CString	v_transfer_plan_no = "";
	CString	v_stock_no = "";
	CString	v_delivy_plan_status = "";
	CString	v_order_no = "";
	CString v_psc = "";
	CString v_stock_place_no = "";
	CString v_heat_no = "";
	CString v_plan_type = "";
	CString v_mat_kind = "";
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;
	date = datetime.Substring(0, 8);
	time = datetime.Substring(8, 6);

	CModel twm42("TWM42");
	CModel twm41("TWM41");

	CDbCommand cmd_inq(conn);
	CString sqlstr, sqlstrwhere;
	try
	{
		/********************************
		*	读取传入的参数				*
		********************************/
		if (bcls_rec->Tables[0].Columns.Contains("transfer_plan_no"))	v_transfer_plan_no = bcls_rec->Tables[0].Rows[0]["transfer_plan_no"];	//计划号
		if (bcls_rec->Tables[0].Columns.Contains("stock_no"))		v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];	//仓库代码
		if (bcls_rec->Tables[0].Columns.Contains("factory_div"))		factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"];	//厂别
		if (bcls_rec->Tables[0].Columns.Contains("stock_place_no"))			v_stock_place_no = bcls_rec->Tables[0].Rows[0]["stock_place_no"];	//轧批号
		if (bcls_rec->Tables[0].Columns.Contains("heat_no"))			v_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"];	//炉号
		if (bcls_rec->Tables[0].Columns.Contains("plan_type"))			v_plan_type = bcls_rec->Tables[0].Rows[0]["plan_type"];	//计划类型
		if (bcls_rec->Tables[0].Columns.Contains("mat_kind"))			v_mat_kind = bcls_rec->Tables[0].Rows[0]["mat_kind"];	//物料类型

		//Log::Info("", __FUNCTION__, "ORDER_NO=[{0}]", v_order_no);
		Log::Info("", __FUNCTION__, "TRANSFER_PLAN_NO=[{0}]", v_transfer_plan_no);
		Log::Info("", __FUNCTION__, "STOCK_NO=[{0}]", v_stock_no);
		Log::Info("", __FUNCTION__, "FACTORY_DIV=[{0}]", factory_div);
		Log::Info("", __FUNCTION__, "BATCH_NO=[{0}],HEAT_NO=[{1}]", v_stock_place_no, v_heat_no);
		/********************************
		*	动态显示定义字段			*
		********************************/

		twm41["TRANSFER_PLAN_NO"] = v_transfer_plan_no;
		twm41["MAT_KIND"] = v_mat_kind;
		if (!twm41.Query("TRANSFER_PLAN_NO,MAT_KIND"))
		{
			CFormattable arguments[] = { v_mat_kind, v_transfer_plan_no };
			CMessageFormat::Format(s.msg, "读取转库计划表出错，物料类型=[{0}]，计划号[{1}]", arguments, 2);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Info("", __FUNCTION__, "TRNP_MODE_CODE=[{0}]", twm41["TRNP_MODE_CODE"].ToString());

		sqlstr = "SELECT  '" + twm41["TRANSFER_PLAN_NO"].ToString() + "' AS TRANSFER_PLAN_NO, "
			"			b.MAT_NO, b.MAT_THICK, b.MAT_LEN, "
			"			b.MAT_ACT_LEN,b.SG_SIGN,b.STOCK_PLACE_NO,b.STOCK_NO,b.MAT_NUM,"
			"			b.MAT_THEORY_WT,b.MAT_ACT_WT,b.LAYERNO,b.ORDER_NO,"
			"			b.TRANSFER_PLAN_NO,b.MAT_WIDTH, "
			"           b.MAT_KIND,b.PROD_CODE, b.PROD_TIME , "
			;
		if (twm41["MAT_KIND"].ToString() == "BW") {
			sqlstr += " (CASE WHEN b.MAT_SHAPE_FLAG='7' THEN b.LAYERNO || b.ROWNO ||b.COLUMN_NO ELSE b.LAYERNO ||b.COLUMN_NO END) STOCK_PLACE_POSITION, ";
			sqlstr += " b.ROLL_PLAN_NO AS BATCH_NO ";
		}
		else if (twm41["MAT_KIND"].ToString() == "SM") {
			sqlstr += " b.LAYERNO ||b.COLUMN_NO STOCK_PLACE_POSITION, ";
			sqlstr += " b.HEAT_NO ";
		}

		if (v_plan_type.Trim() == "J")
		{
			sqlstr += " FROM	twm42 a , TMM" + twm41["MAT_KIND"].ToString() + "01 b  where 1=1 "
				" AND	a.TRANSFER_PLAN_NO =@v_transfer_plan_no "
				" AND	b.STOCK_NO		=	@v_stock_no "
				" AND	a.MAT_NO		=	b.MAT_NO "
				" AND	NOT EXISTS (SELECT 1 FROM TWMB5 WHERE MAT_NO=B.MAT_NO) ";//2020应该排除已装车材料
		}
		else
		{

			sqlstr += " FROM  TMM" + twm41["MAT_KIND"].ToString() + "01 b  where 1=1 "
				" AND	b.TRANSFER_PLAN_NO = ' ' "
				" AND	b.STOCK_NO		=	@v_stock_no "
				" AND	NOT EXISTS (SELECT 1 FROM TWMB5 WHERE MAT_NO=B.MAT_NO) "//2020应该排除已装车材料
				" AND b.IN_FLAG='1' ";
		}

		sqlstr += " AND	b.stock_place_no		    =	@v_stock_place_no ";
#pragma region 查询条件
		if (v_plan_type.Trim() == "L")
		{
			if (twm41["MAT_THICK"].ToDecimal() > 0)
			{
				sqlstr += " AND	b.MAT_THICK		=	@MAT_THICK ";
			}
			if (twm41["MAT_WIDTH"].ToDecimal() > 0)
			{
				sqlstr += " AND	b.MAT_WIDTH   =	@MAT_WIDTH ";
			}
			if (twm41["MAT_LEN"].ToDecimal() > 0)
			{
				sqlstr += " AND	b.MAT_LEN   =	@MAT_LEN ";
			}
			if (twm41["PSC"].ToString().Trim() != "")
			{
				sqlstr += " AND	b.PSC   =	@PSC ";
			}
			if (twm41["MSC"].ToString().Trim() != "")
			{
				sqlstr += " AND	b.MSC   =	@MSC ";
			}
			if (twm41["SG_STD"].ToString().Trim() != "")
			{
				sqlstr += " AND	b.SG_STD   =	@SG_STD ";
			}
			if (twm41["FIX_FLAG"].ToString().Trim() != "" && twm41["MAT_KIND"].ToString() == "BW")
			{
				if (twm41["FACTORY_DIV"].ToString().Trim() != "BW1")
				{
					sqlstr += " AND	b.FIX_FLAG		=	@FIX_FLAG ";
					Log::Trace("", "", "FIX_FLAG=+++++++++++++[{0}]", twm41["FIX_FLAG"].ToString().Trim());
					if (twm41["FIX_FLAG"].ToString().Trim() == "1")//定尺需要卡材料
					{
						sqlstr += " AND	b.MAT_LEN   =	@MAT_LEN ";
					}
				}
			}
			if (twm41["MAT_KIND"].ToString() == "SM")
			{
				if (twm41["MAT_LEN"].ToDecimal() > 0)
				{
					sqlstr += " AND	b.MAT_LEN   =	@MAT_LEN ";
				}
			}
			if (twm41["CROSS_CODE"].ToString().Trim() != "" && twm41["MAT_KIND"].ToString() == "BW")
			{
				sqlstr += " AND	b.CROSS_CODE		=	@CROSS_CODE ";
			}
		}
#pragma endregion
		sqlstr += "ORDER BY b.STOCK_NO ASC,b.STOCK_PLACE_NO ASC,b.LAYERNO DESC,b.ROWNO DESC,b.COLUMN_NO DESC ";
		Log::Info("", "", "sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_stock_no", v_stock_no);
		cmd_inq.Parameters.Set("v_transfer_plan_no", v_transfer_plan_no);
		cmd_inq.Parameters.Set("v_stock_place_no", v_stock_place_no);
		cmd_inq.Parameters.Set("MAT_THICK", twm41["MAT_THICK"].ToDecimal());
		cmd_inq.Parameters.Set("MAT_WIDTH", twm41["MAT_WIDTH"].ToDecimal());
		cmd_inq.Parameters.Set("PSC", twm41["PSC"].ToString());
		cmd_inq.Parameters.Set("MSC", twm41["MSC"].ToString());
		cmd_inq.Parameters.Set("SG_SIGN", twm41["SG_SIGN"].ToString());
		cmd_inq.Parameters.Set("FIX_FLAG", twm41["FIX_FLAG"].ToString());
		cmd_inq.Parameters.Set("MAT_LEN", twm41["MAT_LEN"].ToString());
		cmd_inq.Parameters.Set("CROSS_CODE", twm41["CROSS_CODE"].ToString());
		cmd_inq.Parameters.Set("SG_STD", twm41["SG_STD"].ToString());
		cmd_inq.ExecuteReader();
		fetchRowCount = 0;
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		CFormattable arguments[] = { bcls_ret->Tables[0].Rows.get_Count() };// 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到[{0}]条记录。", arguments, 1);//格式化字符串
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
