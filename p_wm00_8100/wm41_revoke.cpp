/*=========================================================================
//程序名称:		wm41_revoke
//隶属子系统:	WM00
//产品名称:		吊销计划
//创建人员:
//创建时间:		2022-09-05
//修改人员:
//修改日期:
//=========================================================================*/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

int f_7000y1_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//按件吊销电文
int f_7000y2_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//按量吊销电文
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmbw99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
// service入口
BM2F_ENTERACE(wm41_revoke)
/* -EP_SYSTEM_HEAD_END */
int f_wm41_revoke(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CDecimal rowscount = 0;
	int countes = 0;
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
		if (bcls_rec->Tables.IndexOf("7000Y1") < 0)  //按量电文块
		{
			bcls_rec->Tables.Add("7000Y1");
			bcls_rec->Tables["7000Y1"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["7000Y1"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
			bcls_rec->Tables["7000Y1"].Columns.Add(DT_STRING, "REJE_REAS_CODE");
			bcls_rec->Tables["7000Y1"].Columns.Add(DT_STRING, "OPERATE_TIME");
			bcls_rec->Tables["7000Y1"].Columns.Add(DT_STRING, "OPERATOR");
			bcls_rec->Tables["7000Y1"].Columns.Add(DT_STRING, "REMARK");
		}
		if (bcls_rec->Tables.IndexOf("7000Y2") < 0)  //按件电文块
		{
			bcls_rec->Tables.Add("7000Y2");
			bcls_rec->Tables["7000Y2"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
			bcls_rec->Tables["7000Y2"].Columns.Add(DT_STRING, "REJE_REAS_CODE");
			bcls_rec->Tables["7000Y2"].Columns.Add(DT_STRING, "OPERATE_TIME");
			bcls_rec->Tables["7000Y2"].Columns.Add(DT_STRING, "OPERATOR");
			bcls_rec->Tables["7000Y2"].Columns.Add(DT_STRING, "REMARK");
		}
		EIClass bcls_rec_mm99;
		bcls_rec_mm99.Tables[0].set_TableName("MM0099");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "TRANSFER_PLAN_NO");
		bcls_rec_mm99.Tables["MM0099"].Columns.Add(DT_STRING, "TRANSFER_REJECT_CAUSE");


		transfer_plan_no = bcls_rec->Tables[0].Rows[0]["TRANSFER_PLAN_NO"].ToString().Trim();
		plan_type = bcls_rec->Tables[0].Rows[0]["PLAN_TYPE"].ToString().Trim();
		mat_kind = bcls_rec->Tables[0].Rows[0]["MAT_KIND"].ToString().Trim();
		twmb5["TRANSFER_PLAN_NO"] = transfer_plan_no;
		if (twmb5.QueryCount("TRANSFER_PLAN_NO") > 0)  //查询是否有已装车的材料
		{
			sprintf(s.msg, "吊销计划前请把已装车的材料卸车!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		else
		{
			if (plan_type.Trim() == "J")      //按件发送电文
			{
				for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
				{
					bcls_rec->Tables["7000Y1"].Rows.Clear();
					bcls_rec->Tables["7000Y1"].Rows.Add();
					bcls_rec->Tables["7000Y1"].Rows[0]["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
					bcls_rec->Tables["7000Y1"].Rows[0]["TRANSFER_PLAN_NO"] = transfer_plan_no;
					bcls_rec->Tables["7000Y1"].Rows[0]["REJE_REAS_CODE"] = bcls_rec->Tables[0].Rows[0]["REJE_REAS_CODE"];
					bcls_rec->Tables["7000Y1"].Rows[0]["OPERATE_TIME"] = datetime;
					bcls_rec->Tables["7000Y1"].Rows[0]["OPERATOR"] = s.userid;
					bcls_rec->Tables["7000Y1"].Rows[0]["REMARK"] = bcls_rec->Tables[0].Rows[0]["REMARK"];
					if (f_7000y1_snd(bcls_rec, bcls_ret, conn) != 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					bcls_rec_mm99.Tables["MM0099"].Rows.Clear();
					bcls_rec_mm99.Tables["MM0099"].Rows.Add();
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["EVENT_ID"] = "PM07";
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "WM00";
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["TRANSFER_PLAN_NO"] = transfer_plan_no;
					bcls_rec_mm99.Tables["MM0099"].Rows[0]["TRANSFER_REJECT_CAUSE"] = bcls_rec->Tables[0].Rows[0]["REJE_REAS_CODE"];
					if (mat_kind.Trim() == "BW")
					{
						doFlag = f_mmbw99(&bcls_rec_mm99, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else if (mat_kind.Trim() == "SM")
					{
						doFlag = f_mmsm99(&bcls_rec_mm99, bcls_ret, conn);
						if (doFlag < 0)
						{
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
					else
					{
						sprintf(s.msg, "物料类型为空,请检查计划!");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					Log::Trace("", "", "MAT_NO=[{0}]", bcls_rec->Tables[0].Rows[i]["MAT_NO"]);
					twm42["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
					Log::Trace("", "", "{0}", bcls_rec->Tables[0].Rows[i]["MAT_NO"]);
					twm42.Delete("MAT_NO");
					//rowscount = rowscount + 1;
					countes = countes + 1;
				}
				twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
				twm41.Query("TRANSFER_PLAN_NO");
				Log::Trace("", "", "countes={0}", countes);
				if (twm41["TOTAL_NUM"].ToDecimal() == (twm41["DELIVY_NUM"].ToDecimal() + countes))
				{
					twm41.Delete("TRANSFER_PLAN_NO");
				}
			}
			else if (plan_type.Trim() == "L")   //按量发送电文
			{
				Db::QueryTable("SELECT DELIVY_NUM,DELIVY_WT FROM TWM41 WHERE TRANSFER_PLAN_NO='" + transfer_plan_no + "' ", info);
				Log::Trace("", "", "出厂重量=[{0}],出厂个数=[{1}]", info.Rows[0]["DELIVY_NUM"].ToDecimal(), info.Rows[0]["DELIVY_WT"].ToDecimal());
				if (info.Rows[0]["DELIVY_NUM"].ToDecimal() > 0 || info.Rows[0]["DELIVY_WT"].ToDecimal() > 0)
				{
					sprintf(s.msg, "按量计划部分已经完成转库,不能吊销!");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				else
				{
					bcls_rec->Tables["7000Y2"].Rows.Clear();
					bcls_rec->Tables["7000Y2"].Rows.Add();
					bcls_rec->Tables["7000Y2"].Rows[0]["TRANSFER_PLAN_NO"] = transfer_plan_no;
					bcls_rec->Tables["7000Y2"].Rows[0]["REJE_REAS_CODE"] = bcls_rec->Tables[0].Rows[0]["REJE_REAS_CODE"];
					bcls_rec->Tables["7000Y2"].Rows[0]["OPERATE_TIME"] = datetime;
					bcls_rec->Tables["7000Y2"].Rows[0]["OPERATOR"] = s.userid;
					bcls_rec->Tables["7000Y2"].Rows[0]["REMARK"] = bcls_rec->Tables[0].Rows[0]["REMARK"];
					if (f_7000y2_snd(bcls_rec, bcls_ret, conn) != 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					twm41["TRANSFER_PLAN_NO"] = transfer_plan_no;
					twm41.Delete();
				}
				
			}
			else
			{
				sprintf(s.msg, "计划类型错误,请检查!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

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
