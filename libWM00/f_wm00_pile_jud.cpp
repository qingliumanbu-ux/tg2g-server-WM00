/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 炼钢板坯垛位判断
**************************************************/

#include "stdafx.h"
#include "tep0002.h"
#include "tmmsm01.h"
#include "twm04.h"
#include "twm0e.h"
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */


int f_wm00_pile_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	CString v_mat_no, v_stock_place_no = "";
	CDecimal v_circle, v_slab_pile_value = 0;
	CDecimal slab_cold_time = 0;
	CDecimal v_max_cold_time = 0;
	CDecimal w_max = 0;
	CDecimal w_min = 0;
	CDecimal l_max = 0;
	CDecimal l_min = 0;
	CDecimal v_slab_num = 0;
	CString v_cut_time_min = "0";
	//CDecimal v_cut_time_min = 0;
	CString ch_pile_value_min = "0";
	CString ch_pile_value_max = "0";
	int	checkFlag = 0;//忽略判定条件
	int	slab_cold_flag = 0;//垛位冷热区分
	CString RetFlag = "N";
	//定义实体类
	CTMMSM01 tmmsm01(conn);
	CTWM04 twm04(conn);

	CDbCommand cmd_inq(conn);
	
	try
	{
		
		v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];
		v_stock_place_no = bcls_rec->Tables[0].Rows[0]["STOCK_PLACE_NO"];
		v_circle = bcls_rec->Tables[0].Rows[0]["Y"].ToDecimal();
		v_slab_pile_value = bcls_rec->Tables[0].Rows[0]["slab_pile_value"].ToDecimal();
		Log::Debug("", __FUNCTION__, "传入参数v_mat_no = [{0}]", v_mat_no);
		Log::Debug("", __FUNCTION__, "传入参数v_stock_place_no = [{0}]", v_stock_place_no);
		Log::Debug("", __FUNCTION__, "传入参数v_circle = [{0}]", v_circle);
		Log::Debug("", __FUNCTION__, "传入参数v_slab_pile_value = [{0}]", v_slab_pile_value);

		if (!bcls_ret->Tables[0].Columns.Contains("RETFLAG"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "RETFLAG");
		}
		if (bcls_ret->Tables[0].Rows.get_Count() == 0){
			bcls_ret->Tables[0].Rows.Add();
		}
		//获取传入垛位的属性
		twm04.Reset();
		twm04.STOCK_PLACE_NO = v_stock_place_no;
		if (!twm04.Query("STOCK_PLACE_NO")){
			s.flag = -1;
			strcpy(s.msg, "垛位号" + v_stock_place_no + "查询失败！");
			return -1;
		}

			
		tmmsm01.Reset();
		tmmsm01.MAT_NO = v_mat_no;
		if (!tmmsm01.Query("MAT_NO")){
			s.flag = -1;
			strcpy(s.msg, "板坯号" + v_mat_no + "查询失败！");
			return -1;
		}

		//如果垛位堆放块数达到总要求的80%以上，则取消部分限制条件
		if ((twm04.MAX_HEIGHT - twm04.PILE_MAT_NUM_ACT) <= 3)
		{
			checkFlag = 1;
		}
		Log::Debug("", __FUNCTION__, "传入参数checkFlag = [{0}]", checkFlag);

		//如果传入垛位为非正常垛位直接退出
		if (0 == strcmp(v_stock_place_no, "X01"))
		{
			//EDLog(1, 1, "传人垛位为X01，直接结束");
			RetFlag = "N";
		}
		else{

			//传入参数为1时表示对垛位不进行冷热区分,二热轧合格坯不考虑垛位中的冷热情况，满足其他要求就可以堆放
			if (v_circle == 1 || (0 == strcmp(tmmsm01.SURFACE_DECIDE_CODE, "1") && (0 == strcmp(tmmsm01.MAT_DESTION, "01") || 0 == strcmp(tmmsm01.MAT_DESTION, "00"))))
			{
				slab_cold_flag = 1;
			}

			//板坯冷却时间：当前时间-切断时间
			sqlstr = "SELECT timestampdiff(8, CHAR(TIMESTAMP(to_date('" + dateNow14 + "', 'yyyy-mm-dd hh24:mi:ss')) - TIMESTAMP(to_date('" + tmmsm01.SLAB_CUT_TIME + "', 'yyyy-mm-dd hh24:mi:ss')))) AS diffTimes16"
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
			slab_cold_time = cmd_inq.GetDecimal(1);//时间差

			//垛位判断
			if (twm04.COLD_HOT_REQ == 999 || twm04.COLD_HOT_REQ < slab_cold_time)//冷热要求判断
			{
				Log::Trace("", __FUNCTION__, "冷热要求判断符合要求");
				if (tmmsm01.MAT_ACT_LEN < twm04.MAX_LEN && tmmsm01.MAT_ACT_WIDTH < twm04.MAX_WIDTH)//长度、宽度范围判断
				{
					Log::Trace("", __FUNCTION__, "长度、宽度范围判断");
					//若该垛位上没有板坯，则直接将返回值赋成Y表示找到该垛位
					if (0 == twm04.PILE_MAT_NUM_ACT)
					{
						Log::Trace("", __FUNCTION__, "该垛位上没有板坯,该垛位符合要求");
						RetFlag = "Y";
						bcls_ret->Tables[0].Rows[0]["RETFLAG"] = RetFlag;
					}
					else
					{
						//判断垛位是否允许混堆放（相同扩展属性，不同主属性）,传入参数为9则代表不考虑属性校验
						if (0 != strcmp(twm04.FIELDNO, "9"))
						{
							if (twm04.PILE_MAIN_NO != v_slab_pile_value)
							{
								//EDLog(1, 1, "垛位属性值与板坯属性值不匹配，不允许堆放!!");
								RetFlag = "N";
								bcls_ret->Tables[0].Rows[0]["RETFLAG"] = RetFlag;
								return doFlag;
								//goto l_retflag;
							}
						}
						else
						{
							if (twm04.PILE_MAIN_NO / 10 != v_slab_pile_value / 10)
							{
								//EDLog(1, 1, "板坯属性值不满足混堆要求，不允许堆放!!");
								RetFlag = "N";
								bcls_ret->Tables[0].Rows[0]["RETFLAG"] = RetFlag;
								return doFlag;
								//goto l_retflag;
							}
						}
						//取垛位中板坯数据
						sqlstr = "SELECT    NVL(MIN(MAT_ACT_WIDTH), 0) AS MAT_ACT_WIDTH_MIN,"
							" NVL(MAX(MAT_ACT_WIDTH), 0) AS MAT_ACT_WIDTH_MAX,"
							" NVL(MIN(MAT_ACT_LEN), 0) AS MAT_ACT_LEN_MIN,"
							" NVL(MAX(MAT_ACT_LEN), 0) AS MAT_ACT_LEN_MAX,"
							" NVL(MIN(SLAB_CUT_TIME), '19000701000000') AS SLAB_CUT_TIME"
							 "  FROM   TMMSM01 WHERE   STOCK_PLACE_NO = @v_stock_place_no";
						cmd_inq.Close();
						cmd_inq.SetCommandText(sqlstr);
						Log::Trace("", __FUNCTION__, "v_stock_place_no = [{0}]", v_stock_place_no);
						Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
						cmd_inq.Parameters.Set("v_stock_place_no", v_stock_place_no);
						cmd_inq.ExecuteReader();
						cmd_inq.Read();
						w_min = cmd_inq.GetDecimal(1);
						w_max = cmd_inq.GetDecimal(2);
						l_min = cmd_inq.GetDecimal(3);
						l_max = cmd_inq.GetDecimal(4);
						v_cut_time_min = cmd_inq.GetString(5);
						
						//最大冷却时间
						sqlstr = "SELECT timestampdiff(8, CHAR(TIMESTAMP(to_date('" + dateNow14 + "', 'yyyy-mm-dd hh24:mi:ss')) - TIMESTAMP(to_date('" + v_cut_time_min + "', 'yyyy-mm-dd hh24:mi:ss')))) AS diffTimes16"
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
						v_max_cold_time = cmd_inq.GetDecimal(1);//时间差

						if ((w_min >= 1200 && tmmsm01.MAT_ACT_WIDTH >= 1200) || ((tmmsm01.MAT_ACT_WIDTH - w_min).Abs() <= twm04.WID_DIFF_MAX && (tmmsm01.MAT_ACT_WIDTH - w_max).Abs() <= twm04.WID_DIFF_MAX))//2016-10-13:
						{
							//EDLog(1, 1, "宽度与垛位中板坯判断满足要求");
							if ((tmmsm01.MAT_ACT_LEN - l_min).Abs() <= twm04.LEN_DIFF_MAX && (tmmsm01.MAT_ACT_LEN - l_max).Abs() <= twm04.LEN_DIFF_MAX)//2016-10-13:
							{
								EDLog(1, 1, "长度与垛位中板坯判断满足要求");
								if (slab_cold_flag == 1 || twm04.INTER_OP_TIME_HH == 99 || (slab_cold_time - v_max_cold_time).Abs() <= twm04.INTER_OP_TIME_HH)
								{
									Log::Trace("", __FUNCTION__, "冷却时间差 = [{0}]", (slab_cold_time - v_max_cold_time).Abs());
									Log::Trace("", __FUNCTION__, "冷却时间差满足要求");
									RetFlag = "Y";
								}
								else
								{
									Log::Trace("", __FUNCTION__, "冷却时间差不满足要求");
									RetFlag = "N";
								}
							}
							else if (checkFlag == 1 && (l_max - tmmsm01.MAT_ACT_LEN) > twm04.LEN_DIFF_MAX)
							{
								Log::Trace("", __FUNCTION__, "修正后长度与垛位中板坯判断满足要求");
								RetFlag = "Y";
							}
							else
							{
								Log::Trace("", __FUNCTION__, "长度不满足要求");
								RetFlag = "N";
							}
						}
						else if (checkFlag == 1 && (w_max - tmmsm01.MAT_ACT_WIDTH) > twm04.WID_DIFF_MAX)
						{
							Log::Trace("", __FUNCTION__, "修正后宽度与垛位中板坯判断满足要求");
							RetFlag = "Y";
						}
						else
						{
							Log::Trace("", __FUNCTION__, "宽度不满足要求");
							RetFlag = "N";
						}
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "规格不满足垛位要求");
					RetFlag = "N";
				}

			}
			else
			{
				Log::Trace("", __FUNCTION__, "冷热不满足垛位要求");
				RetFlag = "N";
			}

			if (0 == strcmp(RetFlag, "Y"))//还需要校验推荐的目标垛位是否有其他命令冲突
			{
				sqlstr = "SELECT	count(*) FROM   twma7  WHERE   STOCK_PLACE_NO_TO = @v_stock_place_no AND CRANE_INST_STATUS = '0'";
				cmd_inq.Close();
				cmd_inq.SetCommandText(sqlstr);
				Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
				cmd_inq.Parameters.Set("v_stock_place_no", v_stock_place_no);
				cmd_inq.ExecuteReader();
				cmd_inq.Read();
				v_slab_num = cmd_inq.GetDecimal(1);
				if (v_slab_num > 0)
				{
					Log::Trace("", __FUNCTION__, "增加对命令中同目的垛位板坯的判断");
					sqlstr = "SELECT	NVL(MIN(MAT_ACT_THICK), 0) AS MAT_ACT_WIDTH_MIN,"
						" NVL(MAX(MAT_ACT_THICK), 0) AS MAT_ACT_WIDTH_MAX,"
						" NVL(MIN(MAT_ACT_WT), 0) AS MAT_ACT_LEN_MIN,"
						" NVL(MAX(MAT_ACT_WT), 0) AS MAT_ACT_LEN_MAX"
						" FROM   twma7  WHERE   STOCK_PLACE_NO_TO = @v_stock_place_no AND CRANE_INST_STATUS = '0'";
					cmd_inq.Close();
					cmd_inq.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd_inq.Parameters.Set("v_stock_place_no", v_stock_place_no);
					cmd_inq.ExecuteReader();
					cmd_inq.Read();
					w_min = cmd_inq.GetDecimal(1);
					w_max = cmd_inq.GetDecimal(2);
					l_max = cmd_inq.GetDecimal(3);
					ch_pile_value_min = "0";//FIN_POS 在梅钢中都是空，所以都是0
					ch_pile_value_max = "0";//FIN_POS 在梅钢中都是空，所以都是0
				

					if (0 != strcmp(ch_pile_value_max, " ") && (v_slab_pile_value != atoi(ch_pile_value_min) || v_slab_pile_value != atoi(ch_pile_value_max)))
					{
						//EDLog(1, 1, "命令目标垛位中有不同属性的板坯，不推荐该垛位！！");
						RetFlag = "N";
						bcls_ret->Tables[0].Rows[0]["RETFLAG"] = RetFlag;
						return doFlag;
					}
					//命令中板坯校验
					if ((w_min * 10 >= 1200 && tmmsm01.MAT_ACT_WIDTH >= 1200) || ((tmmsm01.MAT_ACT_WIDTH - w_min * 10).Abs() <= twm04.WID_DIFF_MAX && (tmmsm01.MAT_ACT_WIDTH - w_max * 10).Abs() <= twm04.WID_DIFF_MAX))//2016-10-13:
					{
						Log::Trace("", __FUNCTION__, "宽度与命令中板坯判断满足要求");
						if ((tmmsm01.MAT_ACT_LEN - l_min).Abs() <= twm04.LEN_DIFF_MAX && (tmmsm01.MAT_ACT_LEN - l_max).Abs() <= twm04.LEN_DIFF_MAX)//2016-10-13:
						{
							Log::Trace("", __FUNCTION__, "长度与命令中板坯判断满足要求");
							RetFlag = "Y";
						}
						else if (checkFlag == 1 && (l_max - tmmsm01.MAT_ACT_LEN) > twm04.LEN_DIFF_MAX)
						{
							Log::Trace("", __FUNCTION__, "修正后长度与命令中板坯判断满足要求");
							RetFlag = "Y";
						}
						else
						{
							Log::Trace("", __FUNCTION__, "与命令中板坯长度不满足要求");
							RetFlag = "N";
						}
					}
					else if (checkFlag == 1 && (w_max * 10 - tmmsm01.MAT_ACT_WIDTH) > twm04.WID_DIFF_MAX)
					{
						Log::Trace("", __FUNCTION__, "修正后宽度与命令中板坯判断满足要求");
						RetFlag = "Y";
					}
					else
					{
						Log::Trace("", __FUNCTION__, "与命令中板坯宽度不满足要求");
						RetFlag = "N";
					}
				}
			}

		}
		
		bcls_ret->Tables[0].Rows[0]["RETFLAG"] = RetFlag;
		Log::Debug("", __FUNCTION__, "返回RetFlag = [{0}]", RetFlag);
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


