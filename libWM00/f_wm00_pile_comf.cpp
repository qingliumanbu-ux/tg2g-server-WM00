/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      172104
Version:     1.0
Date:        2023-05-10 15:06:52
Description: 炼钢板坯垛位确定
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

/* ***** 外部函数申明 ***** */
int f_wm00_pile_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_wm00_pile_comf(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString  dateNow14 = CDateTime::Now().ToString("yyyyMMddHHmmss");  //14位日期变量
	CString	RetFlag = " ";
	int		blckNum = 0;
	int	days_diffx;
	int	hours_diffx;
	int	minutes_diffx;
	int	seconds_diffx;
	int		slab_cold_time = 0;
	int		flag = 0;
	CString	v_a_flag = " ";//推荐标志，0：正常推荐，1:扩展推荐
	CString	v_pos = "000";//传入参数。该垛位不能使用

	int v_num = 0;
	CString v_mat_no = " ";
	CString v_stock_place_no = " ";
	int v_circle = 0;//主分区值
	int v_circle_1 = 0;//辅助分区值
	double w_max = 0.0;
	double w_min = 0.0;
	double l_max = 0.0;
	double l_min = 0.0;
	CString v_cut_time_min = " ";
	int v_max_cold_time = 0;
	//定义实体类


	CDbCommand cmd_inq(conn);
	EIClass bcls_temp;

	//
	EIClass bcls_pile_jud;
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "MAT_NO");//材料号
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");//
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "Y");//
	bcls_pile_jud.Tables[0].Columns.Add(DT_STRING, "SLAB_PILE_VALUE");//
	bcls_pile_jud.Tables[0].Rows.Add();
	try
	{
		v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"];//板坯号
		v_circle = bcls_rec->Tables[0].Rows[0]["A"];//分区属性
		v_a_flag = bcls_rec->Tables[0].Rows[0]["A_FLAG"];//标志位
		v_pos = bcls_rec->Tables[0].Rows[0]["POS"];//不推荐垛位

		//EDLog(1, 1, "***(第三层)非扩展垛位判断开始***");

		//计算辅助分区值
		v_circle_1 = int(v_circle / 10) * 10;


	
		Log::Debug("", __FUNCTION__, "垛位推荐开始（第三层）");

		Log::Debug("", __FUNCTION__, "***D1**18小时内使用垛位集推荐开始**D1***");
		sqlstr = "SELECT STOCK_PLACE_NO FROM TWM04 WHERE stock_no  = 'SA4' AND PILE_MAIN_NO = @v_circle "
			" AND(PILE_MAT_NUM_ACT + PRE_MAT_NUM) < MAX_HEIGHT AND MAX_HEIGHT > 0 AND STOCK_STATUS = '0'"
			" AND timestampdiff(4, char(current timestamp - timestamp(FIELDNO_UPTIME))) <= 24 * 60 "/*原程序current timestamp - TO_DATE(FIELDNO_UPTIME,'YYYY-MM-DD HH24:MI:SS') <= 24*60*60*/
			" ORDER BY FIELDNO_UPTIME DESC, PILE_FIELDNO_NO ASC ";
		cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
		Log::Trace("", __FUNCTION__, "v_circle = [{0}]", v_circle);
		cmd_inq.Parameters.Set("v_circle", v_circle);
		int count=cmd_inq.ExecuteQuery(bcls_temp.Tables[0]);
		for (int i = 0; i < count;i++)
		{
			v_stock_place_no = bcls_temp.Tables[0].Rows[i]["STOCK_PLACE_NO"];

			if (i>100){
				Log::Debug("", __FUNCTION__, "24小时内使用过垛位集循环推荐结束，未获取到有效垛位(D1)");
				flag = 0;
				break;
			}

			if (v_stock_place_no == v_pos)
			{
				Log::Debug("", __FUNCTION__, "该垛位是传人垛位，不满足要求 = [{0}]", v_pos);
			}
			else
			{
				Log::Debug("", __FUNCTION__, "推荐垛位为 = [{0}]", v_stock_place_no);
				bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
				bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_stock_place_no;
				bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
				bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = v_circle;
				Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------开始111");
				doFlag = f_wm00_pile_jud(&bcls_pile_jud, bcls_ret, conn);
				Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------结束111");
				if (0 != doFlag)
				{
					Log::Debug("", __FUNCTION__, "***(第三层)推荐垛位板坯过程调用失败***");
					strcpy(s.msg, "***(第三层)推荐垛位板坯过程调用失败***");
					s.flag = -1;
					return -1;
				}
				Log::Debug("", __FUNCTION__, "***(第三层)推荐垛位板坯过程调用成功***");
				RetFlag = bcls_ret->Tables[0].Rows[0]["RETFLAG"];

				if (0 == strcmp(RetFlag, "Y"))
				{
					Log::Debug("", __FUNCTION__, "D1推荐垛位满足要求，推荐成功");
					flag = 1;
					break;
				}
				else
				{
					flag = 0;
					Log::Debug("", __FUNCTION__, "D1推荐垛位不满足要求，继续推荐");
				}
			}
		}

		//第一次推荐失败，寻找18小时以外没有推荐过的空垛位。

		if (flag == 0)//主属性空垛位推荐
		{
			Log::Debug("", __FUNCTION__, "***D2**未使用的空垛位集推荐开始**D2***");
			
			sqlstr = "	SELECT STOCK_PLACE_NO FROM TWM04 WHERE stock_no  = 'SA4' AND PILE_MAIN_NO = @v_circle AND PILE_MAT_NUM_ACT = 0 AND STOCK_STATUS = '0'"
				" AND MAX_HEIGHT > 0 ORDER BY PILE_FIELDNO_NO ASC";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			Log::Trace("", __FUNCTION__, "v_circle = [{0}]", v_circle);
			cmd_inq.Parameters.Set("v_circle", v_circle);
			int count2 = cmd_inq.ExecuteQuery(bcls_temp.Tables[0]);
			if (count2==0){
				v_stock_place_no = "ZZZ";
			}
			for (int i = 0; i < count2; i++)
			{
				v_stock_place_no = " ";
				v_stock_place_no = bcls_temp.Tables[0].Rows[i]["STOCK_PLACE_NO"];
				if (v_pos==v_stock_place_no)
				{
					Log::Debug("", __FUNCTION__, "该垛位是传人垛位，不满足要求 = [{0}]", v_pos);
					//EDLog(1, 1, "该垛位是传人垛位，不满足要求=【%s】", v_pos);
				}
				else
				{
					Log::Debug("", __FUNCTION__, "推荐垛位为 = [{0}]", v_stock_place_no);
					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_stock_place_no;
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = v_circle;
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------开始222");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, bcls_ret, conn);
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud-----------------------------结束222");
					if (0 != doFlag)
					{
						Log::Debug("", __FUNCTION__, "***(第三层)推荐垛位板坯过程调用失败***");
						strcpy(s.msg, "***(第三层)推荐垛位板坯过程调用失败***");
						s.flag = -1;
						return -1;
					}
					RetFlag = bcls_ret->Tables[0].Rows[0]["RETFLAG"];

					if (0 == strcmp(RetFlag, "Y"))
					{
						Log::Debug("", __FUNCTION__, "D1推荐垛位满足要求，推荐成功");
						flag = 1;
						break;
					}
					else
					{
						flag = 0;
						Log::Debug("", __FUNCTION__, "D1推荐垛位不满足要求，继续推荐");
					}
				}
			}
		}

		if (flag == 0)//扩展属性空垛位推荐
		{
			Log::Debug("", __FUNCTION__, "***D4**扩展属性空垛位集推荐开始**D4***");
			sqlstr = "	SELECT STOCK_PLACE_NO FROM twm04 WHERE stock_no  = 'SA4' AND pile_assis_no = @v_circle_1 AND(PILE_MAT_NUM_ACT + pre_mat_num) = 0"
				" AND MAX_HEIGHT > 0 AND STOCK_STATUS = '0' AND store_area = '1' AND PILE_MAIN_NO <> @v_circle ORDER BY pile_fieldno_no2 DESC";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			Log::Trace("", __FUNCTION__, "v_circle = [{0}]", v_circle);
			Log::Trace("", __FUNCTION__, "v_circle_1 = [{0}]", v_circle_1);
			cmd_inq.Parameters.Set("v_circle", v_circle);
			cmd_inq.Parameters.Set("v_circle_1", v_circle_1);
			int count4 = cmd_inq.ExecuteQuery(bcls_temp.Tables[0]);
			for (int i = 0; i < count4; i++)
			{
				v_stock_place_no = " ";
				v_stock_place_no = bcls_temp.Tables[0].Rows[i]["STOCK_PLACE_NO"];

				if (v_pos==v_stock_place_no)
				{
					Log::Debug("", __FUNCTION__, "该垛位是传人垛位，不满足要求 = [{0}]", v_pos);
				}
				else
				{
					Log::Debug("", __FUNCTION__, "推荐垛位为 = [{0}]", v_stock_place_no);
					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_stock_place_no;
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = v_circle;
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------开始333");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, bcls_ret, conn);
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------结束333");
					if (0 != doFlag)
					{
						Log::Debug("", __FUNCTION__, "***(第三层)推荐垛位板坯过程调用失败***");
						strcpy(s.msg, "***(第三层)推荐垛位板坯过程调用失败***");
						s.flag = -1;
						return -1;
					}
					RetFlag = bcls_ret->Tables[0].Rows[0]["RETFLAG"];

					if (0 == strcmp(RetFlag, "Y"))
					{
						Log::Debug("", __FUNCTION__, "D1推荐垛位满足要求，推荐成功");
						flag = 1;
						break;
					}
					else
					{
						flag = 0;
						Log::Debug("", __FUNCTION__, "D1推荐垛位不满足要求，继续推荐");
					}
				}
			}
		}


		if (flag == 0)//扩展属性非空垛位推荐（最好不要进行推荐）
		{
			Log::Debug("", __FUNCTION__, "***D2**未使用的空垛位集推荐开始**D2***");

			sqlstr = "SELECT STOCK_PLACE_NO FROM twm04 WHERE	stock_no  = 'SA4' AND PILE_ASSIS_NO = @v_circle_1 AND PILE_MAT_NUM_ACT > 0 "
				" AND PILE_MAT_NUM_ACT < MAX_HEIGHT AND MAX_HEIGHT > 0 AND PRE_MAT_NUM = 0"
				" AND STOCK_STATUS = '0' AND STORE_AREA = '1' AND FIELDNO = '9' AND PILE_MAIN_NO <> @v_circle "
				" AND timestampdiff(4, char(current timestamp - timestamp(FIELDNO_UPTIME))) <= 24 * 60"
				" ORDER BY PILE_MAT_NUM_ACT DESC";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.Parameters.Set("v_circle", v_circle);
			cmd_inq.Parameters.Set("v_circle_1", v_circle_1);
			int count5 = cmd_inq.ExecuteQuery(bcls_temp.Tables[0]);
			if (count5 == 0){
				v_stock_place_no = "ZZZ";
			}
			for (int i = 0; i < count5; i++)
			{
				v_stock_place_no = " ";
				v_stock_place_no = bcls_temp.Tables[0].Rows[i]["STOCK_PLACE_NO"];

				if (v_pos == v_stock_place_no)
				{
					Log::Debug("", __FUNCTION__, "该垛位是传人垛位，不满足要求 = [{0}]", v_pos);
					//EDLog(1, 1, "该垛位是传人垛位，不满足要求=【%s】", v_pos);
				}
				else
				{
					Log::Debug("", __FUNCTION__, "D5推荐垛位为 = [{0}]", v_stock_place_no);
					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_stock_place_no;
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = v_circle;
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------开始444");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, bcls_ret, conn);
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------结束444");
					if (0 != doFlag)
					{
						strcpy(s.msg, "***(第三层)推荐垛位板坯过程调用失败***");
						s.flag = -1;
						return -1;
					}

					RetFlag = bcls_ret->Tables[0].Rows[0]["RETFLAG"];
					if (0 == strcmp(RetFlag, "Y"))
					{
						Log::Debug("", __FUNCTION__, "D5推荐垛位满足要求，推荐成功");
						flag = 1;
						break;
					}
					else
					{
						Log::Debug("", __FUNCTION__, "D5推荐垛位不满足要求，继续推荐");
						flag = 0;
					}
				}
			}
		}


		if (flag == 0)//对主属性非空的18小时以外的垛位进行推荐
		{
			Log::Debug("", __FUNCTION__, "***D3**未使用的空垛位集推荐开始**D3***");
			sqlstr = "	SELECT STOCK_PLACE_NO FROM twm04 WHERE stock_no  = 'SA4' AND PILE_MAIN_NO = @v_circle AND(PILE_MAT_NUM_ACT + PRE_MAT_NUM) < MAX_HEIGHT"
				" AND MAX_HEIGHT > 0 AND  PILE_MAT_NUM_ACT > 0 AND STOCK_STATUS = '0'"
				" ORDER BY PILE_MAT_NUM_ACT DESC, FIELDNO_UPTIME DESC, PILE_FIELDNO_NO ASC";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.Parameters.Set("v_circle", v_circle);
			int count3 = cmd_inq.ExecuteQuery(bcls_temp.Tables[0]);
			if (count3 == 0){
				v_stock_place_no = "ZZZ";
			}
			for (int i = 0; i < count3; i++)
			{
				v_stock_place_no = " ";
				v_stock_place_no = bcls_temp.Tables[0].Rows[i]["STOCK_PLACE_NO"];

				if (i>100){
					Log::Debug("", __FUNCTION__, "非空垛位集循环推荐结束，未获取到有效垛位(D3)");
					flag = 0;
					break;
				}
				

				if (v_pos == v_stock_place_no)
				{
					Log::Debug("", __FUNCTION__, "该垛位是传人垛位，不满足要求 = [{0}]", v_pos);
					//EDLog(1, 1, "该垛位是传人垛位，不满足要求=【%s】", v_pos);
				}
				else
				{
					Log::Debug("", __FUNCTION__, "D3推荐垛位为 = [{0}]", v_stock_place_no);
					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_stock_place_no;
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "1";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = v_circle;
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------开始555");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, bcls_ret, conn);
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------结束555");
					if (0 != doFlag)
					{
						Log::Debug("", __FUNCTION__, "***(第三层)推荐垛位板坯过程调用失败***");
						strcpy(s.msg, "***(第三层)推荐垛位板坯过程调用失败***");
						s.flag = -1;
						return -1;
					}

					RetFlag = bcls_ret->Tables[0].Rows[0]["RETFLAG"];

					if (0 == strcmp(RetFlag, "Y"))
					{
						Log::Debug("", __FUNCTION__, "D3推荐垛位满足要求，推荐成功");
						flag = 1;
						break;
					}
					else
					{
						flag = 0;
						Log::Debug("", __FUNCTION__, "D3推荐垛位不满足要求，继续推荐");
					}
				}
			}
		}

		if (flag == 0)//对非主属性、非扩展属性的东边空垛位推荐
		{
			Log::Debug("", __FUNCTION__, "***D6**对东边的空垛位进行推荐**D6***");
			sqlstr = "SELECT STOCK_PLACE_NO FROM twm04 WHERE stock_no  = 'SA4' AND pile_assis_no <> @v_circle_1 AND PILE_MAIN_NO <> @v_circle"
				" AND(PILE_MAT_NUM_ACT + pre_mat_num) = 0 AND MAX_HEIGHT > 0 AND STOCK_STATUS = '0' AND store_area = '1'"
				" AND pile_assis_no < '60' AND pile_assis_no > '00' ORDER BY PILE_ASSIS_NO, PILE_FIELDNO_NO";
			cmd_inq.Close();
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			Log::Trace("", __FUNCTION__, "v_circle = [{0}]", v_circle);
			Log::Trace("", __FUNCTION__, "v_circle_1 = [{0}]", v_circle_1);
			cmd_inq.Parameters.Set("v_circle", v_circle);
			cmd_inq.Parameters.Set("v_circle_1", v_circle_1);
			int count6 = cmd_inq.ExecuteQuery(bcls_temp.Tables[0]);
			if (count6 == 0){
				v_stock_place_no = "ZZZ";
			}
			for (int i = 0; i < count6; i++)
			{
				v_stock_place_no = bcls_temp.Tables[0].Rows[i]["STOCK_PLACE_NO"];
				if (i>100){
					Log::Debug("", __FUNCTION__, "对东边的空垛位进行推荐结束，未获取到有效垛位(D6)");
					flag = 0;
					break;
				}
			

				if (v_pos == v_stock_place_no)
				{
					Log::Debug("", __FUNCTION__, "该垛位是传人垛位，不满足要求 = [{0}]", v_pos);
				}
				else
				{
					Log::Debug("", __FUNCTION__, "D6推荐垛位为 = [{0}]", v_stock_place_no);
					bcls_pile_jud.Tables[0].Rows[0]["MAT_NO"] = v_mat_no;
					bcls_pile_jud.Tables[0].Rows[0]["STOCK_PLACE_NO"] = v_stock_place_no;
					bcls_pile_jud.Tables[0].Rows[0]["Y"] = "0";
					bcls_pile_jud.Tables[0].Rows[0]["SLAB_PILE_VALUE"] = v_circle;
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------开始666");
					doFlag = f_wm00_pile_jud(&bcls_pile_jud, bcls_ret, conn);
					Log::Debug("", __FUNCTION__, "f_wm00_pile_jud------------------------------结束666");
					if (0 != doFlag)
					{
						Log::Debug("", __FUNCTION__, "***(第三层)推荐垛位板坯过程调用失败***");
						strcpy(s.msg, "***(第三层)推荐垛位板坯过程调用失败***");
						s.flag = -1;
						return -1;
					}

					RetFlag = bcls_ret->Tables[0].Rows[0]["RETFLAG"];


					if (0 == strcmp(RetFlag, "Y"))
					{
						Log::Debug("", __FUNCTION__, "D6推荐垛位满足要求，推荐成功");
						
						break;
					}
					else
					{
					
						Log::Debug("", __FUNCTION__, "D6推荐垛位不满足要求，继续推荐");
					}
				}
			}
		}

		if (!bcls_ret->Tables[0].Columns.Contains("COMF_STOCK"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "COMF_STOCK");
		}
		if (bcls_ret->Tables[0].Rows.get_Count() == 0){
			bcls_ret->Tables[0].Rows.Add();
		}
		bcls_ret->Tables[0].Rows[0]["COMF_STOCK"] = v_stock_place_no;
	
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


