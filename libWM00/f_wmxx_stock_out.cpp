/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wmxx_stock_out
*  程序描述			: 仓库出库主函数
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 

#if defined _LINE_SM
BM2_FUNCTION_IMPORT
int f_wmsmsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //炼钢产线坯料
#endif

#if defined _LINE_CR
BM2_FUNCTION_IMPORT
int f_wmcrcr_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //冷轧产线冷卷
BM2_FUNCTION_IMPORT
int f_wmcrhr_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //冷轧产线热卷
#endif

#if defined _LINE_BW
BM2_FUNCTION_IMPORT
int f_wmbwbw_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //棒线
BM2_FUNCTION_IMPORT
int f_wmbwsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //棒线坯料
#endif

#if defined _LINE_SF
BM2_FUNCTION_IMPORT
int f_wmsfsf_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //锻造
BM2_FUNCTION_IMPORT
int f_wmsfsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //锻造坯料
#endif

#if defined _LINE_HR
BM2_FUNCTION_IMPORT
int f_wmhrhr_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //热轧产线热卷
BM2_FUNCTION_IMPORT
int f_wmhrsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //热轧产线板坯
#endif

#if defined _LINE_HP
BM2_FUNCTION_IMPORT
int f_wmhphp_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //厚板产线钢板
BM2_FUNCTION_IMPORT
int f_wmhpsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //厚板产线板坯
BM2_FUNCTION_IMPORT
#endif
int f_wmbwsm_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //棒线产线钢坯 （暂无）
BM2_FUNCTION_IMPORT
int f_wmbwbw_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //棒线产线棒线材 （暂无）


BM2_FUNCTION_EXPORT
int f_wmxx_stock_out(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_mat_line_type = "";
	CString v_mat_kind = "";
	CString v_mat_no = "";

	/* ***** 数据库SQL操作字符串 ***** */

	/* ***** 数据库操作类定义 ***** */

	/* ***** 定义表实体对象 ***** */



	/* ***** 应用程序开始处理 ***** */
	try
	{
		if (!bcls_rec->Tables.Contains("WM_STOCK"))
		{
			sprintf(s.msg, "函数f_wm00_stock_in中找不到接收块名[WM_STOCK]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		v_mat_kind = bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_KIND"].ToString();
		v_mat_line_type = bcls_rec->Tables["WM_STOCK"].Rows[0]["MAT_LINE_TYPE"].ToString();

		if (v_mat_line_type.Trim() == "SM" &&
			v_mat_kind.Trim() == "SM")
		{

#if defined _LINE_SM
			doFlag = f_wmsmsm_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "HR" &&
			v_mat_kind.Trim() == "SM")
		{
#if defined _LINE_HR
			//doFlag = f_wmhrsm_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "HR" &&
			v_mat_kind.Trim() == "HR")
		{
#if defined _LINE_HR
		    doFlag = f_wmhrhr_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "CR" &&
			v_mat_kind.Trim() == "HR")
		{
#if defined _LINE_CR
			doFlag = f_wmcrhr_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "CR" &&
			v_mat_kind.Trim() == "CR")
		{
#if defined _LINE_SM
			doFlag = f_wmcrcr_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "HP" &&
			v_mat_kind.Trim() == "SM")
		{
#if defined _LINE_HP
			doFlag = f_wmhpsm_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "HP" &&
			v_mat_kind.Trim() == "HP")
		{
#if defined _LINE_HP
			doFlag = f_wmhphp_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "BW" &&
			v_mat_kind.Trim() == "BW")
		{
#if defined _LINE_BW
			doFlag = f_wmbwbw_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "BW" &&
			v_mat_kind.Trim() == "SM")
		{
#if defined _LINE_BW
			doFlag = f_wmbwsm_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "SF" &&
			v_mat_kind.Trim() == "SF")
		{
#if defined _LINE_SF
			doFlag = f_wmsfsf_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else if (v_mat_line_type.Trim() == "SF" &&
			v_mat_kind.Trim() == "SM")
		{
#if defined _LINE_SF
			doFlag = f_wmsfsm_stock_out(bcls_rec, bcls_ret, conn);
#endif
		}
		else
		{

		}

		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n";

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
