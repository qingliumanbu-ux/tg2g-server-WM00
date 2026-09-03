/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 库位推荐
**************************************************/

#include "stdafx.h"
//#include "1j197b.h"//电文接收
BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */
int f_wm00_hposition(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
struct CX1J197B
{
	CDecimal STRAND_NO;   //流号
	CString MAT_NO;   //材料号
	CString LOC_CODE;   //位置代码
	CDecimal MAT_ACT_WT;   //材料实际重量
	CDecimal HEAD_WIDTH;   //头部宽度
	CDecimal TAIL_WIDTH;   //尾部宽度
	CString JUDGE_CODE;   //判定结果
	CString ARCHIVE_CODE;   //缺陷代码

	void MergeFrom(CDataRow& row)
	{
		CDataTable& table = row.get_Table();
		if (table.Columns.Contains("STRAND_NO"))
		if (row["STRAND_NO"] != CDBNull::Value)
			this->STRAND_NO = (CDecimal)row["STRAND_NO"];
		if (table.Columns.Contains("MAT_NO"))
		if (row["MAT_NO"] != CDBNull::Value && (CString)row["MAT_NO"] != "")
			this->MAT_NO = (CString)row["MAT_NO"];
		if (table.Columns.Contains("LOC_CODE"))
		if (row["LOC_CODE"] != CDBNull::Value && (CString)row["LOC_CODE"] != "")
			this->LOC_CODE = (CString)row["LOC_CODE"];
		if (table.Columns.Contains("MAT_ACT_WT"))
		if (row["MAT_ACT_WT"] != CDBNull::Value)
			this->MAT_ACT_WT = (CDecimal)row["MAT_ACT_WT"];
		if (table.Columns.Contains("HEAD_WIDTH"))
		if (row["HEAD_WIDTH"] != CDBNull::Value)
			this->HEAD_WIDTH = (CDecimal)row["HEAD_WIDTH"];
		if (table.Columns.Contains("TAIL_WIDTH"))
		if (row["TAIL_WIDTH"] != CDBNull::Value)
			this->TAIL_WIDTH = (CDecimal)row["TAIL_WIDTH"];
		if (table.Columns.Contains("JUDGE_CODE"))
		if (row["JUDGE_CODE"] != CDBNull::Value && (CString)row["JUDGE_CODE"] != "")
			this->JUDGE_CODE = (CString)row["JUDGE_CODE"];
		if (table.Columns.Contains("ARCHIVE_CODE"))
		if (row["ARCHIVE_CODE"] != CDBNull::Value && (CString)row["ARCHIVE_CODE"] != "")
			this->ARCHIVE_CODE = (CString)row["ARCHIVE_CODE"];

	}
};

int f_wm00_stock_place_no(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量

	//定义实体类
	CX1J197B x1j197b;
	
	CDbCommand cmd_inq(conn);
	try
	{
		x1j197b.MergeFrom(bcls_rec->Tables[0].Rows[0]);//将电文内容放入结构体

		if (x1j197b.MAT_NO.GetLength() < 10)
		{
			s.flag = -1;
			strcpy(s.msg, "板坯号不正确！");
			return -1;
		}

		//查询静态表TQMTS9P：表检处理19
		//EXEC SQL DECLARE tqmts9p_q CURSOR FOR
		//	SELECT  *
		//	FROM  TQMTS9P
		//	WHERE SLAB_DEAL_TYPE = '19'
		//	ORDER  BY  REC_CREATE_TIME DESC
			;

		//20160913 laiwenbin 针对高低位做判断
		if (0 == strcmp(x1j197b.MAT_NO, "30000000000"))
		{
			bcls_rec->AddColName(1, "loc_code");
			bcls_rec->SetColVal(1, 1, "loc_code", x1j197b.LOC_CODE);
			doFlag = f_wm00_hposition(bcls_rec, bcls_ret, conn);
			if (doFlag != 0)
			{
				EDLog(1, 1, "f_ymsm_hposition处理失败.");
				doFlag = 0;
				//goto l_return;
			}
			else
			{
				strcpy(s.msg, "处理成功！");
				doFlag = 0;
				/*s.sqlcode = 0;
				goto l_return;*/
			}
		}
		else
		{
			//20170412   lwb  下发电文针对PONO进行截取
			bcls_rec->AddColName(1, "mat_no");
			bcls_rec->SetColVal(1, 1, "mat_no", x1j197b.mat_no);
			doFlag = f_pono_mat_cat(bcls_rec, bcls_ret);
			if (0 != doFlag)
			{
				EDLog(1, 1, "截位失败", s.msg);
				doFlag = -1;
				goto l_return;
			}
			bcls_ret->GetColVal(1, 2, "mat_no_af", tmmsm01.mat_no);

			EDLog(1, 1, "tmmsm01.mat_no = [%s]", tmmsm01.mat_no);

			if (0 == strcmp(x1j197b.loc_code, "49") || 0 == strcmp(x1j197b.loc_code, "48"))//20201119增加判定条件
			{
				EDLog(1, 1, "非4号机位置，直接退出.");
				doFlag = 0;
				goto l_return;
			}

			if (0 == strcmp(x1j197b.loc_code, "39"))//20180509增加在线宽度数据上传
			{
				EXEC SQL
					UPDATE TMMSM01
					SET DEFECT_WT = :x1j197b.head_width,
						SPARE_ITEM_N3 = : x1j197b.tail_width
						WHERE mat_no = : tmmsm01.mat_no AND mat_position = '1'
						;

				if (sqlca.sqlcode == M_NO_DATA_FOUND)
				{
					EDLog(1, 1, "处理失败，板坯号码不存在！");
					doFlag = 0;
					s.sqlcode = 0;
					goto l_return;
				}
				else
				{
					//增加对板坯宽度的处理（2019-6-13）
					//取宽度控制开关							
					EXEC SQL select * into :tmmsm01 from tmmsm01 where mat_no = : tmmsm01.mat_no and mat_position in('1', '2');
					if (sqlca.sqlcode == M_NO_DATA_FOUND)
					{
						EDLog(1, 1, "板坯不在炼钢范围内，不处置！");
						doFlag = 0;
						s.sqlcode = 0;
						goto l_return;
					}
					EXEC SQL select * into :tep0002 from tep0002 where code_class = 'YMWD' and code = : tmmsm01.strand_no;
					EDLog(1, 1, " **************%s  ----tmmsm01.mat_no *****************", tmmsm01.mat_no);
					EDLog(1, 1, " **************%s  ----tmmsm01.mat_position ***********", tmmsm01.mat_position);

					if (0 != strcmp(tep0002.code_desc_1_content, "OFF") && 0 == strcmp(tmmsm01.mat_position, "1") && 0 == strcmp(tmmsm01.slab_out_place, " "))//参与宽度控制标定
					{
						EDLog(1, 1, "执行宽度修订程序.板坯去向 = 【%s】", tmmsm01.mat_destion);

						//获取板坯宽度修正的拉速阈值
						strcpy(v_code_strand, tmmsm01.strand_no);
						if (0 == strcmp(tmmsm01.strand_no, "5") || 0 == strcmp(tmmsm01.strand_no, "6"))
						{
							strcat(v_code_strand, "A");
						}
						else if (0 == strcmp(tmmsm01.strand_no, "7") || 0 == strcmp(tmmsm01.strand_no, "8"))
						{
							strcat(v_code_strand, "B");
						}

						EXEC SQL
							select cast(COALESCE(CODE_DESC_3_CONTENT, '1000') as double) / 1000, cast(COALESCE(CODE_DESC_4_CONTENT, '1000') as double) / 1000
						into :d_spd_threshold_max, : d_spd_threshold_min
							  from tep0002
						where code_class = 'YMWD' and code = : v_code_strand;
						if (sqlca.sqlcode == M_NO_DATA_FOUND)
						{
							EDLog(1, 1, "拉速阈值查询出错！");
							d_spd_threshold_max = 1.30;
							d_spd_threshold_min = 1.0;
						}

						//读板坯的切割信息
						EXEC SQL
							SELECT cast_spd_max, cast_spd_min, slab_tapper_width_start
						INTO : f_cast_spd_max, : f_cast_spd_min, : f_slab_tapper_width_start
							   FROM tmmsm33
							   WHERE slab_no = : tmmsm01.mat_no;
						if (sqlca.sqlcode == M_NO_DATA_FOUND)
						{
							EDLog(1, 1, "切割表中无数据，采用默认数据！");
							f_cast_spd_max = atof(tep0002.code_desc_3_content);//最大拉速
							f_cast_spd_min = atof(tep0002.code_desc_3_content);//最小拉速
							f_slab_tapper_width_start = 0;//调宽开始点
							sprintf(tep0002.code_desc_3_content, "%.1f", 0.0);
						}
						else
						{
							sprintf(tep0002.code_desc_3_content, "%.1f", f_cast_spd_max);
						}

						//最大拉速校验
						if (f_cast_spd_max < 0.40)//二级数据错误
						{
							EDLog(1, 1, "板坯最大拉速 < 0.4,取前一块板坯最大拉速（%f）.", f_cast_spd_max);
							if (atof(tep0002.code_desc_3_content) > 0.4)
							{
								f_cast_spd_max = atof(tep0002.code_desc_3_content);//最大拉速
								f_cast_spd_min = atof(tep0002.code_desc_3_content);//最小拉速
							}
							else
							{
								f_cast_spd_max = 1.0;//最大拉速
								f_cast_spd_min = 1.0;//最小拉速
							}
						}

						f_cast_spd_diff = f_cast_spd_max - f_cast_spd_min;//拉速波动值

						//根据拉速确定收缩系数
						if (f_cast_spd_max >= 1.60 && f_cast_spd_min > 1.30)
						{
							v_shrinkage_coefficient = 1.012;
						}
						else if (f_cast_spd_max >= 1.30 && f_cast_spd_min > 1.0)
						{
							v_shrinkage_coefficient = 1.010;
						}
						else if (f_cast_spd_max >= 0.80)
						{
							v_shrinkage_coefficient = 1.008;
						}
						else
						{
							v_shrinkage_coefficient = 1.006;
						}

						EDLog(1, 1, "最大拉速:最小拉速:速度差:收缩系数:拉速阈值 =（ %.2f: %.2f: %.2f: %.3f: %.2f）", f_cast_spd_max, f_cast_spd_min, f_cast_spd_diff, v_shrinkage_coefficient, d_spd_threshold_max);

						flag_modify = 0;

						if (0 == strcmp(tmmsm01.mat_destion, "00"))//1热轧标准收严
						{
							v_width_diff_std_1 = 17;//板坯宽度修订标准上限
							v_width_diff_std_2 = -8;//板坯宽度修订标准下限
							v_width_diff_value_1 = 10;
							v_width_diff_value_2 = 5;
						}
						else//2热轧及外供标准放宽
						{
							v_width_diff_std_1 = 19;//板坯宽度修订标准上限
							v_width_diff_std_2 = -8;//板坯宽度修订标准下限
							v_width_diff_value_1 = 10;
							v_width_diff_value_2 = 5;
						}



						v_7b_head_width = round(x1j197b.head_width / v_shrinkage_coefficient);//测量头宽，收缩系数1.008
						v_7b_tail_width = round(x1j197b.tail_width / v_shrinkage_coefficient);//测量尾宽，收缩系数1.008

						v_slab_head_width = tmmsm01.slab_head_width;//头宽(信息)
						v_slab_tail_width = tmmsm01.slab_tail_width;//尾宽(信息)

						if (v_slab_head_width < 900 || v_slab_head_width > 1650)
						{
							v_slab_head_width = tmmsm01.mat_act_width;
						}
						if (v_slab_tail_width < 900 || v_slab_tail_width > 1650)
						{
							v_slab_tail_width = tmmsm01.mat_act_width;
						}
						//信息头尾宽差
						temp_slab_width_diff = v_slab_head_width - v_slab_tail_width;
						//取上块板坯尾部宽度
						v_slab_tail_width_forth = atoi(tep0002.code_desc_2_content);//取前一块板坯尾部测量宽度。
						v_tail_width_modify_diff_forth = atoi(tep0002.code_desc_4_content);//取前一块板坯尾部宽差修正值。					
						//去除无效数据
						if (v_slab_tail_width_forth < 880 || v_slab_tail_width_forth >1690)
						{
							EDLog(1, 1, "前一块板坯尾部宽度不正确，采用头部信息宽度");
							v_slab_tail_width_forth = v_slab_head_width;
						}

						v_slab_width_dif = v_slab_head_width - v_slab_tail_width;

						EDLog(1, 1, "测量宽度 =（ %d， %d）", v_7b_head_width, v_7b_tail_width);
						EDLog(1, 1, "信息宽度 =（ %d， %d）,宽差 = %d", v_slab_head_width, v_slab_tail_width, v_slab_width_dif);
						EDLog(1, 1, "前一块板坯尾部测量宽度 = %d.", v_slab_tail_width_forth);
						EDLog(1, 1, "宽度修订标志 = %s.", tep0002.code_desc_1_content);

						//判断是否需要对板坯宽度进行修正（需要二次处理的一般不进行修正，下线处理后测量）
						if (0 == strcmp(tmmsm01.defect_code_l_1, "2") || 0 == strcmp(tmmsm01.defect_code_l_1, "3") || 0 == strcmp(tmmsm01.defect_code_l_1, "6"))
						{
							//如果板坯需要更正切割、板坯纵切及二次横切则不对板坯宽度进行修正
							EDLog(1, 1, "板坯需要更正切割、板坯纵切及二次横切则不对板坯宽度进行修正！");
							flag_modify_enable = 0;
						}
						else if (0 == strcmp(tmmsm01.hand_way_code, "4") || 0 == strcmp(tmmsm01.hand_way_code, "5"))
						{
							//如果板坯需要角部加侧面扒皮清理、表面扒皮清理则不对板坯宽度进行修正
							EDLog(1, 1, "板坯需要角部加侧面扒皮清理、表面扒皮清理则不对板坯宽度进行修正！");
							flag_modify_enable = 0;
						}
						else if ((f_cast_spd_max <= d_spd_threshold_max && f_cast_spd_max >= d_spd_threshold_min) && f_cast_spd_diff < 0.21)
						{
							//拉速在这个范围不进行修订
							EDLog(1, 1, "拉速在正常范围不对板坯宽度进行修正！");
							flag_modify_enable = 0;
						}

						//测量数据有效性判断1：不可能存在的数据处理					
						//头部宽度校验
						if (v_7b_head_width > 1700 || v_7b_head_width < 880)
						{
							EDLog(1, 1, "头部宽度在允许范围外，认为检测错误，头宽默认为上块的尾宽！");
							v_7b_head_width = v_slab_tail_width_forth;
							temp_b_diff = v_7b_head_width - v_slab_head_width;//计算头部偏差
							flag_temp = 1;//宽度数据异常
						}

						//尾部宽度校验
						if (v_7b_tail_width > 1700 || v_7b_tail_width < 880)
						{
							//不可能存在的数据，用头部宽度数据加上信息宽度变化值
							EDLog(1, 1, "不可能存在的数据，尾部测量数据用头部宽度加上信息宽度变化值！");
							v_7b_tail_width = v_7b_head_width - (v_slab_head_width - v_slab_tail_width);//头宽默认为上块的尾宽
							temp_t_diff = v_7b_tail_width - v_slab_tail_width;//计算尾部偏差
							flag_temp = 1;//宽度数据异常
						}

						//头、尾测量值与信息值偏差计算
						temp_b_diff = v_7b_head_width - v_slab_head_width;//计算头部偏差
						temp_t_diff = v_7b_tail_width - v_slab_tail_width;//计算尾部偏差

						//调宽坯处理，按照小头的偏差值统一修订
						if (v_slab_width_dif >= 20)
							temp_b_diff = temp_t_diff - 4;//-3主要是为了考虑调宽部分的拉速要小一点
						else if (v_slab_width_dif <= -20)
							temp_t_diff = temp_b_diff - 4;//-3主要是为了考虑调宽部分的拉速要小一点

						EDLog(1, 1, "头尾偏差：%d:%d", temp_b_diff, temp_t_diff);
						//测量值的头尾宽差
						temp_7b_width_diff = v_7b_head_width - v_7b_tail_width;

						if ((temp_b_diff > 45 || temp_b_diff < -30) || (temp_t_diff > 45 || temp_t_diff < -30))
						{
							EDLog(1, 1, "宽度检测存在问题，用拉速修正！");
							//flag_modify_change = 1;//更改修正方式为拉速修订
							if (0 == strcmp(tep0002.code_desc_1_content, "10"))
								strcpy(tep0002.code_desc_1_content, "20");
							else if (0 == strcmp(tep0002.code_desc_1_content, "11"))
								strcpy(tep0002.code_desc_1_content, "21");
							flag_temp = 1;//宽度数据异常
						}

						/*
						if(flag_modify_change == 1 )
						{
						EDLog(1,1, "宽度检测存在问题，不修订，转根据拉速规则修订！");
						if(0 ==strcmp(tep0002.code_desc_1_content,"10"))
						strcpy(tep0002.code_desc_1_content,"20");
						else if(0 ==strcmp(tep0002.code_desc_1_content,"11"))
						strcpy(tep0002.code_desc_1_content,"21");
						}*/

						if (flag_modify_enable == 1 && (0 == strcmp(tep0002.code_desc_1_content, "10") || 0 == strcmp(tep0002.code_desc_1_content, "11")))//根据测量数据进行宽度修正
						{
							//宽度修订
							if (abs(temp_b_diff - temp_t_diff) <= 10)
							{
								int temp = 0;
								temp = round((temp_b_diff + temp_t_diff) / 2);
								if (temp > v_width_diff_std_1)
								{
									EDLog(1, 1, "超宽修订:temp = %d", temp);
									tmmsm01.slab_head_width = v_slab_head_width + temp - v_width_diff_value_1;
									tmmsm01.slab_tail_width = v_slab_tail_width + temp - v_width_diff_value_1;
									flag_modify = 1;
								}
								else if (temp < v_width_diff_std_2)
								{
									EDLog(1, 1, "超窄修订:temp = %d", temp);
									tmmsm01.slab_head_width = v_slab_head_width + temp + v_width_diff_value_2;
									tmmsm01.slab_tail_width = v_slab_tail_width + temp + v_width_diff_value_2;
									flag_modify = 1;
								}
								else
								{
									EDLog(1, 1, "平均宽差在允许范围，不对板坯宽度进行修正！");
								}
							}
							else
							{
								//EDLog(1,1, "宽度修订判断 ：B = %d,T = %d",v_7b_head_width,v_7b_tail_width);
								if (temp_b_diff > v_width_diff_std_1)//头超宽
								{
									EDLog(1, 1, "头超宽修订:temp_b_diff = %d", temp_b_diff);
									tmmsm01.slab_head_width = v_slab_head_width + temp_b_diff - v_width_diff_value_1;
									flag_modify = 1;
								}
								else if (temp_b_diff < v_width_diff_std_2)//头超窄
								{
									EDLog(1, 1, "头超窄修订:temp_b_diff = %d", temp_b_diff);
									tmmsm01.slab_head_width = v_slab_head_width + temp_b_diff + v_width_diff_value_2;
									flag_modify = 2;
								}
								if (temp_t_diff > v_width_diff_std_1)//尾超宽
								{
									EDLog(1, 1, "尾超宽修订:temp_t_diff = %d", temp_t_diff);
									tmmsm01.slab_tail_width = v_slab_tail_width + temp_t_diff - v_width_diff_value_1;
									flag_modify = 1;
								}
								else if (temp_t_diff < v_width_diff_std_2)//尾超窄
								{
									EDLog(1, 1, "尾超窄修订:temp_t_diff = %d", temp_t_diff);
									tmmsm01.slab_tail_width = v_slab_tail_width + temp_t_diff + v_width_diff_value_2;
									flag_modify = 2;
								}
							}
						}
						//else if(flag_modify_enable == 1 && 0 == strcmp(tmmsm01.mat_destion,"00") && (0 ==strcmp(tep0002.code_desc_1_content,"20") || 0 ==strcmp(tep0002.code_desc_1_content,"21")))//根据拉速进行修正（只针对去1热轧）
						else if (flag_modify_enable == 1 && (0 == strcmp(tep0002.code_desc_1_content, "20") || 0 == strcmp(tep0002.code_desc_1_content, "21")))//根据拉速进行修正
						{
							EDLog(1, 1, "根据拉速修正!");
							if (f_cast_spd_min > 1.19 && f_cast_spd_max > 1.29)
							{
								if (f_cast_spd_max > 1.59)
								{
									EDLog(1, 1, "超1.6拉速超宽修订 + 20mm");
									tmmsm01.slab_head_width = v_slab_head_width + 20;
									tmmsm01.slab_tail_width = v_slab_tail_width + 20;
									flag_modify = 1;
								}
								else if (f_cast_spd_max > 1.49)
								{
									EDLog(1, 1, "超1.5拉速超宽修订 + 15mm");
									tmmsm01.slab_head_width = v_slab_head_width + 15;
									tmmsm01.slab_tail_width = v_slab_tail_width + 15;
									flag_modify = 1;
								}
								else if (f_cast_spd_max > 1.39)
								{
									EDLog(1, 1, "超1.4拉速超宽修订 + 10mm");
									tmmsm01.slab_head_width = v_slab_head_width + 10;
									tmmsm01.slab_tail_width = v_slab_tail_width + 10;
									flag_modify = 1;
								}
								else
								{
									EDLog(1, 1, "拉速1.4以下不修订");
								}
							}
							else if (f_cast_spd_max < 0.99)
							{
								EDLog(1, 1, "低拉速超窄修订 - 10mm");
								tmmsm01.slab_head_width = v_slab_head_width - 10;
								tmmsm01.slab_tail_width = v_slab_tail_width - 10;
								flag_modify = 2;
							}
							v_7b_head_width = tmmsm01.slab_head_width;
							v_7b_tail_width = tmmsm01.slab_tail_width;
							if (0 == strcmp(tmmsm01.mat_destion, "00"))//1热轧标准收严
							{
								v_width_diff_std_1 = 10;
							}
							else
							{
								v_width_diff_std_1 = 15;
							}
						}

						//更新tmmsm01表宽度信息
						if (flag_modify == 1 || flag_modify == 2)
						{
							EDLog(1, 1, "记录板坯修正信息：%.0f:%.0f:%.0f", tmmsm01.mat_act_width, tmmsm01.slab_head_width, tmmsm01.slab_tail_width);
							sprintf(tmmsm01.rel_remark, "%.0f :%.0f :%.0f:%.1f", tmmsm01.mat_act_width, tmmsm01.slab_head_width, tmmsm01.slab_tail_width, f_cast_spd_max);

							//宽度处理
							if (tmmsm01.slab_head_width > 1650)
							{
								tmmsm01.slab_head_width = 1650;
							}
							if (tmmsm01.slab_tail_width > 1650)
							{
								tmmsm01.slab_tail_width = 1650;
							}
							tmmsm01.mat_act_width = f_ymsm_slab_width_round((tmmsm01.slab_head_width + tmmsm01.slab_tail_width) / 2);
							tmmsm01.slab_head_width = f_ymsm_slab_width_round(tmmsm01.slab_head_width);
							tmmsm01.slab_tail_width = f_ymsm_slab_width_round(tmmsm01.slab_tail_width);
							tmmsm01.slab_head_tail_width_diff = abs(tmmsm01.slab_head_width - tmmsm01.slab_tail_width);

							if (0 == strcmp(tep0002.code_desc_1_content, "20") || 0 == strcmp(tep0002.code_desc_1_content, "10"))
							{
								EDLog(1, 1, "只记录信息，不修改板坯宽度信息！");
								EXEC SQL
									UPDATE tmmsm01
									set rel_remark = :tmmsm01.rel_remark
								where mat_no = : tmmsm01.mat_no and mat_position in('1', '2');
							}
							else if (0 == strcmp(tep0002.code_desc_1_content, "21") || 0 == strcmp(tep0002.code_desc_1_content, "11"))
							{
								//宽度处理:如果没有超轧制允许最大宽度则直接修改为轧制允许最大宽度
								//if((0 ==strcmp(tmmsm01.mat_destion,"00")||0 ==strcmp(tmmsm01.mat_destion,"01")) && strlen(tmmsm01.prec_slab_no) > 5)
								if (strlen(tmmsm01.prec_slab_no) > 5)
								{
									EDLog(1, 1, "去向00、01有计划板坯，需要比对轧制上限！");
									if (tmmsm01.slab_head_width == tmmsm01.slab_tail_width)
									{
										if (tmmsm01.slab_head_width > tmmsm01.slab_width_max_nom && (v_7b_head_width <= (tmmsm01.slab_width_max_nom + v_width_diff_std_1) && v_7b_tail_width <= (tmmsm01.slab_width_max_nom + v_width_diff_std_1)))
										{
											tmmsm01.slab_head_width = int(tmmsm01.slab_width_max_nom);
											tmmsm01.slab_tail_width = int(tmmsm01.slab_width_max_nom);
											v_flag_modify_2 = 1;
											EDLog(1, 1, "等宽板坯，轧制上限满足修改范围，修正为%.0f！", tmmsm01.slab_width_max_nom);
										}
										else if (tmmsm01.slab_head_width < tmmsm01.slab_width_min_nom && (v_7b_head_width >= (tmmsm01.slab_width_min_nom - v_width_diff_std_2) && v_7b_tail_width >= (tmmsm01.slab_width_min_nom - v_width_diff_std_2)))
										{
											tmmsm01.slab_head_width = int(tmmsm01.slab_width_min_nom);
											tmmsm01.slab_tail_width = int(tmmsm01.slab_width_min_nom);
											v_flag_modify_2 = 1;
											EDLog(1, 1, "等宽板坯，轧制下限满足修改范围，修正为%.0f！", tmmsm01.slab_width_min_nom);
										}
									}
									else
									{
										//头部判断
										if (tmmsm01.slab_head_width > tmmsm01.slab_width_max_nom && v_7b_head_width <= (tmmsm01.slab_width_max_nom + v_width_diff_std_1))
										{
											tmmsm01.slab_head_width = int(tmmsm01.slab_width_max_nom);
											v_flag_modify_2 = 1;
											EDLog(1, 1, "头部宽度满足轧制上限要求，修正为%.0f！", tmmsm01.slab_head_width);
										}
										else if (tmmsm01.slab_head_width < tmmsm01.slab_width_min_nom && v_7b_head_width >= (tmmsm01.slab_width_min_nom - v_width_diff_std_2))
										{
											tmmsm01.slab_head_width = int(tmmsm01.slab_width_min_nom);
											v_flag_modify_2 = 1;
											EDLog(1, 1, "头部宽度满足轧制下限要求，修正为%.0f！", tmmsm01.slab_head_width);
										}
										//尾部判断
										if (tmmsm01.slab_tail_width > tmmsm01.slab_width_max_nom && v_7b_tail_width <= (tmmsm01.slab_width_max_nom + v_width_diff_std_1))
										{
											tmmsm01.slab_tail_width = int(tmmsm01.slab_width_max_nom);
											v_flag_modify_2 = 1;
											EDLog(1, 1, "尾部宽度满足轧制上限要求，修正为%.0f！", tmmsm01.slab_tail_width);
										}
										else if (tmmsm01.slab_head_width < tmmsm01.slab_width_min_nom && v_7b_tail_width >= (tmmsm01.slab_width_min_nom - v_width_diff_std_2))
										{
											tmmsm01.slab_tail_width = int(tmmsm01.slab_width_min_nom);
											v_flag_modify_2 = 1;
											EDLog(1, 1, "尾部宽度满足轧制下限要求，修正为%.0f！", tmmsm01.slab_tail_width);
										}
									}
									//二次修订标准宽度
									if (v_flag_modify_2 == 1)
									{
										tmmsm01.mat_act_width = int((tmmsm01.slab_head_width + tmmsm01.slab_tail_width) / 2);
										tmmsm01.slab_head_tail_width_diff = abs(tmmsm01.slab_head_width - tmmsm01.slab_tail_width);
									}
								}

								//重量计算
								bcls_rec->SetBlkName(1, "CSWT");
								bcls_rec->SetColVal("CSWT", 1, (T_INFO *)&tmmsm01_info);
								ret = f_ymCountSlabWeight(bcls_rec, bcls_ret);
								bcls_ret->GetColVal("CSWT", 1, "theory_wt", &tmmsm01.mat_theory_wt);
								bcls_ret->GetColVal("CSWT", 1, "theory_wt_act", &tmmsm01.mat_act_wt);
								//tmmsm01.mat_theory_wt = tmmsm01.mat_act_wt;

								//规格二次判定
								if (0 == strcmp(tmmsm01.surface_decide_code, "1") && (0 != strcmp(tmmsm01.hot_send_flag, "3") && 0 != strcmp(tmmsm01.hot_send_flag, "2")))
								{
									if (0 == strcmp(tmmsm01.mat_destion, "00"))
									{
										if ((tmmsm01.slab_head_width > 1320 || tmmsm01.slab_tail_width > 1320) || tmmsm01.slab_head_tail_width_diff > 34)
										{
											if ((5300 < tmmsm01.mat_act_len && 8000 > tmmsm01.mat_act_len) || 4500 > tmmsm01.mat_act_len)
											{
												strcpy(tmmsm01.surface_decide_code, "4");//封锁
												strcpy(tmmsm01.hold_flag, "1");//封锁标记
												strcpy(tmmsm01.finish_flag, "3");//需精整
												strcpy(tmmsm01.defect_code_l_1, "3");//板坯纵切
											}
											else
											{
												strcpy(tmmsm01.mat_destion, "01");
											}
										}

										if (tmmsm01.slab_head_tail_width_diff > 54)
										{
											strcpy(tmmsm01.surface_decide_code, "4");//封锁
											strcpy(tmmsm01.hold_flag, "1");//封锁标记
											strcpy(tmmsm01.finish_flag, "3");//需精整
											strcpy(tmmsm01.defect_code_l_1, "3");//板坯纵切
										}
									}
								}

								EXEC SQL
									UPDATE tmmsm01
									set rel_remark = :tmmsm01.rel_remark,
									mat_act_width = : tmmsm01.mat_act_width,
									slab_head_width = : tmmsm01.slab_head_width,
									slab_tail_width = : tmmsm01.slab_tail_width,
									slab_head_tail_width_diff = : tmmsm01.slab_head_tail_width_diff,
									mat_act_wt = : tmmsm01.mat_act_wt,
									mat_theory_wt = : tmmsm01.mat_theory_wt,
									mat_destion = : tmmsm01.mat_destion,
									surface_decide_code = : tmmsm01.surface_decide_code,
									hold_flag = : tmmsm01.hold_flag,
									finish_flag = : tmmsm01.finish_flag
								where mat_no = : tmmsm01.mat_no and mat_position in('1', '2');
							}
						}
						//记录板坯修正信息
						v_tail_width_modify_diff = tmmsm01.slab_tail_width - v_slab_tail_width;
						if (v_tail_width_modify_diff < 0)
							v_tail_width_modify_diff = 0;
						sprintf(tep0002.code_desc_2_content, "%d", v_7b_tail_width);
						sprintf(tep0002.code_desc_3_content, "%.1f", f_cast_spd_max);
						sprintf(tep0002.code_desc_4_content, "%d", v_tail_width_modify_diff);
						sprintf(tep0002.code_desc_5_content, "%d", f_slab_tapper_width_start);

						EDLog(1, 1, "记录%s流当前修正信息:%s  %s  %s", tmmsm01.strand_no, tep0002.code_desc_2_content, tep0002.code_desc_3_content, tep0002.code_desc_4_content);

						EXEC SQL
							UPDATE tep0002
							SET CODE_DESC_2_CONTENT = :tep0002.code_desc_2_content,
							CODE_DESC_3_CONTENT = : tep0002.code_desc_3_content,
							CODE_DESC_4_CONTENT = : tep0002.code_desc_4_content,
							CODE_DESC_5_CONTENT = : tep0002.code_desc_5_content
							WHERE CODE_CLASS = 'YMWD' and CODE = : tmmsm01.strand_no;

						EXEC SQL
							UPDATE tep0002_res
							SET CODE_DESC_2_CONTENT = :tep0002.code_desc_2_content,
							CODE_DESC_3_CONTENT = : tep0002.code_desc_3_content,
							CODE_DESC_4_CONTENT = : tep0002.code_desc_4_content,
							CODE_DESC_5_CONTENT = : tep0002.code_desc_5_content
							WHERE CODE_CLASS = 'YMWD' and CODE = : tmmsm01.strand_no and CULTURE = 'zh_Hans';
						if (sqlca.sqlcode == M_NO_DATA_FOUND)
						{
							EDLog(1, 1, "tep0002表没有更新成功！！");
						}

						//更新YMSM44表的质量标示QUAL_ID
						EXEC SQL
							UPDATE tymsm44
							SET qual_id = :tep0002.code_desc_4_content
							WHERE mat_no = : tmmsm01.mat_no and rest_roller_no not in('R1', 'R2');
					}
					//对封锁板坯及时下发下线命令
					if (strcmp(ptmmsm01->surface_decide_code, "4") == 0)
					{
						//调用函数
						bcls_rec_f.AddColName(1, "mat_no");
						bcls_rec_f.AddColName(1, "slab_path");
						bcls_rec_f.SetColVal(1, 1, "mat_no", ptmmsm01->mat_no);
						bcls_rec_f.SetColVal(1, 1, "slab_path", "9");
						bcls_rec_f.SetSYS(s);
						doFlag = f_ym191jd2_snd(&bcls_rec_f, &bcls_ret_f);
						if (doFlag != 0)
						{
							//bcls_ret_f.GetSYS(&s);
							EDLog(1, 1, "函数调用失败f_ym191jd2_snd() msg = [%s]", s.msg);
							//goto l_suberror;
						}
					}
					doFlag = 0;
					s.sqlcode = 0;
					goto l_return;
				}
			}
			else if (0 == strcmp(x1j197b.loc_code, "38"))//20191129增加表检数据处理
			{
				EXEC SQL
					UPDATE TMMSM01
					SET DEFECT_POSITION_L_3 = :x1j197b.judge_code,
					DEFECT_POSITION_L_4 = : x1j197b.archive_code/*,
																DEFECT_WT = :x1j197b.head_width,
																SPARE_ITEM_N3 =:x1j197b.tail_width*/
																WHERE mat_no = : tmmsm01.mat_no
																/*WHERE mat_no = :tmmsm01.mat_no AND mat_position ='1'*/
																;

				if (sqlca.sqlcode == M_NO_DATA_FOUND)
				{
					strcpy(s.msg, "处理失败，板坯号码不存在！");
					doFlag = 0;
					s.sqlcode = 0;
					goto l_return;
				}
				else
				{
					//增加对板坯宽度的处理（2019-6-13）
					//取宽度控制开关
					EXEC SQL select * into :tmmsm01 from tmmsm01 where mat_no = : tmmsm01.mat_no and mat_position  in('1', '2');
					if (sqlca.sqlcode == M_NO_DATA_FOUND)
					{
						EDLog(1, 1, "板坯不在炼钢范围内，不处置！");
						doFlag = 0;
						s.sqlcode = 0;
						goto l_return;
					}

					EDLog(1, 1, " **************%s  ----tmmsm01.mat_no *****************", tmmsm01.mat_no);
					EDLog(1, 1, " **************%s  ----tmmsm01.mat_position ***********", tmmsm01.mat_position);

					EDLog(1, 1, "进入表检处置！judge_code =【%s】，archive_code =【%s】", x1j197b.judge_code, x1j197b.archive_code);
					//增加对缺陷的自动处置流程（通过调用静态表配置文件实现）
					//从代码表中取YMHP中系统通道开关来决定是否执行
					if (0 != strcmp(x1j197b.judge_code, " ") && 0 != strcmp(x1j197b.judge_code, "01") && 0 != strcmp(x1j197b.archive_code, " "))
					{
						EXEC SQL select * into :tep0002 from tep0002 where code_class = 'YMHP' and code = '09';
						if (0 == strcmp(tep0002.code_desc_4_content, "ON"))
						{
							//EDLog(1,1, "进入表检处置！judge_code =【%s】，archive_code =【%s】",x1j197b.judge_code,x1j197b.archive_code);
							//EXEC SQL select * into :tmmsm01 from tmmsm01 where mat_no =:tmmsm01.mat_no and mat_position in ('1','2');						
							if (sqlca.sqlcode != M_NO_DATA_FOUND)
							{
								if (0 == strcmp(tmmsm01.hot_send_flag, "3") || 0 == strcmp(tmmsm01.hot_send_flag, "2"))
								{
									strcpy(s.msg, "强制热送板坯，不处理！");
									doFlag = 0;
									s.sqlcode = 0;
									goto l_return;
								}
								//将查询结果赋值给指针
								ptmmsm01 = &tmmsm01;
								EXEC SQL OPEN tqmts9p_q;
								for (;;)
								{
									EXEC SQL FETCH tqmts9p_q INTO : tqmts9p;
									if (sqlca.sqlcode == M_NO_DATA_FOUND)
									{
										find_flag = 0;
										break;
									}
									if (0 == strcmp(ptmmsm01->prec_st_no, tqmts9p.st_no) && 0 == strcmp(x1j197b.archive_code, tqmts9p.slab_deal_flag) && 0 == strcmp(x1j197b.judge_code, tqmts9p.ass_dif_code))
									{
										//出钢记号满足、缺陷代码满足、判定结果满足
										find_flag = 1;
										break;
									}
									else continue;
								}
								EXEC SQL CLOSE tqmts9p_q;

								if (!find_flag)
								{
									EXEC SQL OPEN tqmts9p_q;
									for (;;)
									{
										EXEC SQL FETCH tqmts9p_q INTO : tqmts9p;
										if (sqlca.sqlcode == M_NO_DATA_FOUND)
										{
											find_flag = 0;
											break;
										}
										if (0 == strcmp("XX", tqmts9p.st_no) && 0 == strcmp(x1j197b.archive_code, tqmts9p.slab_deal_flag) && 0 == strcmp(x1j197b.judge_code, tqmts9p.ass_dif_code))
										{
											//出钢记号满足、缺陷代码满足、判定结果满足
											find_flag = 1;
											break;
										}
										else continue;
									}
									EXEC SQL CLOSE tqmts9p_q;
								}

								//数据处理
								if (find_flag)
								{
									doFlag = f_mmsm01_data_proc(ptmmsm01, &tqmts9p, bcls_ret);

									if (0 == doFlag)
									{
										//更新板坯信息
										blck_mat_upd.SetBlkName(1, "MMSM01");
										blck_mat_upd.AddColName(1, "event_id");
										blck_mat_upd.SetColVal(1, 1, (T_INFO *)&tmmsm01_info);
										blck_mat_upd.SetColVal(1, 1, "event_id", "MM02");
										doFlag = f_mmsm01_event(&blck_mat_upd, bcls_ret);
										if (0 != doFlag)
										{
											bcls_ret->GetSYS(&s);
											strcpy(s.msg, "调用精整板坯事件函数f_mmsm01_event失败:s.msg=[%s]");
											EDLog(1, 1, "调用精整板坯事件函数f_mmsm01_event失败:s.msg=[%s]", s.msg);
											goto l_apperror;
										}
									}
								}
							}
						}
					}

					strcpy(s.msg, "处理成功！");
					doFlag = 0;
					s.sqlcode = 0;
					goto l_return;
				}
			}

			EXEC SQL select * into :tmmsm01 from tmmsm01 where mat_no = : tmmsm01.mat_no and mat_position = '1';
			EDLog(1, 1, " **************%s  ----tmmsm01.mat_no *****************", tmmsm01.mat_no);
			EDLog(1, 1, " **************%s  ----tmmsm01.mat_position *****************", tmmsm01.mat_position);


			temp_slab_wt = x1j197b.mat_act_wt;//为保证传入到板坯物流控制函数的正确性先将值取出
			sprintf(tymsm44.rest_roller_no, "%s", x1j197b.loc_code);
			if (strcmp(tmmsm01.mat_position, "1") == 0)
			{
				strcpy(tymsm44.mat_no, tmmsm01.mat_no);
				//EDLog(1, 1, " **************%s  ----tmmsm01.mat_no *****************",tymsm44.mat_no);

				EXEC SQL select rest_roller_no into : v_rest_roller_no from tymsm44 where mat_no = : tymsm44.mat_no;

				if (strcmp(v_rest_roller_no, "R1") == 0)
				{
					// strcpy(s.msg,"板坯已经送热轧！");
					doFlag = 0;
					s.sqlcode = 0;
					goto l_return;
				}

				EXEC SQL select * into :tep0002 from tep0002 where code_class = 'YMHP' and code = : tymsm44.rest_roller_no;
				strcpy(tymsm44.rest_roller_no, tep0002.code_desc_1_content);
				strcpy(v_on_off_flag, tep0002.code_desc_2_content);
				if (strcmp(tep0002.code_desc_1_content, "601") == 0 || strcmp(tep0002.code_desc_1_content, "602") == 0)
				{
					bcls_rec->AddColName(1, "mat_no");
					bcls_rec->AddColName(1, "slab_origion");
					bcls_rec->AddColName(1, "stock_place_no");
					bcls_rec->AddColName(1, "layerno");
					EPCutStrZ(x1j197b.mat_no, 1, 1, v_slab_origion);
					//EDLog(1,1, "newbk=%s",newbk);
					bcls_rec->SetColVal(1, 1, "mat_no", tmmsm01.mat_no);
					bcls_rec->SetColVal(1, 1, "slab_origion", v_slab_origion);
					bcls_rec->SetColVal(1, 1, "stock_place_no", tep0002.code_desc_1_content);
					if (strcmp(x1j197b.loc_code, "05") == 0)
					{
						bcls_rec->SetColVal(1, 1, "layerno", "01");
					}
					else if (strcmp(x1j197b.loc_code, "06") == 0)
					{
						bcls_rec->SetColVal(1, 1, "layerno", "02");
					}
					else if (strcmp(x1j197b.loc_code, "07") == 0)
					{
						bcls_rec->SetColVal(1, 1, "layerno", "03");
					}

					bcls_rec->SetSYS(s);
					EDLog(1, 1, "x1j197b.mat_no=%s", tmmsm01.mat_no);
					EDLog(1, 1, "v_slab_origion=%s", v_slab_origion);
					doFlag = f_ymsm_inyard(bcls_rec, bcls_ret);

					if (doFlag != 0)
					{
						EDLog(1, 1, "f_ymsm_inyard发送失败.");
						doFlag = -1;
						goto l_return;
					}
					else
					{
						strcpy(s.msg, "处理成功！");
						doFlag = 0;
						s.sqlcode = 0;
						goto l_return;
					}
				}
				EDLog(1, 1, "tymsm44.rest_roller_no=%s", tymsm44.rest_roller_no);

				strcpy(v_location, "A1");

				EXEC SQL select count(1) into :v_num from tymsm44 where mat_no = : tmmsm01.mat_no;

				if (v_num == 1)
				{
					EXEC SQL
						update tymsm44
						set rest_roller_no = :tymsm44.rest_roller_no
					where mat_no = : tmmsm01.mat_no;
				}
				else if (v_num == 0)
				{
					EXEC SQL
						insert into tymsm44
						(REST_ROLLER_NO, MAT_NO)
						values(:tymsm44.rest_roller_no, : tmmsm01.mat_no);
				}

				bcls_rec->SetBlkName(1, "MMSM01");
				bcls_rec->AddColName(1, "mat_no");
				bcls_rec->SetColVal("MMSM01", 1, "mat_no", tmmsm01.mat_no);
				bcls_rec->SetSYS(s);

				if (0 == strcmp(tymsm44.rest_roller_no, "A3") && strcmp(tmmsm01.surface_decide_code, "1") == 0 && (0 == x1j197b.mat_act_wt || 5001 == x1j197b.mat_act_wt))
				{
					EXEC SQL
						select substr(rest_roller_no, 1, 1)
					into :v_location
						  from tymsm44
					where mat_no = : tmmsm01.mat_no
					;

					if (5001 == x1j197b.mat_act_wt)//理论计重
					{
						strcpy(v_location, "A0");//秤需要理论计重
					}
					else
					{
						strcpy(v_location, "A1");//秤需要称重
					}
					doFlag = f_ym193101_snd(tmmsm01.mat_no, "1", v_location);

					if (doFlag != 0)
					{
						EDLog(1, 1, "ym193001发送失败.");
						//s.flag =-1;
						//goto l_return;
					}
				}
				//if(strcmp(tmmsm01.surface_decide_code,"4") == 0 || strcmp(tmmsm01.mat_destion,"01")!=0)
				if (strcmp(tmmsm01.surface_decide_code, "4") == 0 && 0 == strcmp(tep0002.code_desc_3_content, "ON"))
				{
					//调用函数
					bcls_rec_f.AddColName(1, "mat_no");
					bcls_rec_f.AddColName(1, "slab_path");
					bcls_rec_f.SetColVal(1, 1, "mat_no", tmmsm01.mat_no);
					bcls_rec_f.SetColVal(1, 1, "slab_path", "9");
					bcls_rec_f.SetSYS(s);
					doFlag = f_ym191jd2_snd(&bcls_rec_f, &bcls_ret_f);
					if (doFlag != 0)
					{
						//bcls_ret_f.GetSYS(&s);
						EDLog(1, 1, "函数调用失败f_ym191jd2_snd() msg = [%s]", s.msg);
						//goto l_suberror;
					}
				}
				else if (0 == strcmp(tep0002.code_desc_2_content, "ON") && strcmp(tmmsm01.mat_destion, "00") == 0)
				{
					//调用函数
					bcls_rec_f.AddColName(1, "mat_no");
					bcls_rec_f.AddColName(1, "slab_path");
					bcls_rec_f.SetColVal(1, 1, "mat_no", tmmsm01.mat_no);
					bcls_rec_f.SetColVal(1, 1, "slab_path", "9");
					bcls_rec_f.SetSYS(s);
					doFlag = f_ym191jd2_snd(&bcls_rec_f, &bcls_ret_f);
					if (doFlag != 0)
					{
						//bcls_ret_f.GetSYS(&s);
						EDLog(1, 1, "函数调用失败f_ym191jd2_snd() msg = [%s]", s.msg);
						//goto l_suberror;
					}
				}
				if (strcmp(tymsm44.rest_roller_no, "A3") == 0 && x1j197b.mat_act_wt>0 && 0 == strcmp(tep0002.code_desc_4_content, "OFF"))
				{
					EDLog(1, 1, "重量来自L1采集系统");
					if (abs(tmmsm01.mat_theory_wt - x1j197b.mat_act_wt) / tmmsm01.mat_theory_wt * 100 > 3.0 && x1j197b.mat_act_wt != 5001)
					{
						EXEC SQL
							UPDATE tmmsm01
							set rel_remark = '板坯理论重量与实际重量大于3%！请认真核对(1j197b)'
							WHERE mat_no = :tmmsm01.mat_no and mat_position = '1'
							;

						sprintf(s.msg, "板坯理论重量与实际重量大于3%！");

						/* 板坯实际重量不满足条件，称重标识为0 */
						EXEC SQL
							UPDATE TYMSM44 SET AUTO_WT_FLAG = '0' WHERE MAT_NO = :tmmsm01.mat_no;

						doFlag = 0;
						goto l_return;
					}

					if (strcmp(tmmsm01.surface_decide_code, "1") != 0)
					{
						sprintf(s.msg, "板坯表面判定不合格或者封锁的！");
						doFlag = 0;
						goto l_return;
					}
					/* 板坯实际重量满足条件，称重标识为1 */
					EXEC SQL
						UPDATE TYMSM44 SET AUTO_WT_FLAG = '1' WHERE MAT_NO = :tmmsm01.mat_no;

					strcpy(v_send_point, "1");
					strcpy(v_sort, "1");
					if (strcmp(tmmsm01.mat_position, "1") == 0)
					{
						strcpy(v_sort, "1");
						//strcpy(tmmsm01.hot_charge_flag,"1");
					}
					else
					{
						strcpy(v_sort, "0");
						//strcpy(tmmsm01.hot_charge_flag,"0");
					}
					//wcx20130709
					strcpy(wt_flag, "2");
					if (x1j197b.mat_act_wt == 5001)
					{
						x1j197b.mat_act_wt = tmmsm01.mat_theory_wt;
						strcpy(wt_flag, "0");
					}

					//获取板坯重量修正系数
					slab_wtadj = 1;
					v_slab_tapper_width_lmt = 0;
					EXEC SQL
						SELECT  nvl(SLAB_WTADJ_FACTOR, 1), nvl(SLAB_TAPPER_WIDTH_LMT, 0)
					INTO  :slab_wtadj, : v_slab_tapper_width_lmt
						   FROM  TQMTS9M
						   WHERE  STRAND_NO = 'J'
						   FETCH  FIRST 1 ROWS ONLY
						   ;
					//slab_wt = int(x1j197b.mat_act_wt * slab_wtadj);
					//增加送热轧自动处置功能 20220122:zhenglei
					if (v_slab_tapper_width_lmt <= 0)
					{
						slab_wtadj = 1;
					}

					if (slab_wtadj > 1.1 or slab_wtadj < 0.9)
						slab_wtadj = 1.0;

					slab_wt = int(x1j197b.mat_act_wt * slab_wtadj);
					v_slab_tapper_width_lmt = v_slab_tapper_width_lmt - abs(x1j197b.mat_act_wt - slab_wt) / 1000;

					if (v_slab_tapper_width_lmt <= 0)
					{
						v_slab_tapper_width_lmt = 0;
						slab_wtadj = 1;
					}

					//ZXL20120618更新(未精整前)板坯切断重量
					EXEC SQL
						UPDATE TMMSM33
						SET SLAB_WT = :slab_wt
						WHERE SLAB_NO = : tmmsm01.mat_no
						AND  NOT EXISTS(SELECT MAT_NO FROM TMMSM34 WHERE MAT_NO = :tmmsm01.mat_no)
						;
					//更新板坯重量
					EXEC SQL
						UPDATE tmmsm01
						set /*SCRAP_REMARK = to_char(:x1j197b.mat_act_wt),*/
						MAT_ACT_WT = :slab_wt,
						WT_MODE = : wt_flag,
						SLAB_OUT_PLACE = : v_send_point,
						SLAT_UNLADE_CAUSE = '90',
						OUT_STOCK_TIME = : v_create_time
					where mat_no = : tmmsm01.mat_no and mat_position = '1'
						;
					//更新维护表
					EXEC SQL
						UPDATE TQMTS9M
						set SLAB_TAPPER_WIDTH_LMT = :v_slab_tapper_width_lmt,
						SLAB_WTADJ_FACTOR = : slab_wtadj
					where STRAND_NO = 'J'
						;

					bcls_rec->AddColName(1, "mat_no");
					bcls_rec->AddColName(1, "send_point");
					bcls_rec->AddColName(1, "sort");
					bcls_rec->AddColName(1, "end_flag");
					bcls_rec->AddColName(1, "order_no");
					bcls_rec->AddColName(1, "hot_charge_flag");
					//EDLog(1,1, "newbk=%s",newbk);
					bcls_rec->SetColVal(1, 1, "mat_no", tmmsm01.mat_no);
					bcls_rec->SetColVal(1, 1, "send_point", v_send_point);
					bcls_rec->SetColVal(1, 1, "sort", v_sort);
					bcls_rec->SetColVal(1, 1, "end_flag", "X");
					bcls_rec->SetColVal(1, 1, "order_no", tmmsm01.order_no);
					bcls_rec->SetColVal(1, 1, "hot_charge_flag", tmmsm01.hot_charge_flag);

					doFlag = f_ym192906_snd(bcls_rec, bcls_ret);
					if (doFlag != 0)
					{
						EDLog(1, 1, "192906发送失败.");
						doFlag = -1;
						goto l_return;
					}

					doFlag = f_ym1900c9_snd(tmmsm01.mat_no, "1");
					if (doFlag != 0)
					{
						EDLog(1, 1, "1900c9发送失败.");
						doFlag = -1;
						goto l_return;
					}

					doFlag = f_ym1900d4_snd(tmmsm01.mat_no);

					if (doFlag != 0)
					{
						EDLog(1, 1, "1900d4发送失败.");
						doFlag = -1;
						goto l_return;
					}
					/*//计量系统改造要求
					//EXEC SQL select * into :tmmsm01 from tmmsm01 where mat_no=:tmmsm01.mat_no;
					if(tmmsm01.mat_theory_wt != tmmsm01.mat_act_wt)
					{
					doFlag = f_ym193101_snd(tmmsm01.mat_no,"1","A");
					if(doFlag != 0)
					{
					EDLog(1,1, "ym193001发送失败.");
					s.flag =-1;
					goto l_return;
					}
					}
					else
					{
					doFlag = f_ym193101_snd(tmmsm01.mat_no,"2","A");

					if(doFlag != 0)
					{
					EDLog(1,1, "ym193001发送失败.");
					s.flag =-1;
					goto l_return;
					}
					}
					*/
					//LWB20140424 TYMSM44已经有板坯信息，不进行删除，否则已有的其他信息将被删除   
					/*EXEC SQL delete from tymsm44 where mat_no =:tmmsm01.mat_no;*/

					EXEC SQL UPDATE tymsm44 SET REST_ROLLER_NO = 'R1' WHERE MAT_NO = :tmmsm01.mat_no;
				}
				else if (strcmp(tymsm44.rest_roller_no, "A3") == 0 && x1j197b.mat_act_wt>8000 && 0 == strcmp(tep0002.code_desc_4_content, "ON"))
				{
					EDLog(1, 1, "重量来自计量系统");
					EXEC SQL
						UPDATE tmmsm01
						set SCRAP_REMARK = to_char(:x1j197b.mat_act_wt)
					where mat_no = :tmmsm01.mat_no and mat_position = '1'
						;
				}

				if (0 == strcmp(tep0002.code_desc_5_content, "ON"))
				{
					bcls_rec->AddColName(1, "loc_code");
					bcls_rec->AddColName(1, "mat_no");
					bcls_rec->AddColName(1, "mat_act_wt");
					bcls_rec->SetColVal(1, 1, "loc_code", x1j197b.loc_code);
					bcls_rec->SetColVal(1, 1, "mat_no", tmmsm01.mat_no);
					bcls_rec->SetColVal(1, 1, "mat_act_wt", temp_slab_wt);
					EDLog(1, 1, "temp_slab_wt= [%f]", temp_slab_wt);
					doFlag = f_ymsm_ctrl_slab(bcls_rec, bcls_ret);
					if (0 != doFlag)
					{
						EDLog(1, 1, "f_ymsm_ctrl_slab调用板坯物流控制函数错误.");
						//goto l_return;
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


