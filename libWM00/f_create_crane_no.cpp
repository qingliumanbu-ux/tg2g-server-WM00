/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 生成吊车命令
**************************************************/

#include "stdafx.h"
//行车命令表



BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */


int f_create_crane_no(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString crane_no("");
	CString move_type("");
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	CDecimal rowCount = 0;
	//定义实体类
	CModel twma7("TWMA7");//行车命令表
	CModel tmmsm01("TMMSM01");//物料主表
	CModel twm04_Y("TWM04");//源垛位
	CModel twm04_T("TWM04");//目标垛位
	CModel twm06("TWM06");//吊车
	CDbCommand cmd_inq(conn);
	try
	{
		rowCount = bcls_rec->Tables[0].Rows.get_Count();
		Log::Trace("", __FUNCTION__, "rowCount [{0}]", rowCount);

		///*crane_no = bcls_rec->Tables[1].Rows[0]["CRANE_NO"].ToString().Trim();
		//move_type = bcls_rec->Tables[1].Rows[0]["MOVE_TYPE"].ToString().Trim();
		//Log::Trace("", __FUNCTION__, "crane_no = [{0}]", crane_no);
		//Log::Trace("", __FUNCTION__, "move_type = [{0}]", move_type);*/

		for (int i = 0; i < rowCount; i++)
		{
			twma7.Reset();
			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			/*twma7["CRANE_NO"] = crane_no;
			twma7["MOVE_TYPE"] = move_type;*/
			Log::Trace("", __FUNCTION__, "bcls_rec->Tables[0].Rows[i]", bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString());
			
			if (twma7["MAT_NO"].ToString().Trim() == ""){
				s.flag = -1;
				strcpy(s.msg, "板坯材料号不能位空！");
				return -1;
			}
			tmmsm01.Reset();
			tmmsm01["MAT_NO"] = twma7["MAT_NO"];
			if (!tmmsm01.Query("MAT_NO")){
				sprintf(s.msg, "板坯" + twma7["MAT_NO"].ToString() + "查询失败." );
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twm06["CRANE_NO"] = twma7["CRANE_NO"];
			Log::Trace("", __FUNCTION__, "twma7[CRANE_NO] = [{0}]", twma7["CRANE_NO"].ToString());
			if (!twm06.Query("CRANE_NO")){
				sprintf(s.msg, "吊车号" + twm06["CRANE_NO"].ToString() + "查询失败.");

				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if (tmmsm01["STOCK_PLACE_POSITION"].ToString() != "2"){
			//	sprintf(s.msg, "板坯" + twma7["MAT_NO"].ToString() + "位置不是2.");
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			if (tmmsm01["CMD_FLAG"].ToString() == "1"){
				sprintf(s.msg, "板坯" + twma7["MAT_NO"].ToString() + "命令重复.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["STOCK_PLACE_NO"].ToString() != twma7["STOCK_PLACE_NO_FROM"].ToString())
			{
				sprintf(s.msg, "画面没有刷新，请重新刷新垛位信息！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//
			if (twma7["STOCK_PLACE_NO_TO"].ToString() == "705") //直接送热轧
			{
				if (tmmsm01["SURFACE_DECIDE_CODE"].ToString() != "1")//表面判定代码
				{
					sprintf(s.msg, "板坯" + twma7["MAT_NO"].ToString() + "表面判定不合格，不满足直接装车要求，请入库操作.", twma7["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			if (twma7["STOCK_PLACE_NO_TO"].ToString() == "704")
			{
				if (strcmp(tmmsm01["MAT_STATUS"].ToString(), "07") == 0 && strcmp(tmmsm01["SURFACE_DECIDE_CODE"].ToString(), "1") == 0)
				{

				}
				else
				{
					sprintf(s.msg, "板坯" + twma7["MAT_NO"].ToString() + "非计划板坯，请重新确认.", twma7["MAT_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			if (twma7["STOCK_PLACE_NO_TO"].ToString() == "703")
			{
				if (strcmp(tmmsm01["SURFACE_DECIDE_CODE"].ToString(), "1") == 0 && strcmp(tmmsm01["MAT_STATUS"].ToString(), "07") != 0)
				{

				}
				else
				{
					sprintf(s.msg, "板坯[%s]不能发2热轧板坯，请重新确认.", twma7["MAT_NO"].ToString());
					doFlag = -1;
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			//
			if (twma7["STOCK_PLACE_NO_FROM"].ToString().Trim() != "")
			{
				twm04_Y.Reset();
				twm04_Y["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_FROM"];
				if (!twm04_Y.Query("STOCK_PLACE_NO")){
					sprintf(s.msg, "源垛位没有在垛位信息中维护！");
					doFlag = -1;
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}
			twm04_T.Reset();
			twm04_T["STOCK_PLACE_NO"] = twma7["STOCK_PLACE_NO_TO"];
			Log::Trace("", __FUNCTION__, "twm04_T[STOCK_PLACE_NO] = [{0}]", twm04_T["STOCK_PLACE_NO"].ToString());
			if (!twm04_T.Query("STOCK_PLACE_NO")){
				sprintf(s.msg, "目标垛位没有在垛位信息中维护！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}
			sqlstr = "SELECT timestampdiff(8, CHAR(TIMESTAMP(to_date('" + dateNow14 + "', 'yyyy-mm-dd hh24:mi:ss')) - TIMESTAMP(to_date('" + tmmsm01["SLAB_CUT_TIME"].ToString() + "', 'yyyy-mm-dd hh24:mi:ss')))) AS diffTimes16"
				" FROM sysibm.sysdummy1";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.ExecuteReader();
			if (!cmd_inq.Read()){
				s.flag = -1;
				strcpy(s.msg, "获取失败！");
				return -1;
			}
			CDecimal v_max_cold_time = cmd_inq.GetDecimal(1);//时间差
			if (twm04_T["COLD_HOT_REQ"].ToDecimal() !=999)
			{
				if (twm04_T["COLD_HOT_REQ"].ToDecimal() > v_max_cold_time)
				{
					sprintf(s.msg,  "板坯冷却小时数小于跺位要求！");
					doFlag = -1;
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			if (twm04_T["MAX_WIDTH"].ToDecimal() < tmmsm01["MAT_ACT_WIDTH"].ToDecimal())
			{
				sprintf(s.msg,  "板坯宽度大于限制宽度！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm04_T["MAX_LEN"].ToDecimal() < tmmsm01["MAT_ACT_LEN"].ToDecimal())
			{
				sprintf(s.msg,  "板坯长度大于限制长度！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm04_T["MAX_HEIGHT"].ToDecimal() < (twm04_T["PILE_MAT_NUM_ACT"].ToDecimal() + twm04_T["PRE_MAT_NUM"].ToDecimal()))/*2016-10-22：增加预约板坯数*/
			{
				sprintf(s.msg, "板坯块数大于限制块数！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (strcmp(twm04_T["STOCK_STATUS"].ToString(), "1") == 0)
			{
				sprintf(s.msg,  "目标垛位有行车命令没有确认，不能往里面倒板坯！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (twm04_T["PILE_MAT_NUM_ACT"].ToDecimal() == 0)
			{
				EDLog(1, 1, "垛位无板坯，不需要校验");
			}
			else if (0 == strcmp(twm04_T["HALL_NO"].ToString(), "S02") && 0 == strcmp(twm04_T["STOCK_PLACE_TYPE"].ToString(), "1"))//只对下线热堆场的普通垛位进行判断
			{
				sqlstr = "SELECT    NVL(MIN(MAT_ACT_WIDTH), 0) AS MAT_ACT_WIDTH_MIN,"
					" NVL(MAX(MAT_ACT_WIDTH), 0) AS MAT_ACT_WIDTH_MAX,NVL(MIN(MAT_ACT_LEN), 0) AS MAT_ACT_LEN_MIN,"
					" NVL(MAX(MAT_ACT_LEN), 0) AS MAT_ACT_LEN_MAX FROM   TMMSM01 "
					" WHERE   STOCK_PLACE_NO = @STOCK_PLACE_NO AND STOCK_PLACE_POSITION = '2'AND CMD_FLAG <> '1'";
				cmd_inq.Close();
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("STOCK_PLACE_NO", twma7["STOCK_PLACE_NO_TO"].ToString());
				cmd_inq.ExecuteReader();
				cmd_inq.Read();
				CDecimal w_min = cmd_inq.GetDecimal(1);
				CDecimal w_max = cmd_inq.GetDecimal(2);
				CDecimal l_min = cmd_inq.GetDecimal(3);
				CDecimal l_max = cmd_inq.GetDecimal(4);
				
				//判断垛位宽度是否满足
				if ((w_min >= 1200 && tmmsm01["MAT_ACT_WIDTH"].ToDecimal() >= 1200) || ((tmmsm01["MAT_ACT_WIDTH"].ToDecimal() - w_min).Abs() <= twm04_T["WID_DIFF_MAX"].ToDecimal() && (tmmsm01["MAT_ACT_WIDTH"].ToDecimal() - w_max).Abs() <= twm04_T["WID_DIFF_MAX"].ToDecimal()))
				{
					EDLog(1, 1, "宽度与垛位中板坯判断满足要求");
				}
				else
				{
					sprintf(s.msg, "%s", "板坯宽度不能满足垛位限制要求！");
					doFlag = -1;
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//判断垛位长度是否满足
				if ((tmmsm01["MAT_ACT_LEN"].ToDecimal() - l_min).Abs() <= twm04_T["LEN_DIFF_MAX"].ToDecimal() && (tmmsm01["MAT_ACT_LEN"].ToDecimal() - l_max).Abs() <= twm04_T["LEN_DIFF_MAX"].ToDecimal())
				{
					EDLog(1, 1, "长度与垛位中板坯判断满足要求");
				}
				else
				{
					sprintf(s.msg, "%s", "板坯长度不能满足垛位限制要求！");
					doFlag = -1;
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}

			//行车命令号生成 
			sqlstr = "select CODE_DESC_1_CONTENT  from tep0002 where CODE_CLASS = 'WMAC' and code =@code";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", twma7["CRANE_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (!cmd_inq.Read()){
				sprintf(s.msg, "%s", "行车命令号创建失败，小代码查询空WMAC！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma7["CRANE_INST_CODE"] = cmd_inq.GetString(1) + EPGetNextSeq("CRANE_CMD_NO", conn);

			//生成板坯命令
			twma7["CRANE_INST_STATUS"] = "0";
			twma7["STOCK_NO_FROM"] = twm04_Y["STOCK_NO"];
			twma7["STOCK_NO_TO"] = twm04_T["STOCK_NO"];
			twma7["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"].ToDecimal() / 10;
			twma7["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
			//twma7["MOVE_TYPE"] = "";
			twma7["REC_CREATOR"] = s.userid;
			twma7["REC_CREATE_TIME"] = dateNow14;
			twma7["MAT_ACT_WIDTH"] = tmmsm01["MAT_ACT_WIDTH"];
			twma7["MAT_ACT_LEN"] = tmmsm01["MAT_ACT_LEN"];
			twma7["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
			twma7["MAT_DESTION"] = tmmsm01["MAT_DESTION"];
			twma7["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"];
			twma7["HEAD_TAIL_WITH_DIFF"] = (tmmsm01["SLAB_HEAD_WIDTH"].ToDecimal() - tmmsm01["SLAB_TAIL_WIDTH"].ToDecimal()).Abs();
			twma7["CLIENT_IP"] = twm06["MAC"];
			twma7.Insert();

			//zhenglei：增加tymsm01表垛位预约数的维护，查询目前未确认的板坯命令数（目标垛位），然后更新tymsm01表
			twm04_T["PRE_MAT_NUM"] = twm04_T["PRE_MAT_NUM"].ToDecimal() + 1;
			twm04_T.Update("PRE_MAT_NUM","STOCK_PLACE_NO");
			twm04_Y["STOCK_STATUS"] = "1";
			twm04_Y.Update("STOCK_STATUS", "STOCK_PLACE_NO");
			tmmsm01["CMD_FLAG"] = "1";
			tmmsm01.Update("CMD_FLAG","MAT_NO");
			
			//EXEC SQL update tymsm01 set pre_mat_num = pre_mat_num + 1 where stock_place_no = :tymsm10.targ_pos;
			//EXEC SQL update tymsm01 set stock_status = '1' where stock_place_no = :tymsm10.source_pos;
			//EXEC SQL update tmmsm01 set cmd_flag = '1' where mat_position = '2'	and mat_no = :tymsm10.mat_no;//行车命令标志，1：命令中，0：无命令
			
			////入库调用入库函数
			//if (twma7["MOVE_TYPE"].ToString() == "I"){//入库
			//
			//}
			//else if(twma7["MOVE_TYPE"].ToString() == "O"){//出库

			//}
			//else  if (twma7["MOVE_TYPE"].ToString() == "D"){//出库

			//}
			//else{
			//	sprintf(s.msg, "%s", "行车移动类型出错！");
			//	doFlag = -1;
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


