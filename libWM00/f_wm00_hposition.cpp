/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 垛板台高低位处理
**************************************************/

#include "stdafx.h"
#include "tep0002.h"
#include "tmmsm01.h"
#include "twm04.h"
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */
int f_wm00_pile_recom(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wm00_pile_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wm00_pile_final_recom(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wm00_pile_comd_prod(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_wm00_hposition(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	int i_mat_num = 0;//板坯块数
	int i_width[4] = { 0, 0, 0, 0 };//保存板坯宽度
	char c_mat_no[4][21] = { " ", " ", " ", " " };//保存板坯号
	char c_pile_value[4][3] = { " ", " ", " ", " " };//保存分区属性值
	char c_targ_pos[4][8] = { " ", " ", " ", " " };//保存推荐垛位
	int i_rec_flag[4] = { 1, 1, 1, 1 };//垛位推荐标记，1推荐，0用上一块
	int i_rec_flag_tem[4] = { 1, 1, 1, 1 };
	CString v_pile_comd_status = "0"; //2016-10-14：行车命令是否自动发送状态，0，不发，1：自动生成
	CString v_pos = "000";//2016-11-1：垛位推荐用的传递值
	CString RetFlag = " ";
	//定义实体类
	CTEP0002 tep0002(conn);
	CTMMSM01 tmmsm01(conn);
	CTWM04 twm04(conn);

	CDbCommand cmd_inq(conn);
	CDbCommand cmd(conn);
	EIClass bcls_mmsm01;
	//

	//
	EIClass bcls_pile_jud;
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");//
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "Y");//
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "SLAB_PILE_VALUE");//
	bcls_pile_jud.Tables[0].Rows.Add();
	EIClass bcls_pile_jud_f;
	//
	EIClass bcls_pile_final;
	bcls_pile_final.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
	bcls_pile_final.Tables[0].Columns.Add(DT_STRING, "B");//
	bcls_pile_final.Tables[0].Columns.Add(DT_STRING, "POS");//
	bcls_pile_final.Tables[0].Rows.Add();
	EIClass bcls_pile_final_f;
	//
	EIClass bcls_pile_comd;
	bcls_pile_comd.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
	bcls_pile_comd.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_FROM");//源垛位
	bcls_pile_comd.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO_TO");//目标垛位
	bcls_pile_comd.Tables[0].Columns.Add(DT_STRING, "CRANE_NO");//行车号
	bcls_pile_comd.Tables[0].Columns.Add(DT_STRING, "CMD_TYPE");//
	bcls_pile_comd.Tables[0].Rows.Add();

	//
	CString v_loc_code= " ";
	CString v_loc_code_old = " ";//2016-10-14：取更新前的跺板台状态值
	CString v_loc_code_switch = "OFF";//2016-10-14：取跺板台自动行车命令开关值
	CString v_mat_no= " ";
	CString v_stock_place_no= " ";
	CString v_cmd_switch = "OFF";//2016-11-1：取跺板台自动行车命令开关值
	CString v_pile_value = " ";//2016-11-1：板坯分区值
	CString v_targ_pos = "000";//2016-11-1：板坯垛位值
	CString v_mat_no_temp = " ";//临时用
	CString v_pile_value_temp = " ";//临时用
	int v_slab_num_act = 0;
	int v_pile_height_max = 0;
	int v_pre_mat_num = 0;
	try
	{
		EDLog(1, 1, "-----------------f_wm00_hposition begin -----------------");
		//接收板坯位置信息
		v_loc_code = bcls_rec->Tables[0].Rows[0]["LOC_CODE"];

		//垛板台I101
		if (0 == strcmp(v_loc_code, "31") || 0 == strcmp(v_loc_code, "32") || 0 == strcmp(v_loc_code, "33"))
		{
			//2016-10-14：取更新前的跺板台状态值
			tep0002.Reset();
			tep0002.CODE_CLASS = "WMY2";
			tep0002.CODE = "location_I101";
			if (!tep0002.Query("CODE_CLASS,CODE")){
				strcpy(s.msg, "location_I101板坯库开关变量WMY2查询失败.");
				s.flag = -1;
				return -1;
			}
			v_loc_code_old = tep0002.CODE_DESC_1_CONTENT;
			v_loc_code_switch = tep0002.CODE_DESC_2_CONTENT;
			v_cmd_switch = tep0002.CODE_DESC_3_CONTENT;
			Log::Trace("", __FUNCTION__, "v_loc_code = [{0}]", v_loc_code);
			Log::Trace("", __FUNCTION__, "v_loc_code_old = [{0}]", v_loc_code_old);
			Log::Trace("", __FUNCTION__, "v_loc_code_switch = [{0}]", v_loc_code_switch);
			Log::Trace("", __FUNCTION__, "v_cmd_switch = [{0}]", v_cmd_switch);
			//2016-10-14：判断状态是否由32直接跳到31，如果是则需要在31状态下发行车命令，避免漏发
			if (0 == strcmp(v_loc_code, "31") && 0 == strcmp(v_loc_code_old, "32") && 0 == strcmp(v_loc_code_switch, "ON"))
			{
				v_pile_comd_status = "1";
				v_stock_place_no = "I101";
			}
			else
			{

				if (0 == strcmp(v_loc_code, "33") && 0 == strcmp(v_loc_code_switch, "ON"))
				{
					v_pile_comd_status = "1";
					v_stock_place_no = "I101";
				}

			}

			//更新跺板台状态值
			tep0002.CODE_DESC_1_CONTENT = v_loc_code;
			tep0002.Update("CODE_DESC_1_CONTENT","CODE_CLASS,CODE");
			sqlstr = "UPDATE TEP0002_RES SET CODE_DESC_1_CONTENT = @v_loc_code"
				" WHERE CULTURE = 'zh_Hans' AND CODE_CLASS = 'WMY2' AND CODE = @CODE";
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd.Parameters.Set("v_loc_code", v_loc_code);
			cmd.Parameters.Set("CODE", tep0002.CODE);
			cmd.ExecuteNonQuery();
		}
		else if (0 == strcmp(v_loc_code, "41") || 0 == strcmp(v_loc_code, "42") || 0 == strcmp(v_loc_code, "43"))
		{//垛板台 i102
			//2016-10-14：取更新前的跺板台状态值
			tep0002.Reset();
			tep0002.CODE_CLASS = "WMY2";
			tep0002.CODE = "location_I102";
			if (!tep0002.Query("CODE_CLASS,CODE")){
				strcpy(s.msg, "location_I101板坯库开关变量WMY2查询失败.");
				s.flag = -1;
				return -1;
			}
			v_loc_code_old = tep0002.CODE_DESC_1_CONTENT;
			v_loc_code_switch = tep0002.CODE_DESC_2_CONTENT;
			v_cmd_switch = tep0002.CODE_DESC_3_CONTENT;
			//2016-10-14：判断状态是否由32直接跳到31，如果是则需要在31状态下发行车命令，避免漏发
			if (0 == strcmp(v_loc_code, "41") && 0 == strcmp(v_loc_code_old, "42") && 0 == strcmp(v_loc_code_switch, "ON"))
			{
				v_pile_comd_status = "1";
				v_stock_place_no = "I102";
			}
			else
			{
				if (0 == strcmp(v_loc_code, "43") && 0 == strcmp(v_loc_code_switch, "ON"))
				{
					v_pile_comd_status = "1";
					v_stock_place_no = "I102";
				}
			}
			//更新跺板台状态值
			tep0002.CODE_DESC_1_CONTENT = v_loc_code;
			tep0002.Update("CODE_DESC_1_CONTENT", "CODE_CLASS,CODE");
			sqlstr = "UPDATE TEP0002_RES SET CODE_DESC_1_CONTENT = @v_loc_code"
				" WHERE CULTURE = 'zh_Hans' AND CODE_CLASS = 'WMY2' AND CODE = @CODE";
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd.Parameters.Set("v_loc_code", v_loc_code);
			cmd.Parameters.Set("CODE", tep0002.CODE);
			cmd.ExecuteNonQuery();
		}
			else if (0 == strcmp(v_loc_code, "51") || 0 == strcmp(v_loc_code, "52") || 0 == strcmp(v_loc_code, "53"))
			{//垛板台 H101
				//2016-10-14：取更新前的跺板台状态值
				tep0002.Reset();
				tep0002.CODE_CLASS = "WMY2";
				tep0002.CODE = "location_H101";
				if (!tep0002.Query("CODE_CLASS,CODE")){
					strcpy(s.msg, "location_H101板坯库开关变量WMY2查询失败.");
					s.flag = -1;
					return -1;
				}
				v_loc_code_old = tep0002.CODE_DESC_1_CONTENT;
				v_loc_code_switch = tep0002.CODE_DESC_2_CONTENT;
				v_cmd_switch = tep0002.CODE_DESC_3_CONTENT;
				//2016-10-14：判断状态是否由32直接跳到31，如果是则需要在31状态下发行车命令，避免漏发
				if (0 == strcmp(v_loc_code, "51") && 0 == strcmp(v_loc_code_old, "52") && 0 == strcmp(v_loc_code_switch, "ON"))
				{
					v_pile_comd_status = "1";
					v_stock_place_no = "H101";
				}
				else
				{
					if (0 == strcmp(v_loc_code, "53") && 0 == strcmp(v_loc_code_switch, "ON"))
					{
						v_pile_comd_status = "1";
						v_stock_place_no = "H101";
					}
				}
			//更新跺板台状态值
			tep0002.CODE_DESC_1_CONTENT = v_loc_code;
			tep0002.Update("CODE_DESC_1_CONTENT", "CODE_CLASS,CODE");
			sqlstr = "UPDATE TEP0002_RES SET CODE_DESC_1_CONTENT = @v_loc_code"
				" WHERE CULTURE = 'zh_Hans' AND CODE_CLASS = 'WMY2' AND CODE = @CODE";
			cmd.Close();
			cmd.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd.Parameters.Set("v_loc_code", v_loc_code);
			cmd.Parameters.Set("CODE", tep0002.CODE);
			cmd.ExecuteNonQuery();
		}
		Log::Trace("", __FUNCTION__, "v_pile_comd_status = [{0}]", v_pile_comd_status);
		//板坯在高位时根据垛位分别产生不同的行车命令（根据是否自动要求）
		if (0 == strcmp(v_pile_comd_status, "1"))
		{
			sqlstr = "SELECT * FROM tmmsm01 WHERE stock_place_no=@stock_place_no AND cmd_flag != '1' ORDER BY layerno ASC";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.Parameters.Set("stock_place_no", v_stock_place_no);
			int count = cmd_inq.ExecuteQuery(bcls_mmsm01.Tables[0]);
			for (int i = 0; i < count;i++)
			{
				tmmsm01.Reset();
				tmmsm01.MergeFrom(bcls_mmsm01.Tables[0].Rows[i]);
				strcpy(c_mat_no[i], tmmsm01.MAT_NO);//保存板坯号
				i_width[i] = tmmsm01.MAT_ACT_WIDTH.ToInt32();//保存对应宽度


				//调用垛位属性计算
				EDLog(1, 1, "板坯分区属性计算开始");
				EIClass bcls_rec_f;
				bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
				bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PILE_JUDGE_TYPE");//判断类别
				bcls_rec_f.Tables[0].Rows.Add();
				bcls_rec_f.Tables[0].Rows[0]["MAT_NO"] = tmmsm01.MAT_NO;
				bcls_rec_f.Tables[0].Rows[0]["PILE_JUDGE_TYPE"] = "A001";
				Log::Trace("", __FUNCTION__, "========================f_wm00_pile_recom开始11==============================");
				doFlag = f_wm00_pile_recom(&bcls_rec_f, bcls_ret, conn);
				Log::Trace("", __FUNCTION__, "========================f_wm00_pile_recom结束11==============================");
				if (doFlag != 0)
				{
					Log::Trace("", __FUNCTION__, "函数调用失败f_wm00_pile_recom() msg = [%s]", s.msg);
					//bcls_temp.Tables[0].Rows[i]["PILE_VALUE"] = " " ;//分区属性
					/*goto l_suberror;*/
					s.flag = -1;
					doFlag = -1;
					throw CApplicationException(doFlag, s.msg, s.svc_name);
				}
				else
				{
					//取计算的分区值
					v_pile_value = bcls_ret->Tables[0].Rows[0]["PILE_VALUE"];
					Log::Trace("", __FUNCTION__, "v_pile_value = [{0}]", v_pile_value);
					i_mat_num++;
				}
				strcpy(c_pile_value[i], v_pile_value);//保存分区属性值

			}//循环结束
			Log::Trace("", __FUNCTION__, "板坯块数 = [{0}]", i_mat_num);

			if (i_mat_num > 3)//超3块就不再推荐
			{
				Log::Trace("", __FUNCTION__, "1111.");
				sprintf(s.msg, "板坯块数超过3.");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			/***************************************************************************/
			//垛位推荐部分
			for (int i = 0; i<i_mat_num; i++)
			{
				Log::Trace("", __FUNCTION__, "推荐板坯号 = [{0}]", c_mat_no[i]);
				Log::Trace("", __FUNCTION__, "计划推荐垛位 = [{0}]", v_targ_pos);
				//EDLog(1, 1, "*推荐板坯号 = 【%s】,计划推荐垛位 = 【%s】!*", c_mat_no[i], v_targ_pos);
				if (i_rec_flag[i] == 0 && 0 != strcmp(v_targ_pos, "X01"))
				{
					Log::Trace("", __FUNCTION__, "*板坯分区属性值相同，宽度满足要求，直接进行垛位判断(第一层)*");

					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = c_mat_no[i];
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_targ_pos;
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "1";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = c_pile_value[i];
					Log::Trace("", __FUNCTION__, "========================f_wm00_pile_jud开始==============================");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, &bcls_pile_jud_f,conn);
					Log::Trace("", __FUNCTION__, "========================f_wm00_pile_jud结束==============================");
					if (0 != doFlag)
					{
						Log::Trace("", __FUNCTION__, "*推荐垛位板坯判断过程调用失败(第一层)*");
						i_rec_flag[i] = 1;

					}
					RetFlag = bcls_pile_jud_f.Tables[0].Rows[0]["RETFLAG"];
					Log::Trace("", __FUNCTION__, "RetFlag = [{0}]", RetFlag);
					if (0 == strcmp(RetFlag, "Y"))
					{
						Log::Trace("", __FUNCTION__, "*垛位满足要求，推荐成功(第一层)*");
						strcpy(c_targ_pos[i], v_targ_pos);//保存推荐垛位
						Log::Trace("", __FUNCTION__, "c_targ_pos[i] = [{0}]", c_targ_pos[i]);
					}
					else
					{
						Log::Trace("", __FUNCTION__, "*垛位不满足要求，重新推荐(第一层)*");
						i_rec_flag[i] = 1;
						v_pos = v_targ_pos;
						Log::Trace("", __FUNCTION__, "v_pos = [{0}]", v_pos);
					}
				}
				else
				{
					Log::Trace("", __FUNCTION__, "*板坯分区属性值不相同或规格不满足需要重新推荐！*");
				}

				//垛位推荐
				if (i_rec_flag[i] == 1 || 0 == strcmp(v_targ_pos, "X01"))
				{
					//调用垛位推荐
					Log::Trace("", __FUNCTION__, "*板坯垛位推荐开始(第一层)*");
					bcls_pile_final.Tables[0].Rows[0]["MAT_NO"] = c_mat_no[i];
					bcls_pile_final.Tables[0].Rows[0]["B"] = c_pile_value[i];
					bcls_pile_final.Tables[0].Rows[0]["POS"] = v_pos;
					Log::Trace("", __FUNCTION__, "========================f_wm00_pile_final_recom开始==============================");
					doFlag = f_wm00_pile_final_recom(&bcls_pile_final, &bcls_pile_final_f, conn);
					Log::Trace("", __FUNCTION__, "========================f_wm00_pile_final_recom结束==============================");
					if (0 != doFlag)
					{
						Log::Trace("", __FUNCTION__, "*垛位推荐失败(第一层)*11");
						v_targ_pos = "X01";
						//strcpy(v_targ_pos, "X01");//如果垛位推荐失败则默认推荐垛位为X01
						//goto l_apperror;
					}
					else
					{
						v_targ_pos = bcls_pile_final_f.Tables[0].Rows[0]["TARG_POS"];
						Log::Debug("", __FUNCTION__, "返回垛位号 = [{0}]", v_targ_pos);
					}
					
					strcpy(c_targ_pos[i], v_targ_pos);//保存推荐垛位
					Log::Debug("", __FUNCTION__, "c_targ_pos[i] = [{0}]", c_targ_pos[i]);
				}

				//下一块板坯垛位推荐预处理
				if (i < i_mat_num - 1)//对一次两块的情况进行计算
				{
					if (0 == strcmp(c_pile_value[i], c_pile_value[i + 1]))
					{
						if ((i_width[i] >= 1200 && i_width[i + 1] >= 1200) || abs(i_width[i] - i_width[i + 1]) <= 350)
						{
							i_rec_flag[i + 1] = 0;
							i_rec_flag_tem[i + 1] = 0;
						}
						else
						{
							twm04.Reset();
							twm04.STOCK_PLACE_NO = v_targ_pos;
							if (!twm04.Query("STOCK_PLACE_NO")){
								strcpy(s.msg, "目标垛位" + v_targ_pos+"没有在垛位表中进行维护");
								s.flag = -1;
								return -1;
							}
							v_slab_num_act = twm04.PILE_MAT_NUM_ACT.ToInt32();
							v_pile_height_max = twm04.MAX_HEIGHT.ToInt32();
							v_pre_mat_num = twm04.PRE_MAT_NUM.ToInt32();
								   ;
							if ((3 >= v_pile_height_max - (v_slab_num_act + v_pre_mat_num)) && ((i_width[i] - i_width[i + 1]) > -50) && v_pile_height_max > 0)
							{
								i_rec_flag[i + 1] = 0;
								i_rec_flag_tem[i + 1] = 0;
							}
							else
							{
								v_pos=v_targ_pos;
							}
						}
					}
				}

				//重新处理推荐垛位
				if (i >= 1 && 0 == strcmp(c_pile_value[i - 1], c_pile_value[i]) && 0 != strcmp(c_targ_pos[i - 1], c_targ_pos[i]) && i_rec_flag_tem[i] == 0)
				{
					//重新验证垛位
					Log::Debug("", __FUNCTION__, "*重新用第二层板坯推荐垛位验证第一块板坯推荐(第一层)*");
					
					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = c_mat_no[i - 1];
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = c_targ_pos[i];
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = c_pile_value[i - 1];

					Log::Trace("", __FUNCTION__, "========================f_wm00_pile_jud开始==============================");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, &bcls_pile_jud_f, conn);
					Log::Trace("", __FUNCTION__, "========================f_wm00_pile_jud推荐==============================");
					if (0 != doFlag)
					{
						Log::Debug("", __FUNCTION__, "*推荐垛位板坯判断过程调用失败(第一层)*");
			
					}
					RetFlag = bcls_pile_jud_f.Tables[0].Rows[0]["RETFLAG"];

					if (0 == strcmp(RetFlag, "Y"))
					{
	
						strcpy(c_targ_pos[i - 1], c_targ_pos[i]);//更换推荐垛位

						if (i == 2 && 0 == strcmp(c_pile_value[i - 2], c_pile_value[i - 1]) && 0 != strcmp(c_targ_pos[i - 2], c_targ_pos[i - 1]) && i_rec_flag_tem[i - 1] == 0)
						{
							//重新验证垛位
							Log::Debug("", __FUNCTION__, "*重新用第二层板坯推荐垛位验证第一块板坯推荐(第一层)*");
							bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = c_mat_no[i - 2];
							bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = c_targ_pos[i];
							bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
							bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = c_pile_value[i - 2];
							Log::Trace("", __FUNCTION__, "========================f_wm00_pile_jud开始444==============================");
							doFlag = f_wm00_pile_jud(&bcls_pile_jud, &bcls_pile_jud_f, conn);
							Log::Trace("", __FUNCTION__, "========================f_wm00_pile_jud结束444==============================");
							if (0 != doFlag)
							{
								Log::Debug("", __FUNCTION__, "*推荐垛位板坯判断过程调用失败(第一层)*");

							}
							RetFlag = bcls_pile_jud_f.Tables[0].Rows[0]["RETFLAG"];

							if (0 == strcmp(RetFlag, "Y"))
							{
								//EDLog(1, 1, "*垛位满足要求，修改原推荐垛位【%s】为新垛位【%s】(第一层)*", c_targ_pos[i - 2], c_targ_pos[i]);
								//c_targ_pos[i - 2] = c_targ_pos[i];
								strcpy(c_targ_pos[i - 2], c_targ_pos[i]);//保存推荐垛位							
							}
							else
							{
								Log::Debug("", __FUNCTION__, "*垛位不满足要求，不修改推荐垛位(第一层)*");
							}
						}
					}
					else
					{
						Log::Debug("", __FUNCTION__, "*垛位不满足要求，不修改推荐垛位(第一层)*");
					}
				}
			}
			//调用行车命令生成
			if (0 == strcmp(v_cmd_switch, "ON"))
			{
				Log::Debug("", __FUNCTION__, "进入行车命令");
				for (int i = i_mat_num - 1; i >= 0; i--)
				{
					//更新板坯垛位分区属性值及计算出来的垛位号
					v_mat_no_temp=c_mat_no[i];
					v_pile_value_temp = c_pile_value[i];
					//strcpy(v_mat_no_temp, c_mat_no[i]);
					//strcpy(v_pile_value_temp, c_pile_value[i]);

					sqlstr = "UPDATE TMMSM01 SET SCRAP_REMARK = @v_targ_pos,"
						" DEFECT_DEGREE_L_4 = @v_pile_value_temp WHERE MAT_NO = @v_mat_no_temp";
					cmd_inq.Close();
					cmd_inq.SetCommandText(sqlstr);
					Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
					cmd_inq.Parameters.Set("v_targ_pos", v_targ_pos);
					cmd_inq.Parameters.Set("v_pile_value_temp", v_pile_value_temp);
					cmd_inq.Parameters.Set("v_mat_no_temp", v_mat_no_temp);
					cmd_inq.ExecuteNonQuery();
					
						;

					Log::Debug("", __FUNCTION__, "自动生成行车命令");
					bcls_pile_comd.Tables[0].Rows[0]["MAT_NO"] = c_mat_no[i];
					bcls_pile_comd.Tables[0].Rows[0]["STOCK_PLACE_NO_FROM"] = v_stock_place_no;
					bcls_pile_comd.Tables[0].Rows[0]["STOCK_PLACE_NO_TO"] = c_targ_pos[i];
					bcls_pile_comd.Tables[0].Rows[0]["CRANE_NO"] = "99";//行车号：通用
					bcls_pile_comd.Tables[0].Rows[0]["CMD_TYPE"] = "1";//命令标识：0手动，1自动

					doFlag = f_wm00_pile_comd_prod(&bcls_pile_comd, bcls_ret, conn);

					if (doFlag != 0)
					{

						Log::Debug("", __FUNCTION__, "f_ymsm_pile_comd_prod失败");
					}
				}
			}
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


