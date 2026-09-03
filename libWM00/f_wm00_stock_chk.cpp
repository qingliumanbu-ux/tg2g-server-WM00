/* **************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_wm00_stock_udpate
*  程序描述			: 更新库位跟踪表
*  备注说明			:
*  修改历史			:
*  		henno 2016-09-28			(ADD)程序建立
*			... ...
* **************************************************************************** */
/* C/C++ 的标准头文件部分 */
#include "stdafx.h"		// 框架头，不可删除 
//#include "twm04.h" 



BM2_FUNCTION_EXPORT
int f_wm00_stock_chk(CString stockPlaceNo, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/* ***** 程序变量 ***** */
	int doFlag = 0;
	CString v_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CDecimal v_max_layerno = 0;
	CDecimal v_count = 0;


	/* ***** 数据库SQL操作字符串 ***** */
	CString	sqlstr = " ";

	/* ***** 数据库操作类定义 ***** */
	CDbCommand comm(conn);

	/* ***** 定义表实体对象 ***** */
	CModel twm04("TWM04");
	CModel twm04_right("TWM04");
	CModel twm04_left("TWM04");
	//CTWM04 twm04(conn);
	//CTWM04 twm04_right(conn);
	//CTWM04 twm04_left(conn);



	/* ***** 应用程序开始处理 ***** */
	try
	{
		Log::Trace("", __FUNCTION__, "检查库位 stockPlaceNo\t[{0}]", stockPlaceNo);

		twm04["STOCK_PLACE_NO"] = stockPlaceNo;

		if (stockPlaceNo.Trim() == "")
		{
			return doFlag;
		}

		if (!twm04.Query("STOCK_PLACE_NO"))
		{
			return doFlag;
		}

		if (twm04["STOCK_STATUS"].ToString().Trim() == "1")
		{
			sprintf(s.msg, "垛位【%s】已被封锁，无法入库。", (const char*)twm04["STOCK_PLACE_NO"]);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/// 校验库位是否超限
		if (twm04["PILE_MAT_WT_ACT"].ToDecimal() > twm04["MAX_WT"].ToDecimal())
		{
			sprintf(s.msg, "垛位【%s】堆放重量超限。", (const char*)twm04["STOCK_PLACE_NO"]);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//MAX_HEIGHT 改字段梅钢为存储最大支数
		/*if (twm04["PILE_MAT_HEI_ACT"].ToDecimal() > twm04["MAX_HEIGHT"].ToDecimal())
		{
			sprintf(s.msg, "垛位【%s】堆放高度超限。", (const char*)twm04["STOCK_PLACE_NO"]);
			throw CApplicationException(-1, s.msg, log.Location);
		}*/


		if (twm04["MANAGE_ACCU"].ToString().Trim() == "1" ||
			twm04["MANAGE_ACCU"].ToString().Trim() == "3")
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
				sqlstr =
					" SELECT ISNULL(MAX(LAYERNO), 0) FROM TWMA2"
					" WHERE STOCK_PLACE_NO = @stock_place_no";
				break;
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr =
					" SELECT NVL(MAX(LAYERNO), 0) FROM TWMA2"
					" WHERE STOCK_PLACE_NO = @stock_place_no";
				break;
			}

			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("stock_place_no", twm04["STOCK_PLACE_NO"].ToString());
			v_max_layerno = comm.ExecuteScalar();

			if (v_max_layerno > twm04["MAX_LAYER_COUNT"].ToDecimal())
			{
				sprintf(s.msg, "垛位【%s】堆放层数超限。", (const char*)twm04["STOCK_PLACE_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//一品一地
		else if (twm04["MANAGE_ACCU"].ToString().Trim() == "5")
		{
			sqlstr =
				" SELECT COUNT(1) FROM TWMA2"
				" WHERE STOCK_PLACE_NO = @stock_place_no";
			comm.SetCommandText(sqlstr);
			comm.Parameters.Set("stock_place_no", twm04["STOCK_PLACE_NO"].ToString());
			v_count = comm.ExecuteScalar();

			if (v_count > 1)
			{
				sprintf(s.msg, "垛位[%s]已经被占用，不能继续堆放。", (const char*)twm04["STOCK_PLACE_NO"]);
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//一品一地 堆放时  如果层号>1 校验下方位置是否有材料
			if (twm04["LAYERNO"].ToDecimal() > 1)
			{
				sqlstr =
					" SELECT * FROM TWM04"
					" WHERE STOCK_NO = @stock_no"
					" AND STOCK_ROW_NO = @stock_row_no"
					" AND STOCK_COL_NO = @stock_col_no"
					" AND LAYERNO = @layerno";

				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("stock_no", twm04["STOCK_NO"].ToString());
				comm.Parameters.Set("stock_row_no", twm04["STOCK_ROW_NO"].ToString());
				comm.Parameters.Set("stock_col_no", twm04["STOCK_COL_NO"].ToString());
				comm.Parameters.Set("layerno", twm04["LAYERNO"].ToDecimal() - 1);
				comm.ExecuteReader();
				if (comm.Read())
				{
					comm.Fetch(twm04_left);
					twm04_left.TrimOrBlank();
				}
				else
				{
					sprintf(s.msg, "垛位[%s]左下方垛位未正确配置。", (const char*)twm04["STOCK_PLACE_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				comm.Close();


				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("stock_no", twm04["STOCK_NO"].ToString());
				comm.Parameters.Set("stock_row_no", twm04["STOCK_ROW_NO"].ToString());
				comm.Parameters.Set("stock_col_no", twm04["STOCK_COL_NO"].ToString());
				comm.Parameters.Set("layerno", twm04["LAYERNO"].ToDecimal() - 1);
				comm.ExecuteReader();
				if (comm.Read())
				{
					comm.Fetch(twm04_right);
					twm04_right.TrimOrBlank();
				}
				else
				{
					sprintf(s.msg, "垛位[%s]右下方垛位未正确配置。", (const char*)twm04["STOCK_PLACE_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				comm.Close();


				sqlstr =
					" SELECT COUNT(1) FROM TWMA2"
					" WHERE STOCK_PLACE_NO = @stock_place_no";
				comm.SetCommandText(sqlstr);
				comm.Parameters.Set("stock_place_no", twm04_left["STOCK_PLACE_NO"].ToString());
				v_count = comm.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg, "垛位[%s]左下方垛位为空，不能入库。", (const char*)twm04["STOCK_PLACE_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				comm.Parameters.Set("stock_place_no", twm04_right["STOCK_PLACE_NO"].ToString());
				v_count = comm.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg, "垛位[%s]右下方垛位为空，不能入库。", (const char*)twm04["STOCK_PLACE_NO"]);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
