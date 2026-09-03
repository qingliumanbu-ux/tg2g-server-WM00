/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-01
Description: 钢包信息报废
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包信息报废
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  
  

//外部函数声明

BM2F_ENTERACE(wm06_blo)                                                     

int f_wm06_blo(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	
	/* 实体类定义 */ 
	CModel twm06("TWM06");
	CModel ttmsm02("TTMSM02");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		
		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			twm06.Reset();
			twm06.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			twm06.TrimOrBlank();

			Log::Trace("",__FUNCTION__,"twm06.SM_UNIT_NO		= [{0}]",(const char*)twm06["CRANE_NO"].ToString());
			Log::Trace("",__FUNCTION__,"twm06.LADLE_NO		= [{0}]",(const char*)twm06["CRANE_STATUS"].ToString());
			/*Log::Trace("",__FUNCTION__,"ttmsm01.SCRAP_CAUSE_CODE	= [{0}]",(const char*)ttmsm01["SCRAP_CAUSE_CODE"].ToString());
			Log::Trace("",__FUNCTION__,"ttmsm01.SCRAP_CAUSE_NAME	= [{0}]",(const char*)ttmsm01["SCRAP_CAUSE_NAME"].ToString());
			Log::Trace("",__FUNCTION__,"ttmsm01.SCRAP_TIME			= [{0}]",(const char*)ttmsm01["SCRAP_TIME"].ToString());
			Log::Trace("",__FUNCTION__,"ttmsm01.SCRAP_MAKER			= [{0}]",(const char*)ttmsm01["SCRAP_MAKER"].ToString());*/

			/* 检查输入参数合法性 */
			/*if(ttmsm01["SM_UNIT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"炼钢单元号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if(twm06["CRANE_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"吊车号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该钢包号是否存在 */
			if(twm06.QueryCount("CRANE_NO") <= 0)
			{
				sprintf(s.msg,"钢包号[%s]不存在!",(const char*)twm06["CRANE_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//ttmsm02["LADLE_NO"]   = ttmsm01["LADLE_NO"];
			//ttmsm02["SM_UNIT_NO"] = ttmsm01["SM_UNIT_NO"];
			//if(ttmsm02.QueryCount("SM_UNIT_NO,LADLE_NO") > 0)
			//{
			//	sprintf(s.msg,"钢包号[%s]尚在配包计划中!",(const char*)ttmsm01["LADLE_NO"].ToString());
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}

			/* 查询钢包信息 */			 
			twm06.Query("CRANE_NO");
			twm06.TrimOrBlank();

			/* 设置默认值 */
			//twm06["LADLE_STATUS"]	= "99";	//99-报废
			
			/* 修改钢包信息 */		
			//ttmsm01["SCRAP_CAUSE_CODE"];	//报废原因代码
			//ttmsm01["SCRAP_CAUSE_NAME"];	//报废原因名称
			if(twm06["CRANE_STATUS"].ToString().Trim() == "" || '0')
			{
				
				twm06["CRANE_STATUS"]		= '1';   //封锁
				Log::Trace("", __FUNCTION__, "twm06.CRANE_STATUS		= [{0}]", (const char*)twm06["CRANE_STATUS"].ToString());
			}
			//if(ttmsm01["SCRAP_MAKER"].ToString().Trim() == "")
			//{
			//	ttmsm01["SCRAP_MAKER"]		= s.userid;   //报废责任者
			//}			
			//ttmsm01["REC_REVISOR"]		= s.userid;   //记录创建责任者
			//ttmsm01["REC_REVISE_TIME"]	= datetime;   //记录创建时刻
			sqlstr ="CRANE_STATUS";
			twm06.Update(sqlstr,"CRANE_NO");
		}
		

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

