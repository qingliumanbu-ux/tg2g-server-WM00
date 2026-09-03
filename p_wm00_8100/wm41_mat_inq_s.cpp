/*=========================================================================
//程序名称:		wm_mat_inq_r
//隶属子系统:	WM00
//产品名称:		点击合同号时查询符合规格材料的库区
//创建人员:		KE2017
//创建时间:		2022-09-27
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"

// service入口
BM2F_ENTERACE(wm41_mat_inq_s)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_mat_inq_s(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	/*程序用变量*/
	int blkNum = 0;
	CString	blkName = "wm41_mat_inq_s";
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
	CString	v_aim_stock_no = "";
	CString	v_delivy_plan_status = "";
	CString	v_mat_kind = "";
	CString	v_stock_no = "";
	CString	v_stock_place_no = "", v_psc = "";
	CString batch_no_name = "";//批次号或炉号字段名称
	CString v_plan_type = "";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");;
	date = datetime.Substring(0, 8);
	time = datetime.Substring(8, 6);

	CModel twm42("TWM42");
	CModel twm41("TWM41");

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CString sqlstr, sqlstrwhere, sqlstrgroup;
	try
	{
		/********************************
		*	读取传入的参数				*
		********************************/
		if (bcls_rec->Tables[0].Columns.Contains("mat_kind"))			v_mat_kind = bcls_rec->Tables[0].Rows[0]["mat_kind"];	//合同号
		if (bcls_rec->Tables[0].Columns.Contains("transfer_plan_no"))	v_transfer_plan_no = bcls_rec->Tables[0].Rows[0]["transfer_plan_no"];	//计划号
		if (bcls_rec->Tables[0].Columns.Contains("aim_stock_no"))		v_aim_stock_no = bcls_rec->Tables[0].Rows[0]["aim_stock_no"];	//仓库代码
		if (bcls_rec->Tables[0].Columns.Contains("stock_no"))			v_stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"];	//仓库代码
		if (bcls_rec->Tables[0].Columns.Contains("factory_div"))		factory_div = bcls_rec->Tables[0].Rows[0]["factory_div"];	//厂别
		if (bcls_rec->Tables[0].Columns.Contains("plan_type"))		v_plan_type = bcls_rec->Tables[0].Rows[0]["plan_type"];	//厂别


		Log::Info("", __FUNCTION__, "MAT_KIND=[{0}]", v_mat_kind);
		Log::Info("", __FUNCTION__, "TRANSFER_PLAN_NO=[{0}]", v_transfer_plan_no);
		Log::Info("", __FUNCTION__, "STOCK_NO=[{0}]", v_aim_stock_no);
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

		batch_no_name = " b.STOCK_PLACE_NO ";


		if (v_plan_type.Trim() == "J")
		{
			sqlstr = "SELECT DISTINCT " + batch_no_name +
				" FROM	TWM42 a , TMM" + twm41["MAT_KIND"].ToString() + "01 b WHERE 1=1"
				" AND a.TRANSFER_PLAN_NO= @v_transfer_plan_no "
				" AND	b.STOCK_NO		=	@v_stock_no "
				" AND	a.MAT_NO		=	b.MAT_NO "
				" AND	NOT EXISTS (SELECT 1 FROM TWMB5 WHERE MAT_NO=B.MAT_NO)";
		}
		else
		{
			sqlstr = "SELECT DISTINCT " + batch_no_name +
				" FROM	 TMM" + twm41["MAT_KIND"].ToString() + "01 b WHERE 1=1"
				" AND	b.TRANSFER_PLAN_NO = ' ' "
				//" AND	a.DELIVY_QTY_FLAG =	'1' "
				" AND	b.STOCK_NO		=	@v_stock_no "
				" AND	b.SG_SIGN		=	@SG_SIGN "
				" AND	NOT EXISTS (SELECT 1 FROM TWMB5 WHERE MAT_NO=B.MAT_NO) "//2020应该排除已装车材料
				" AND IN_FLAG='1' "
				" AND b.STOCK_PLACE_NO!=' ' ";
		}

#pragma region 后续查询条件
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
#pragma region 特钢时用的代码
		//else
		//{
		//	sqlstr += "AND ((b.REC_CREATOR != 'QC' AND b.MAT_LEN = @ORDER_LEN) OR b.REC_CREATOR = 'QC') ";
		//}
		//2020朱敏的意思是只卡PSC码
		//if (tsm0003["FIX_FLAG"].ToString() != "")
		//{
		//	sqlstr +=
		//		" AND	b.FIX_FLAG		=	@FIX_FLAG ";
		//	cmd_inq.Parameters.Set("FIX_FLAG", tsm0003["FIX_FLAG"].ToString());
		//	if (tsm0003["FIX_FLAG"].ToString() == "1")//定尺需要卡材料
		//	{
		//		sqlstr +=
		//			" AND	b.MAT_LEN   =	@ORDER_LEN ";
		//		cmd_inq.Parameters.Set("ORDER_LEN", tsm0003["ORDER_LEN"].ToDecimal());
		//	}
		//}
		//if (tsm0003["TRNP_MODE_CODE"].ToString() == "12" || tsm0003["TRNP_MODE_CODE"].ToString() == "22")//特钢铁运发货必须为已计量过的材料
		//{
		//	if ((factory_div == "XT1" || factory_div == "XT3") && (tsm0003["WT_MODE"].ToString() == "0" || tsm0003["WT_MODE"].ToString() == ""))
		//	{
		//		sqlstr += " AND	b.PONDER_NO  !=	'' ";
		//	}
		//}
#pragma endregion

		sqlstr += "ORDER BY b.STOCK_PLACE_NO ";

		Log::Info("", __FUNCTION__, "sqlstr=[{0}],STOCK_NO=[{1}],MAT_THICK=[{2}],MAT_LEN=[{3}],PSC=[{4}],SG_SIGN=[{5}],MAT_WIDTH=[{6}],MSC=[{7}]", sqlstr, v_stock_no, twm41["MAT_THICK"].ToDecimal(), twm41["MAT_LEN"].ToDecimal(), twm41["PSC"].ToString(), twm41["SG_SIGN"].ToString(), twm41["MAT_WIDTH"].ToDecimal(), twm41["MSC"].ToString());
		Log::Info("", "", "sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("MAT_THICK", twm41["MAT_THICK"].ToDecimal());
		cmd_inq.Parameters.Set("MAT_WIDTH", twm41["MAT_WIDTH"].ToDecimal());
		cmd_inq.Parameters.Set("PSC", twm41["PSC"].ToString());
		cmd_inq.Parameters.Set("MSC", twm41["MSC"].ToString());
		cmd_inq.Parameters.Set("v_stock_no", v_stock_no);
		cmd_inq.Parameters.Set("v_transfer_plan_no", v_transfer_plan_no);
		cmd_inq.Parameters.Set("SG_SIGN", twm41["SG_SIGN"].ToString());
		//cmd_inq.Parameters.Set("PSC", twm41["PSC"].ToString().Substring(0, twm41["PSC"].ToString().GetLength() - 1));
		cmd_inq.Parameters.Set("FIX_FLAG", twm41["FIX_FLAG"].ToString());
		cmd_inq.Parameters.Set("MAT_LEN", twm41["MAT_LEN"].ToString());
		cmd_inq.Parameters.Set("CROSS_CODE", twm41["CROSS_CODE"].ToString());
		cmd_inq.Parameters.Set("SG_STD", twm41["SG_STD"].ToString());
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		Log::Info("", __FUNCTION__, "count=[{0}]", bcls_ret->Tables[0].Rows.get_Count());

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
