/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     ljnie
Version:    1.0
Date:       2016/7/15 11:05:53
Description: 车辆来料信息修改
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 车辆来料信息修改
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

////业务头文件
//#include "twma0.h"//倒垛队列  
//#include "twma1.h"//材料信息  
//#include "twma2.h"//库位跟踪表  

//外部函数声明
BM2_FUNCTION_IMPORT
//int f_wm00_update_stock_place(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//更新库位号TWMA2

//BM2_FUNCTION_IMPORT
//int f_wm00_wm03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//库位变更电文发送

BM2F_ENTERACE(wma3_upd)

int f_wma3_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	//CTWMA0 twma0(conn);
	//CTWMA1 twma1(conn);
	//CTWMA2 twma2(conn);
	CModel twma0("TWMA0");
	CModel twma1("TWMA1");
	CModel twma2("TWMA2");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//更新库位号
	EIClass bcls_rec_update;
	bcls_rec_update.Tables[0].set_TableName("WM00_UPDATE");
	bcls_rec_update.Tables[0].Columns.Add(twma2);
	CDataRow &row_update = bcls_rec_update.Tables["WM00_UPDATE"].Rows.Add();

	//	//向MMS发送库位材料信息电文
	//	EIClass bcls_wm03;
	//	bcls_wm03.Tables[0].set_TableName("WM03");
	//	bcls_wm03.Tables[0].Columns.Add(DT_STRING, "MAT_NO");

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			twma0.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twma0.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "twma0.MAT_NO = [{0}]", (const char*)twma0["MAT_NO"]);

			/* 检查输入参数合法性 */
			if (twma0["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "材料号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改倒垛队列表 TWMA0 */
			twma0["REC_REVISOR"] = s.userid;
			twma0["REC_REVISE_TIME"] = datetime;
			sqlstr = " ";
			//车号、车上位置、层号
			if (twma0["VEHICLE_NO"].ToString().Trim() != "")
			{
				sqlstr += "VEHICLE_NO,";
			}
			if (twma0["STOCK_PLACE_POSITION"].ToString().Trim() != "")
			{
				sqlstr += "STOCK_PLACE_POSITION,";
			}
			if (twma0["LAYERNO"].ToDecimal() > 0)
			{
				sqlstr += "LAYERNO,";
			}
			sqlstr += "REC_REVISOR,REC_REVISE_TIME";
			Log::Trace("", __FUNCTION__, "Update sqlstr = [{0}]", (const char*)sqlstr);
			twma0.Update(sqlstr, "MAT_NO");

			/*修改库位跟踪表 TWMA2*/
			twma1["MAT_NO"] = twma0["MAT_NO"];
			twma1["REC_REVISOR"] = s.userid;
			twma1["REC_REVISE_TIME"] = datetime;
			twma1["VEHICLE_NO"] = twma0["VEHICLE_NO"];
			//upd by ljnie 20160819 库位信息在A2表
			/*twma1.STOCK_PLACE_POSITION = twma0.STOCK_PLACE_POSITION;
			twma1.LAYERNO = twma0.LAYERNO;
			twma1.Update("VEHICLE_NO,STOCK_PLACE_POSITION,LAYERNO,REC_REVISOR,REC_REVISE_TIME","MAT_NO");*/
			twma1.Update("VEHICLE_NO,REC_REVISOR,REC_REVISE_TIME", "MAT_NO");

			twma1.Query("MAT_NO");

			twma2.Reset();
			twma2["MAT_NO"] = twma1["MAT_NO"];
			twma2.Query("MAT_NO");
			twma2["STOCK_NO"] = twma1["STOCK_NO"];
			/*twma2.STOCK_PLACE_NO = twma1.STOCK_PLACE_NO;*/
			twma2["LAYERNO"] = twma0["LAYERNO"];
			twma2["LOGIC_STOCK_NO"] = twma1["LOGIC_STOCK_NO"];
			twma2["STOCK_PLACE_POSITION"] = twma0["STOCK_PLACE_POSITION"];
			row_update.Merge(twma2);
			//调用更新库位函数
			//doFlag = f_wm00_update_stock_place(&bcls_rec_update, bcls_ret, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//#if defined _SYS_PES
			//		//向MMS发送库位材料信息电文
			//		bcls_wm03.Tables[0].Rows.Add();
			//		bcls_wm03.Tables[0].Rows[0]["MAT_NO"] = twma0.MAT_NO;
			//		doFlag = f_wm00_wm03_snd(&bcls_wm03, bcls_ret, conn);
			//		if (doFlag != 0)
			//		{
			//			throw CApplicationException(-1, s.msg, log.Location);
			//		}
			//#endif


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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

