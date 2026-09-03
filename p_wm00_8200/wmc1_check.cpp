/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:         QL
Version:		1.0
Date:			2016-06-30
Description:	清盘核对
**************************************************/

//框架头文件
#include "stdafx.h"

BM2F_ENTERACE(wmc1_check);

int f_wmc1_check(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	
	CDecimal rowCount = 0;
	int fetchRowCount = 0;

	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	CString sqlstr1 = "";
	CString sqlwhere = "";
	CString stock_no = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	/* ***** 定义表实体对象 ***** */
	CModel twmb3 = CModel("TWMB3");

	CDataTable dt_mat;

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//分页信息
		CDataTable& table = bcls_ret->Tables.Add("PAGEINFO");
		table.Columns.Add(DT_DECIMAL, "recordsum");

		//2)获取分页信息
		if (bcls_rec->Tables.Contains("PageInfo"))
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;  //每页记录数量
		}
		Log::Trace("", __FUNCTION__, "pageInfo.RecordFrom[{0}]pageInfo.PageSize[{1}]", pageInfo.RecordFrom, pageInfo.PageSize);	

		stock_no = bcls_rec->Tables[0].Rows[0]["stock_no"].ToString().Trim();
		Log::Trace("", "", "传入后台库区号：{0}", stock_no);

		sqlstr = "select a.mat_no as MAT_A,b.mat_no as MAT_B,a.stock_place_no as POSITION_A,b.stock_place_no as POSITION_B ,"
			     " a.layerno as LAYERNO_A, b.layerno as LAYERNO_B, a.mat_num as NUM_A, b.mat_num as NUM_B "
				 " from twmb1 a full join twma2 b on a.mat_no = b.mat_no where b.stock_no='" + stock_no + "' or a.stock_no = '" + stock_no + "'" ;
		Log::Trace("", "", "sqlstr：{0}", sqlstr);
		Db::QueryTable(sqlstr, dt_mat);

		Log::Trace("", "", "Count：{0}", dt_mat.Rows.get_Count());
		//dt_mat.Columns.Add(DT_STRING,"RESULT_CODE");
		for (int i = 0; i < dt_mat.Rows.get_Count(); i++)
		{
            if(dt_mat.Rows[i]["MAT_A"].ToString().Trim() == ""&&dt_mat.Rows[i]["MAT_B"].ToString().Trim() != "")
			{
			  
				Log::Trace("", "", "有信息无实物--{0}", dt_mat.Rows[i]["MAT_B"].ToString());				
				twmb3.Reset();
				twmb3["REC_CREATOR"] = s.userid;
				twmb3["REC_CREATE_TIME"] = datetime;
				twmb3["REC_REVISOR"] = " ";
				twmb3["REC_REVISE_TIME"] = " ";
				twmb3["MAT_NO"] = dt_mat.Rows[i]["MAT_B"].ToString();
				twmb3["RESULT_CODE"] = "2";
				twmb3["STOCK_NO"] = stock_no;
				twmb3["STOCK_PLACE_NO"] = dt_mat.Rows[i]["POSITION_B"].ToString(); 
				twmb3["LAYERNO"] = dt_mat.Rows[i]["LAYERNO_B"].ToString(); 
				twmb3["MAT_NUM"] = dt_mat.Rows[i]["NUM_B"].ToString();
				
				if (twmb3.QueryCount("MAT_NO,RESULT_CODE")>0)
				{
					Log::Trace("", "", "{0}有信息无实物update", dt_mat.Rows[i]["MAT_B"].ToString());								
					//twmb3.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,STOCK_NO,STOCK_PLACE_NO,LAYERNO,MAT_NUM", "MAT_NO,RESULT_CODE");
					sqlstr = "update twmb3 set REC_CREATOR='" + twmb3["REC_CREATOR"].ToString() + "'"
						     "  ,REC_CREATE_TIME='" + twmb3["REC_CREATE_TIME"].ToString() + "'"
							 "  ,REC_REVISOR='" + twmb3["REC_REVISOR"].ToString() + "'"
							 "  ,REC_REVISE_TIME='" + twmb3["REC_REVISE_TIME"].ToString() + "'"
							 "  ,MAT_NO='" + twmb3["MAT_NO"].ToString() + "'"
							 "  ,RESULT_CODE='" + twmb3["RESULT_CODE"].ToString() + "'"
							 "  ,STOCK_NO='" + twmb3["STOCK_NO"].ToString() + "'"
							 "  ,STOCK_PLACE_NO='" + twmb3["STOCK_PLACE_NO"].ToString() + "'"
							 "  ,LAYERNO='" + twmb3["LAYERNO"].ToString() + "'"
							 "  ,MAT_NUM='" + twmb3["MAT_NUM"].ToString() + "'"
							 " where MAT_NO='" + twmb3["MAT_NO"].ToString() + "' and RESULT_CODE='" + twmb3["RESULT_CODE"].ToString() + "'" ;
					Log::Trace("", "", "sqlstr：{0}", sqlstr);
					Db::Execute(sqlstr);	
				
					twmb3.MergeTo(bcls_ret->Tables[0],false);

				}
				else
				{
					Log::Trace("", "", "{0}有信息无实物insert", dt_mat.Rows[i]["MAT_B"].ToString());
					twmb3.Insert();
					twmb3.MergeTo(bcls_ret->Tables[0], false);
					

				}

			}
			else if (dt_mat.Rows[i]["MAT_A"].ToString().Trim() != ""&&dt_mat.Rows[i]["MAT_B"].ToString().Trim() == "")
			{
				Log::Trace("", "", "有实物无信息--{0}", dt_mat.Rows[i]["MAT_A"].ToString());
				//dt_mat.Rows[i]["RESULT_CODE"] = "1";
				twmb3.Reset();
				twmb3["REC_CREATOR"] = s.userid;
				twmb3["REC_CREATE_TIME"] = datetime;
				twmb3["REC_REVISOR"] = " ";
				twmb3["REC_REVISE_TIME"] = " ";
				twmb3["MAT_NO"] = dt_mat.Rows[i]["MAT_A"].ToString();
				twmb3["RESULT_CODE"] = "1";
				twmb3["STOCK_NO"] = stock_no;
				twmb3["STOCK_PLACE_NO"] = dt_mat.Rows[i]["POSITION_A"].ToString();
				twmb3["LAYERNO"] = dt_mat.Rows[i]["LAYERNO_A"].ToString();
				twmb3["MAT_NUM"] = dt_mat.Rows[i]["NUM_A"].ToString();

				if (twmb3.QueryCount("MAT_NO,RESULT_CODE")>0)
				{
					Log::Trace("", "", "{0}有实物无信息update", dt_mat.Rows[i]["MAT_A"].ToString());
					//twmb3.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,STOCK_NO,STOCK_PLACE_NO,LAYERNO,MAT_NUM", "MAT_NO,RESULT_CODE");
					sqlstr = "update twmb3 set REC_CREATOR='" + twmb3["REC_CREATOR"].ToString() + "'"
						"  ,REC_CREATE_TIME='" + twmb3["REC_CREATE_TIME"].ToString() + "'"
						"  ,REC_REVISOR='" + twmb3["REC_REVISOR"].ToString() + "'"
						"  ,REC_REVISE_TIME='" + twmb3["REC_REVISE_TIME"].ToString() + "'"
						"  ,MAT_NO='" + twmb3["MAT_NO"].ToString() + "'"
						"  ,RESULT_CODE='" + twmb3["RESULT_CODE"].ToString() + "'"
						"  ,STOCK_NO='" + twmb3["STOCK_NO"].ToString() + "'"
						"  ,STOCK_PLACE_NO='" + twmb3["STOCK_PLACE_NO"].ToString() + "'"
						"  ,LAYERNO='" + twmb3["LAYERNO"].ToString() + "'"
						"  ,MAT_NUM='" + twmb3["MAT_NUM"].ToString() + "'"
						" where MAT_NO='" + twmb3["MAT_NO"].ToString() + "' and RESULT_CODE='" + twmb3["RESULT_CODE"].ToString() + "'";
					Log::Trace("", "", "sqlstr：{0}", sqlstr);
					Db::Execute(sqlstr);
					twmb3.MergeTo(bcls_ret->Tables[0], false);
				}
				else
				{
					Log::Trace("", "", "{0}有实物无信息insert", dt_mat.Rows[i]["MAT_A"].ToString());
					twmb3.Insert();
					twmb3.MergeTo(bcls_ret->Tables[0], false);
				}


			}
			else if (dt_mat.Rows[i]["MAT_A"].ToString().Trim() != ""&&dt_mat.Rows[i]["MAT_B"].ToString().Trim() != "")
			{

				Log::Trace("", "", "111111111@@@");

				if (dt_mat.Rows[i]["POSITION_A"].ToString() == dt_mat.Rows[i]["POSITION_B"].ToString()
					&& dt_mat.Rows[i]["LAYERNO_A"].ToString() == dt_mat.Rows[i]["LAYERNO_B"].ToString()
					&& dt_mat.Rows[i]["NUM_A"].ToString() == dt_mat.Rows[i]["NUM_B"].ToString())
				{
					Log::Trace("", "", "222222@@@");
					dt_mat.Rows[i].Delete();
				}
				else
				{
					Log::Trace("", "", "实物与信息不符--{0}", dt_mat.Rows[i]["MAT_A"].ToString());
					//dt_mat.Rows[i]["RESULT_CODE"] = "3";
					twmb3.Reset();
					twmb3["REC_CREATOR"] = s.userid;
					twmb3["REC_CREATE_TIME"] = datetime;
					twmb3["REC_REVISOR"] = " ";
					twmb3["REC_REVISE_TIME"] = " ";
					twmb3["MAT_NO"] = dt_mat.Rows[i]["MAT_A"].ToString();
					twmb3["RESULT_CODE"] = "3";
					twmb3["STOCK_NO"] = stock_no;
					twmb3["STOCK_PLACE_NO"] = dt_mat.Rows[i]["POSITION_A"].ToString();
					twmb3["LAYERNO"] = dt_mat.Rows[i]["LAYERNO_A"].ToString();
					twmb3["MAT_NUM"] = dt_mat.Rows[i]["NUM_A"].ToString();

					if (twmb3.QueryCount("MAT_NO,RESULT_CODE")>0)
					{
						Log::Trace("", "", "{0}实物与信息不符update", dt_mat.Rows[i]["MAT_A"].ToString());
						//twmb3.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,STOCK_NO,STOCK_PLACE_NO,LAYERNO,MAT_NUM", "MAT_NO,RESULT_CODE");
						sqlstr = "update twmb3 set REC_CREATOR='" + twmb3["REC_CREATOR"].ToString() + "'"
							"  ,REC_CREATE_TIME='" + twmb3["REC_CREATE_TIME"].ToString() + "'"
							"  ,REC_REVISOR='" + twmb3["REC_REVISOR"].ToString() + "'"
							"  ,REC_REVISE_TIME='" + twmb3["REC_REVISE_TIME"].ToString() + "'"
							"  ,MAT_NO='" + twmb3["MAT_NO"].ToString() + "'"
							"  ,RESULT_CODE='" + twmb3["RESULT_CODE"].ToString() + "'"
							"  ,STOCK_NO='" + twmb3["STOCK_NO"].ToString() + "'"
							"  ,STOCK_PLACE_NO='" + twmb3["STOCK_PLACE_NO"].ToString() + "'"
							"  ,LAYERNO='" + twmb3["LAYERNO"].ToString() + "'"
							"  ,MAT_NUM='" + twmb3["MAT_NUM"].ToString() + "'"
							" where MAT_NO='" + twmb3["MAT_NO"].ToString() + "' and RESULT_CODE='" + twmb3["RESULT_CODE"].ToString() + "'";
						Log::Trace("", "", "sqlstr：{0}", sqlstr);
						Db::Execute(sqlstr);
						twmb3.MergeTo(bcls_ret->Tables[0], false);
					}
					else
					{
						Log::Trace("", "", "{0}实物与信息不符insert", dt_mat.Rows[i]["MAT_A"].ToString());
						twmb3.Insert();
						twmb3.MergeTo(bcls_ret->Tables[0], false);
					}

				}			
			}
			else if(dt_mat.Rows[i]["MAT_A"].ToString().Trim() == ""&&dt_mat.Rows[i]["MAT_B"].ToString().Trim() == "")
			{
				sprintf(s.msg, "报错");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
		}
		
		//返回记录总数
		CDataRow& row1 = bcls_ret->Tables["PAGEINFO"].Rows.Add();
		row1["recordsum"] = dt_mat.Rows.get_Count();
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

































