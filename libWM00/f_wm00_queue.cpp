/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      吴新
Version:     1.0
Date:        2016-03-21
Description: 倒垛队列函数
**************************************************/

#include "stdafx.h"


//调用外部函数

BM2_FUNCTION_EXPORT
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int retCnt = 0;
	CString sqlstr = "";
	CString s_message = " ";

	//定义表实体对象
	CModel hwma0("HWMA0");
	CModel twma0("TWMA0");
	CModel twma0_old("TWMA0");
	CModel twm01("TWM01");
	CModel twm000e("TWM000E");


	CDbCommand cmd_inq(conn);

	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{

		//传入参数检核
		if (!bcls_rec->Tables.Contains("WM00QUE"))
		//if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "传入块名错误!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		for (int i = 0; i < bcls_rec->Tables["WM00QUE"].Rows.get_Count(); i++)
		{
			twma0.MergeFrom(bcls_rec->Tables["WM00QUE"].Rows[i]);
			twma0.TrimOrBlank();

			Log::Debug("", __FUNCTION__, "twma0.MAT_NO				= [{0}]", twma0["MAT_NO"].ToString());
			Log::Debug("", __FUNCTION__, "twma0.STOCK_NO			= [{0}]", twma0["STOCK_NO"].ToString());
			Log::Debug("", __FUNCTION__, "twma0.UNIT_CODE			= [{0}]", twma0["UNIT_CODE"].ToString());
			Log::Debug("", __FUNCTION__, "twma0.STOCK_OPER_ORDER	= [{0}]", twma0["STOCK_OPER_ORDER"].ToString());
			Log::Debug("", __FUNCTION__, "twma0.OPER_FLAG			= [{0}]", twma0["OPER_FLAG"].ToString());

			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() == "")
			{
				sprintf(s.msg, _RES("YM00C0000434")/*仓库业务类型不允许为空*/);
				sprintf(s.msg, "仓库业务类型不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma0["OPER_FLAG"].ToString().Trim() == "")
			{
				sprintf(s.msg, _RES("YM00C0000434")/*仓库业务类型不允许为空*/);
				sprintf(s.msg, "仓库OPER_FLAG不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Debug("", __FUNCTION__, "111			");



			////判断是否为删除操作
			if (twma0["OPER_FLAG"].ToString().Trim() == "D")
			{

				twma0.Delete("MAT_NO, STOCK_OPER_ORDER");

				//写倒垛履历表//////////////////////////////////////
				hwma0.CopyFrom(twma0);
				hwma0["EVENT_TIME"] = datetime;
				//hwma0["CLIENT_IP"] = s.fore_ip;
				hwma0["FUNC_ID"] = s.svc_name;
				hwma0["REMARK"] = "接口调用删除";
				hwma0.Insert();

				continue;
			}
			Log::Debug("", __FUNCTION__, "33			");
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			twma0_old["MAT_NO"] = twma0["MAT_NO"];



			sqlstr =
				" SELECT * FROM TWMA0"
				" WHERE MAT_NO = @mat_no"
				" AND STOCK_OPER_ORDER LIKE @stock_oper_order";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
			cmd_inq.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString().Substring(0, 1) + "%");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(twma0_old);

				//写倒垛履历表//////////////////////////////////////
				hwma0.CopyFrom(twma0_old);
				hwma0["EVENT_TIME"] = datetime;
				//hwma0["CLIENT_IP"] = s.fore_ip;
				hwma0["FUNC_ID"] = s.svc_name;
				hwma0["REMARK"] = "接口调用删除";
				hwma0.Insert();
			}
			cmd_inq.Close();


			sqlstr =
				" DELETE FROM TWMA0"
				" WHERE MAT_NO = @mat_no"
				" AND STOCK_OPER_ORDER LIKE @stock_oper_order";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
			cmd_inq.Parameters.Set("stock_oper_order", twma0["STOCK_OPER_ORDER"].ToString().Substring(0, 1) + "%");
			cmd_inq.ExecuteNonQuery();



			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() == "")
			{
				sprintf(s.msg, _RES("YM00C0000434")/*仓库业务类型不允许为空*/);
				sprintf(s.msg, "仓库业务类型不允许为空！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twma0["STOCK_NO"].ToString().Trim() == " " &&
				twma0["UNIT_CODE"].ToString().Trim() == " ")
			{
				sprintf(s.msg, _RES("YM00C0000435")/*库区号 与 机组号 两者其一必须有值！*/);
				sprintf(s.msg, "11111库区号 与 机组号 两者其一必须有值！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			Log::Debug("", __FUNCTION__, "44			");


			if (twma0["STOCK_NO"].ToString() == " " &&
				twma0["STOCK_OPER_ORDER"].ToString()[0] == '2')
			{
				sprintf(s.msg, _RES("YM00C0000435")/*库区号 与 机组号 两者其一必须有值！*/);
				sprintf(s.msg, "出库时 库区号  必须有值！");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			//如果库业务类型是2B，则删除机组对应的入库队列后，再新增
			if (twma0["STOCK_OPER_ORDER"].ToString().Trim() == "2B")
			{
				sqlstr =
					" DELETE FROM TWMA0"
					" WHERE MAT_NO = @mat_no";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("mat_no", twma0["MAT_NO"].ToString());
				cmd_inq.Parameters.Set("unit_code", twma0["UNIT_CODE"].ToString());
			}

			if (twma0["UNIT_CODE"].ToString().Trim() != "" &&
				twma0["STOCK_NO"].ToString().Trim() == "")
			{
				Log::Debug("", __FUNCTION__, "twma0.UNIT_CODE		= [{0}]", twma0["UNIT_CODE"].ToString());
				Log::Debug("", __FUNCTION__, "twma0.NEXT_UNIT_CODE	= [{0}]", twma0["NEXT_UNIT_CODE"].ToString());

				sqlstr =
					" SELECT * FROM TWM000E "
					" WHERE UNIT_CODE = @unit_code"
					" AND EVENT_ID = @event_id";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("unit_code", twma0["UNIT_CODE"].ToString());
				cmd_inq.Parameters.Set("next_unit_code", twma0["NEXT_UNIT_CODE"].ToString());
				cmd_inq.Parameters.Set("event_id", twma0["STOCK_OPER_ORDER"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(twm000e);
				}
				else
				{
					//Log::Trace("", __FUNCTION__, "TWM000E未配置机组[{0}]的出口库区！", twma0["UNIT_CODE"].ToString());
					//continue;
					sprintf(s.msg, "TWM000E未配置机组[%s]的出口库区！", (const char*)twma0["UNIT_CODE"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd_inq.Close();

				twma0["STOCK_NO"] = twm000e["STOCK_NO"];
			}

			Log::Debug("", __FUNCTION__, "155			");

			if (twma0["STOCK_NO"].ToString() == " " &&
				twma0["STOCK_OPER_ORDER"].ToString()[0] == '1')
			{
				sprintf(s.msg, _RES("YM00C0000435")/*库区号 与 机组号 两者其一必须有值！*/);
				sprintf(s.msg, "库区号  必须有值！");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//sqlstr =
			//	" SELECT MAT_KIND, MAT_LINE_TYPE"
			//	" FROM TWM01"
			//	" WHERE STOCK_NO = @stock_no";
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("stock_no", twma0["STOCK_NO"].ToString());
			//cmd_inq.ExecuteReader();
			//if (cmd_inq.Read())
			//{
			//	twma0["MAT_KIND"] = cmd_inq.GetString(1);
			//	twma0["MAT_LINE_TYPE"] = cmd_inq.GetString(2);
			//}
			//cmd_inq.Close();


			twm01["STOCK_NO"] = twma0["STOCK_NO"];
			if (!twm01.Query("STOCK_NO"))
			{
				Log::Trace("", __FUNCTION__, "库区找不到。");
				continue;
			}

			if (twm01["STOCK_TYPE_CODE"].ToString().Trim() == "9")
			{
				Log::Trace("", __FUNCTION__, "外部库区不生成队列。");
				continue;
			}

			twma0["MAT_KIND"] = twm01["MAT_KIND"];
			twma0["MAT_LINE_TYPE"] = twm01["MAT_LINE_TYPE"];

			twma0["REC_CREATOR"] = s.userid;
			twma0["REC_CREATE_TIME"] = datetime;
			twma0["REC_REVISOR"] = s.userid;
			twma0["REC_REVISE_TIME"] = datetime;
			twma0["PROC_STATUS"] = "0";
			twma0.Insert();
			Log::Debug("", __FUNCTION__, "166			");
			//写倒垛履历表//////////////////////////////////////
			hwma0.CopyFrom(twma0);
			hwma0["EVENT_TIME"] = datetime;
			//hwma0["CLIENT_IP"] = s.fore_ip;
			hwma0["FUNC_ID"] = s.svc_name;
			//hwma0["FUNC_ID"] = s.formname;
			hwma0["REMARK"] = "接口调用新增";
			hwma0.Insert();
			Log::Debug("", __FUNCTION__, "77			");
		}




	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
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

	return(doFlag);
}

