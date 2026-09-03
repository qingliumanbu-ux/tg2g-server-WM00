/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Descrtion: 生成吊车命令
**************************************************/

#include "stdafx.h"
#include "twma7.h"//行车命令表
#include "tmmsm01.h"
#include "twm04.h"
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */


int f_wm00_pile_comd_prod(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类
	CTWMA7 twma7(conn);//行车命令表
	CTMMSM01 tmmsm01(conn);//物料主表
	CTWM04 twm04_Y(conn);//源垛位
	CTWM04 twm04_T(conn);//目标垛位
	
	CDbCommand cmd_inq(conn);
	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma7.Reset();
			twma7.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (twma7.MAT_NO.Trim() == ""){
				s.flag = -1;
				strcpy(s.msg, "板坯材料号不能位空！");
				return -1;
			}
			tmmsm01.Reset();
			tmmsm01.MAT_NO = twma7.MAT_NO;
			if (!tmmsm01.Query("MAT_NO")){
				sprintf(s.msg, "板坯" + twma7.MAT_NO + "查询失败." );
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tmmsm01.CMD_FLAG == "1"){
				break;
			}
			Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_FROM = [{0}]", twma7.STOCK_PLACE_NO_FROM);
			//
			if (twma7.STOCK_PLACE_NO_FROM.Trim() != "")
			{
				twm04_Y.Reset();
				twm04_Y.STOCK_PLACE_NO = twma7.STOCK_PLACE_NO_FROM;
				if (!twm04_Y.Query("STOCK_PLACE_NO")){
					sprintf(s.msg, "源垛位没有在垛位信息中维护！");
					doFlag = -1;
					throw CApplicationException(-1, s.msg, log.Location);
				}


			}
			Log::Trace("", __FUNCTION__, "STOCK_PLACE_NO_TO = [{0}]", twma7.STOCK_PLACE_NO_TO);
			twm04_T.Reset();
			twm04_T.STOCK_PLACE_NO = twma7.STOCK_PLACE_NO_TO;
			if (!twm04_T.Query("STOCK_PLACE_NO")){
				sprintf(s.msg, "目标垛位没有在垛位信息中维护！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//行车命令号生成 
			sqlstr = "select CODE_DESC_1_CONTENT  from tep0002 where CODE_CLASS = 'WMAC' and code =@code";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("code", twma7.CRANE_NO);
			cmd_inq.ExecuteReader();
			if (!cmd_inq.Read()){
				sprintf(s.msg, "%s", "行车命令号创建失败，小代码查询空WMAC！");
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}
			twma7.CRANE_INST_CODE = cmd_inq.GetString(1) + EPGetNextSeq("CRANE_CMD_NO", conn);

			//生成板坯命令
			twma7.CRANE_INST_STATUS = "0";
			twma7.STOCK_NO_FROM = twm04_Y.STOCK_NO;
			twma7.STOCK_NO_TO = twm04_T.STOCK_NO;
			twma7.MAT_ACT_WIDTH = tmmsm01.MAT_ACT_WIDTH ;
			twma7.MAT_ACT_LEN = tmmsm01.MAT_ACT_LEN;
			twma7.MOVE_TYPE = "I";
			twma7.REC_CREATOR = s.userid;
			twma7.REC_CREATE_TIME = dateNow14;
			twma7.MAT_ACT_WIDTH = tmmsm01.MAT_ACT_WIDTH;
			twma7.MAT_ACT_LEN = tmmsm01.MAT_ACT_LEN;
			twma7.MAT_STATUS = tmmsm01.MAT_STATUS;
			twma7.MAT_DESTION = tmmsm01.MAT_DESTION;
			twma7.MAT_THEORY_WT = tmmsm01.MAT_THEORY_WT;
			twma7.HEAD_TAIL_WITH_DIFF = (tmmsm01.SLAB_HEAD_WIDTH - tmmsm01.SLAB_TAIL_WIDTH).Abs();
			twma7.Insert();

			//zhenglei：增加tymsm01表垛位预约数的维护，查询目前未确认的板坯命令数（目标垛位），然后更新tymsm01表
			twm04_T.PRE_MAT_NUM = twm04_T.PRE_MAT_NUM + 1;
			twm04_T.Update("PRE_MAT_NUM","STOCK_PLACE_NO");
			twm04_Y.STOCK_STATUS = "1";
			twm04_Y.Update("STOCK_STATUS", "STOCK_PLACE_NO");
			tmmsm01.CMD_FLAG = "1";
			tmmsm01.Update("CMD_FLAG","MAT_NO");
			
			//EXEC SQL update tymsm01 set pre_mat_num = pre_mat_num + 1 where stock_place_no = :tymsm10.targ_pos;
			//EXEC SQL update tymsm01 set stock_status = '1' where stock_place_no = :tymsm10.source_pos;
			//EXEC SQL update tmmsm01 set cmd_flag = '1' where mat_position = '2'	and mat_no = :tymsm10.mat_no;//行车命令标志，1：命令中，0：无命令

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


