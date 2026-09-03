/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:      JQ
Version:     1.1.1
Date:        2019-06-11
Description: 垛位推荐
**************************************************/

#include "stdafx.h"		// 框架头，不可删除 
#include "math.h"
#include<list>

class WmsStockplace;
class WmsMat;
class WmsMap;
class WmsPeople;
class WmsMain;
CDataTable Tab_Sub;

class WmsMat
{
public:
	WmsMat();
	~WmsMat();
	int Setinfo(CString mat_no, CDbConnection * conn);
	CString GetClass();
	CString GetCode(CString code);
public:
	CString MAT_NO = "";                        //材料号
	CString PLAN_NO = "";                       //计划号
	CString LAYERNO = "";                       //材料层号
	CString LOGIC_STOCK_NO = "";                //材料所在逻辑区
	CString STOCK_PLACE_NO_TO = "";             //材料目标库位
	CString STOCK_PLACE_NO = "";                //材料库位
	CString PILE_INDEX = "";                    //材料配山指标
	CDecimal HOOD_TIME = 0;                     //材料出钢记号
	CDecimal MAT_LEN = 0;                       //材料长度
	CDecimal MAT_THICK = 0;                     //材料厚度
	CDecimal MAT_WIDTH = 0;                     //材料宽度
	CDecimal MAT_WT = 0;                        //材料重量
	CDecimal X_RANGE = 0;                       //材料X
	CDecimal Y_RANGE = 0;                       //材料Y
	CDecimal COLUMN_NO = 0;                     //材料所在库位列号
	CString MAT_STATUS = "";                    //材料状态
	CString G_ORDER = "";                       //材料合同
	CString G_SG_SIGN = "";                     //材料钢种
	CString G_ORDER_CUST_CODE = "";             //材料用户

};
WmsMat::WmsMat()
{
}
WmsMat::~WmsMat()
{
}

int WmsMat::Setinfo(CString mat_no, CDbConnection * conn)
{
	try
	{
		CDbCommand cmd_inq(conn);//数据库操作类定义		
		int re_int = 0;          //返回值

		CString sqlstr = "SELECT mat_no,mat_kind,mat_shape_flag,mat_act_thick,mat_act_width,mat_act_len,mat_act_wt"
			" FROM twma1 WHERE mat_no = '" + mat_no + "'";

		CDataTable dtMat;
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(dtMat);
		cmd_inq.Close();
		if (dtMat.Rows.get_Count() == 1)
		{
			MAT_NO = dtMat.Rows[0]["MAT_NO"].ToString();



			re_int = 1;
		}
		else
		{
			re_int = 0;
		}

		return re_int;

	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

class WmsStockplace
{
public:
	WmsStockplace();
	~WmsStockplace();
	void AddMat(CDbConnection* dbConn);
public:
	CString  S_STOCK_PLACE_NO = "";                    //库位号
	CString  S_ROWNO = "";                             //库位行号
	CString  S_COLUMN_NO = "";                         //库位列号
	CString  S_LAYER_NO = "";                          //库位层号
	CDecimal S_X_RANGE = 0;                            //库位X轴坐标
	CDecimal S_Y_RANGE = 0;                            //库位Y轴坐标
	CDecimal S_WIDTH_DELTA = 0;                        //库位宽度差
	CDecimal S_WIDTH_DELTA_NEXT = 0;                   //库位相邻宽度差
	CDecimal S_LEN_DELTA = 0;                          //库位长度差
	CDecimal S_THICK_DELTA = 0;                        //库位厚度差
	CDecimal S_REM_GRADE = 0;                          //库位推荐分数
	CDecimal S_MAX_LEN = 0;                            //库位最大长度
	CDecimal S_MAX_HEIGHT = 0;                         //库位最大高度
	CDecimal S_MAX_WIDTH = 0;                          //库位最大宽度
	CDecimal S_MAX_WT = 0;                             //库位最大重量
	CDecimal S_NOW_HEIGHT = 0;                         //库位当前高度
	
	CDecimal S_MAT_LEN = 0;                            //库位材料长度
	CDecimal S_MAT_THICK = 0;                          //库位材料厚度	
	CString  S_STOCK_STATUS = "";                      //库位状态
	CDecimal S_MAT_WIDTH = 0;                          //库位材料宽度
	CDecimal S_MAT_OUTER_DIA = 0;                      //库位材料外径
	CDecimal S_MAT_WT = 0;                             //库位材料重量
	CString  S_MAT_CMD_FLAG = "";                      //库位材料指令标记
	CDecimal S_LEFT_MAT_WT = 0;                        //左库位材料重量
	CDecimal S_LEFT_MAT_WIDTH = 0;                     //左库位材料宽度
	CDecimal S_LEFT_MAT_OUTER_DIA = 0;                 //左库位材料外径
	CString  S_LEFT_MAT_CMD_FLAG = "";                 //左库位材料指令标记
	CDecimal S_RIGHT_MAT_WT = 0;                       //右库位材料重量
	CDecimal S_RIGHT_MAT_WIDTH = 0;                    //右库位材料宽度
	CDecimal S_RIGHT_MAT_OUTER_DIA = 0;                //右库位材料外径
	CString  S_RIGHT_MAT_CMD_FLAG = "";                //右库位材料指令标记
	list<WmsMat> list_mat;
	WmsMat Mat;
};
WmsStockplace::WmsStockplace()
{

}
WmsStockplace::~WmsStockplace()
{
}

void WmsStockplace::AddMat(CDbConnection* dbConn)
{
	try
	{

	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

class WmsMap
{
public:
	WmsMap();
	~WmsMap();
	void AddStockplace(CDbConnection* dbConn);
	WmsStockplace GetBestStock();
public:
	CString Map_Num = "";                      //地图编号
	CString Hall_no = "";                      //跨
	CDecimal Start_Cloumn = 0;                 //起始列
	CDecimal Max_Cloumn = 0;                   //最大列
	CDecimal Min_Cloumn = 0;                   //最小列
	CString Logic_stock_no = "";               //区域
	CString Rem_Stock_place = "";              //推荐库位
	CDecimal Rem_Grade = 0;                    //推荐分
	CDecimal Dev_no = 0;                       //区域分区
	CDecimal Search_mode = 0;                  //搜索方式
	CDecimal X_CD = 0;                         //X坐标
	CDecimal Y_CD = 0;                         //Y坐标
	CDecimal Y_CD = 0;                         //Y坐标
	CDecimal G_ORDER_NUM = 0;                  //相同合同材料数
	CDecimal G_D_ORDER_NUM = 0;                //不同合同材料数
	CDecimal G_SG_SIGN_NUM = 0;                //相同钢种材料数
	CDecimal G_D_SG_SIGN_NUM = 0;              //不同钢种材料数
	CDecimal G_ORDER_CUST_CODE_NUM = 0;        //相同用户材料数
	CDecimal G_D_ORDER_CUST_CODE_NUM = 0;      //不同用户材料数
	CDecimal G_EMPTY_NUM = 0;                  //空跺位数
	list<WmsStockplace> list_stock_place;

};
WmsMap::WmsMap()
{

}
WmsMap::~WmsMap()
{

}

void WmsMap::AddStockplace(CDbConnection* conn)
{
	try
	{
		CTracer log(__FUNCTION__); 	//系统日志类定义
		CDbCommand cmd_inq(conn);//数据库操作类定义		
		CString Current_row = "";
		CString Current_column = "";

		//加载库位
		CString Sql = "SELECT A.STOCK_PLACE_NO,A.STOCK_STATUS,A.COLUMN_NO,A.LAYERNO，A.ROWNO,SUBSTR(A.COLUMN_NO,length(A.COLUMN_NO),1) AS COLUMN_NUM,"
			" VALUE(B.MAT_NO,'') AS MAT_NO,VALUE(B.CMD_MAT_NO,'') AS CMD_MAT_NO ,VALUE(B.MAT_ACT_OUTER_DIA,0) AS MAT_ACT_OUTER_DIA ,VALUE(B.MAT_ACT_WIDTH,0) AS MAT_ACT_WIDTH ,VALUE(B.MAT_ACT_WT,0) AS MAT_ACT_WT " 
			" FROM TWM04 A" 
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.ORDER_NO, VALUE(E.MAT_NO, '') AS CMD_MAT_NO, B.MAT_ACT_WIDTH, B.MAT_ACT_WT, B.MAT_ACT_OUTER_DIA" 
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') B ON A.STOCK_PLACE_NO = B.STOCK_PLACE_NO" 
			" WHERE A.STOCK_NO = " + Hall_no + "' AND  A.STOCK_PLACE_TYPE = '0'  AND  SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) = '" + Map_Num + "'"
			" ORDER BY ROWNO, COLUMN_NO,LAYERNO";
		CDataTable dtMat;
		cmd_inq.SetCommandText(Sql);
		cmd_inq.ExecuteQuery(dtMat);
		cmd_inq.Close();
		for (int i = 0; i < dtMat.Rows.get_Count(); ++i)
		{
			if (i>0 && dtMat.Rows[i]["ROWNO"].ToString() == dtMat.Rows[i - 1]["ROWNO"].ToString()
				&& dtMat.Rows[i]["COLUMN_NO"].ToString() == dtMat.Rows[i - 1]["COLUMN_NO"].ToString()
				&& dtMat.Rows[i]["LAYERNO"].ToString() == dtMat.Rows[i - 1]["LAYERNO"].ToString())
			{
				sprintf(s.msg, "There are many materials in one stockplace");
				throw CApplicationException(-1, s.msg, log.Location);
				break;
			}

			WmsStockplace Stockplace;
			Stockplace.S_STOCK_PLACE_NO = dtMat.Rows[i]["STOCK_PLACE_NO"].ToString();
			Stockplace.S_STOCK_STATUS = dtMat.Rows[i]["STOCK_STATUS"].ToString();
			Stockplace.S_MAT_WIDTH = dtMat.Rows[i]["MAT_ACT_WIDTH"].ToDecimal();
			Stockplace.S_MAT_WT = dtMat.Rows[i]["MAT_ACT_WT"].ToDecimal();
			Stockplace.S_MAT_OUTER_DIA = dtMat.Rows[i]["MAT_ACT_OUTER_DIA"].ToDecimal();
			Stockplace.S_MAT_CMD_FLAG = dtMat.Rows[i]["CMD_MAT_NO"].ToString();
			Stockplace.S_LAYER_NO = dtMat.Rows[i]["LAYERNO"].ToString();

			if (dtMat.Rows[i]["LAYERNO"].ToString()=="1")
			{
				//第一层库位
				if (dtMat.Rows[i]["COLUMN_NUM"].ToString() == "1")
				{
					Stockplace.S_RIGHT_MAT_WIDTH = dtMat.Rows[i + 2]["MAT_ACT_WIDTH"].ToDecimal();
					Stockplace.S_RIGHT_MAT_OUTER_DIA = dtMat.Rows[i + 2]["MAT_ACT_OUTER_DIA"].ToDecimal();
					Stockplace.S_RIGHT_MAT_WT = dtMat.Rows[i + 2]["MAT_ACT_WT"].ToDecimal();
					Stockplace.S_RIGHT_MAT_CMD_FLAG = dtMat.Rows[i + 2]["CMD_MAT_NO"].ToString();
				}
				else if (dtMat.Rows[i]["COLUMN_NUM"].ToString() == "5")
				{
					Stockplace.S_LEFT_MAT_WIDTH = dtMat.Rows[i - 2]["MAT_ACT_WIDTH"].ToDecimal();
					Stockplace.S_LEFT_MAT_OUTER_DIA = dtMat.Rows[i - 2]["MAT_ACT_OUTER_DIA"].ToDecimal();
					Stockplace.S_LEFT_MAT_WT = dtMat.Rows[i - 2]["MAT_ACT_WT"].ToDecimal();
					Stockplace.S_LEFT_MAT_CMD_FLAG = dtMat.Rows[i - 2]["CMD_MAT_NO"].ToString();
				}
				else
				{
					Stockplace.S_RIGHT_MAT_WIDTH = dtMat.Rows[i + 2]["MAT_ACT_WIDTH"].ToDecimal();
					Stockplace.S_RIGHT_MAT_OUTER_DIA = dtMat.Rows[i + 2]["MAT_ACT_OUTER_DIA"].ToDecimal();
					Stockplace.S_RIGHT_MAT_WT = dtMat.Rows[i + 2]["MAT_ACT_WT"].ToDecimal();
					Stockplace.S_RIGHT_MAT_CMD_FLAG = dtMat.Rows[i + 2]["CMD_MAT_NO"].ToString();

					Stockplace.S_LEFT_MAT_WIDTH = dtMat.Rows[i - 2]["MAT_ACT_WIDTH"].ToDecimal();
					Stockplace.S_LEFT_MAT_OUTER_DIA = dtMat.Rows[i - 2]["MAT_ACT_OUTER_DIA"].ToDecimal();
					Stockplace.S_LEFT_MAT_WT = dtMat.Rows[i - 2]["MAT_ACT_WT"].ToDecimal();
					Stockplace.S_LEFT_MAT_CMD_FLAG = dtMat.Rows[i - 2]["CMD_MAT_NO"].ToString();
				}
			}
			else if (dtMat.Rows[i]["LAYERNO"].ToString() == "2")
			{
				//第二层库位
				Stockplace.S_RIGHT_MAT_WIDTH = dtMat.Rows[i + 1]["MAT_ACT_WIDTH"].ToDecimal();
				Stockplace.S_RIGHT_MAT_OUTER_DIA = dtMat.Rows[i + 1]["MAT_ACT_OUTER_DIA"].ToDecimal();
				Stockplace.S_RIGHT_MAT_WT = dtMat.Rows[i + 1]["MAT_ACT_WT"].ToDecimal();
				Stockplace.S_RIGHT_MAT_CMD_FLAG = dtMat.Rows[i + 1]["CMD_MAT_NO"].ToString();

				Stockplace.S_LEFT_MAT_WIDTH = dtMat.Rows[i - 1]["MAT_ACT_WIDTH"].ToDecimal();
				Stockplace.S_LEFT_MAT_OUTER_DIA = dtMat.Rows[i - 1]["MAT_ACT_OUTER_DIA"].ToDecimal();
				Stockplace.S_LEFT_MAT_WT = dtMat.Rows[i - 1]["MAT_ACT_WT"].ToDecimal();
				Stockplace.S_LEFT_MAT_CMD_FLAG = dtMat.Rows[i - 1]["CMD_MAT_NO"].ToString();

			}
			else{
			
			}


			list_stock_place.push_back(Stockplace);
		}


	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

WmsStockplace WmsMap::GetBestStock()
{
	try
	{
		for (list<WmsStockplace>::iterator Stock = list_stock_place.begin(); Stock != list_stock_place.end(); Stock++)
		{


		}
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}

}


class WmsPeople
{
public:
	WmsPeople();
	~WmsPeople();
	int Search_Map(WmsMap Map, WmsMat mat_info, CString *rem_stock_place, CDecimal next_num, CString out_code13, CString hun_flag);
	WmsMap GetBestMap(WmsMat InputMat);
public:
	list<WmsMap> list_map;
};
WmsPeople::WmsPeople()
{

}
WmsPeople::~WmsPeople()
{

}

WmsMap WmsPeople::GetBestMap(WmsMat InputMat)
{
	try
	{
		CDecimal max_grade = 0;
		WmsMap re_map;
		for (list<WmsMap>::iterator map1 = list_map.begin(); map1 != list_map.end(); map1++)
		{
			//已满的区域排除掉
			if ((*map1).G_EMPTY_NUM == 0) continue;

			//计算区域的距离  -占10分
			int mat_column = atoi((InputMat.COLUMN_NO / 10).ToString());
			int map_column = atoi((*map1).Map_Num);
			(*map1).Rem_Grade = 10 - abs((map_column - mat_column))*2;

			//同合同  占50分
			(*map1).Rem_Grade = (*map1).Rem_Grade + (*map1).G_ORDER_NUM * 5;

			//同用户  占30分
			(*map1).Rem_Grade = (*map1).Rem_Grade + (*map1).G_ORDER_CUST_CODE_NUM * 3;

			//同钢种  占10分
			(*map1).Rem_Grade = (*map1).Rem_Grade + (*map1).G_SG_SIGN_NUM * 1;

			if (max_grade == 0 || max_grade<(*map1).Rem_Grade)
			{
				re_map = (*map1);
				max_grade = (*map1).Rem_Grade;
			}
		}

		return re_map;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}

}

int WmsPeople::Search_Map(WmsMap Map, WmsMat mat_info, CString *rem_stock_place, CDecimal next_num, CString out_code13, CString hun_flag)
{
	try
	{
		Log::Trace("", __FUNCTION__, "Search_Map() begin");

		Log::Trace("", __FUNCTION__, "Search_Map() end");
		return 0;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

class WmsMain
{
public:
	WmsMain(CDbConnection* dbConn);
	~WmsMain();
	int Setinfo(CString mat_no);


public:
	WmsMat InputMat;                    //需要推荐材料
	WmsPeople People;

protected:
	CDbConnection* dbConn;
	CDbCommand dbCmd;
public:
	void CreatePeople(CString hall_no, CString stock_oper_order, WmsMat InputMat, CDbConnection* conn);
	WmsMap CreatePeople_Slab(CString hall_no, CString stock_oper_order, WmsMat InputMat, CDbConnection* conn);	
};
WmsMain::WmsMain(CDbConnection* dbConn) : dbConn(dbConn), dbCmd(dbConn)
{

}
WmsMain::~WmsMain()
{

}

void WmsMain::CreatePeople(CString hall_no, CString stock_oper_order, WmsMat InputMat, CDbConnection* conn)
{
	try
	{
		Log::Trace("", __FUNCTION__, "CreatePeople() begin");
		CDbCommand cmd_inq(conn);//数据库操作类定义	

		//--加载区域
		CString sql = "SELECT A.COLUMN,VALUE(B.ORDER_NUM,0) AS ORDER_NUM,VALUE(C.D_ORDER_NUM,0) AS D_ORDER_NUM,VALUE(D.SG_SIGN_NUM,0) AS SG_SIGN_NUM,VALUE(E.D_SG_SIGN_NUM,0) AS D_SG_SIGN_NUM"
			" , VALUE(F.ORDER_CUST_CODE_NUM, 0) AS ORDER_CUST_CODE_NUM, VALUE(G.D_ORDER_CUST_CODE_NUM, 0) AS D_ORDER_CUST_CODE_NUM,VALUE(H.EMPTY_STOCK,0) AS EMPTY_STOCK FROM("
			" SELECT DISTINCT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN"
			" FROM TWM04 A WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0') A"
			" --同合同"
			" LEFT JOIN(SELECT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS ORDER_NUM"
			" FROM TWM04 A"
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.ORDER_NO"
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') D ON A.STOCK_PLACE_NO = D.STOCK_PLACE_NO"
			" WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND D.ORDER_NO = '" + InputMat.G_ORDER + "'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)"
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) B ON A.COLUMN = B.COLUMN"
			" --不同合同"
			" LEFT JOIN(SELECT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS D_ORDER_NUM"
			" FROM TWM04 A"
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.ORDER_NO"
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') D ON A.STOCK_PLACE_NO = D.STOCK_PLACE_NO"
			" WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND D.ORDER_NO != '" + InputMat.G_ORDER + "'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)"
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) C ON A.COLUMN = C.COLUMN"
			" --同钢种"
			" LEFT JOIN(SELECT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS SG_SIGN_NUM"
			" FROM TWM04 A"
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.SG_SIGN"
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') D ON A.STOCK_PLACE_NO = D.STOCK_PLACE_NO"
			" WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND D.SG_SIGN = '" + InputMat.G_SG_SIGN + "'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)"
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) D ON A.COLUMN = D.COLUMN"
			" --不同钢种"
			" LEFT JOIN(SELECT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS D_SG_SIGN_NUM"
			" FROM TWM04 A"
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.SG_SIGN"
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') D ON A.STOCK_PLACE_NO = D.STOCK_PLACE_NO"
			" WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND D.SG_SIGN != '" + InputMat.G_SG_SIGN + "'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)"
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) E ON A.COLUMN = E.COLUMN"
			" --同用户"
			" LEFT JOIN(SELECT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS ORDER_CUST_CODE_NUM"
			" FROM TWM04 A"
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.ORDER_CUST_CODE"
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') D ON A.STOCK_PLACE_NO = D.STOCK_PLACE_NO"
			" WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND D.ORDER_CUST_CODE = '" + InputMat.G_ORDER_CUST_CODE + "'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)"
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) F ON A.COLUMN = F.COLUMN"
			" --不同用户"
			" LEFT JOIN(SELECT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS D_ORDER_CUST_CODE_NUM"
			" FROM TWM04 A"
			" LEFT JOIN(SELECT B.MAT_NO, C.STOCK_PLACE_NO, B.ORDER_CUST_CODE"
			" FROM TWMA1 B, TWMA2 C LEFT JOIN TWMA7 E ON E.MAT_NO = C.MAT_NO WHERE C.MAT_NO = B.MAT_NO AND C.STOCK_NO = 'C01') D ON A.STOCK_PLACE_NO = D.STOCK_PLACE_NO"
			" WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND D.ORDER_CUST_CODE != '" + InputMat.G_ORDER_CUST_CODE + "'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)"
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) G ON A.COLUMN = G.COLUMN"
			" --空跺位数"
			" LEFT JOIN(" 
			" SELECT DISTINCT SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1) AS COLUMN, COUNT(1) AS EMPTY_STOCK FROM" 
			" TWM04 A WHERE A.STOCK_NO = '" + hall_no + "' AND STOCK_PLACE_TYPE = '0' AND STOCK_STATUS = '0'"
			" GROUP BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)" 
			" ORDER BY SUBSTR(A.COLUMN_NO, 1, length(A.COLUMN_NO) - 1)) H ON A.COLUMN = H.COLUMN";

		CDataTable dtMat;
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(dtMat);
		cmd_inq.Close();

		for (int i = 0; i < dtMat.Rows.get_Count(); ++i)
		{
			WmsMap Map_1;
			Map_1.Map_Num = dtMat.Rows[i]["COLUMN"].ToString();
			Map_1.G_ORDER_NUM = dtMat.Rows[i]["ORDER_NUM"].ToDecimal();
			Map_1.G_D_ORDER_NUM = dtMat.Rows[i]["D_ORDER_NUM"].ToDecimal();
			Map_1.G_SG_SIGN_NUM = dtMat.Rows[i]["SG_SIGN_NUM"].ToDecimal();
			Map_1.G_D_SG_SIGN_NUM = dtMat.Rows[i]["D_SG_SIGN_NUM"].ToDecimal();
			Map_1.G_ORDER_CUST_CODE_NUM = dtMat.Rows[i]["ORDER_CUST_CODE_NUM"].ToDecimal();
			Map_1.G_D_ORDER_CUST_CODE_NUM = dtMat.Rows[i]["D_ORDER_CUST_CODE_NUM"].ToDecimal();
			Map_1.G_EMPTY_NUM = dtMat.Rows[i]["EMPTY_STOCK"].ToDecimal();
			People.list_map.push_back(Map_1);
		}



		Log::Trace("", __FUNCTION__, "CreatePeople() end");
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}

WmsMap WmsMain::CreatePeople_Slab(CString hall_no, CString stock_oper_order, WmsMat InputMat, CDbConnection* conn)
{
	try
	{
		Log::Trace("", __FUNCTION__, "CreatePeople() begin");
		CDbCommand cmd_inq(conn);//数据库操作类定义	

		//--加载区域
		CString sql = "";

		CDataTable dtMat;
		cmd_inq.SetCommandText(sql);
		cmd_inq.ExecuteQuery(dtMat);
		cmd_inq.Close();

		for (int i = 0; i < dtMat.Rows.get_Count(); ++i)
		{
			WmsMap Map_1;
			Map_1.Map_Num = dtMat.Rows[i]["COLUMN"].ToString();
			Map_1.G_ORDER_NUM = dtMat.Rows[i]["ORDER_NUM"].ToDecimal();
			Map_1.G_D_ORDER_NUM = dtMat.Rows[i]["D_ORDER_NUM"].ToDecimal();
			Map_1.G_SG_SIGN_NUM = dtMat.Rows[i]["SG_SIGN_NUM"].ToDecimal();
			Map_1.G_D_SG_SIGN_NUM = dtMat.Rows[i]["D_SG_SIGN_NUM"].ToDecimal();
			Map_1.G_ORDER_CUST_CODE_NUM = dtMat.Rows[i]["ORDER_CUST_CODE_NUM"].ToDecimal();
			Map_1.G_D_ORDER_CUST_CODE_NUM = dtMat.Rows[i]["D_ORDER_CUST_CODE_NUM"].ToDecimal();
			People.list_map.push_back(Map_1);
		}



		Log::Trace("", __FUNCTION__, "CreatePeople() end");
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
	}
}


BM2_FUNCTION_EXPORT
int f_auto_sail(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;

	bcls_ret->Tables[0].Columns.Clear();
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "STOCK_PLACE_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "LOGIC_STOCK_NO");

	try
	{
		if (bcls_rec->Tables.IndexOf("AUTO_INFO_IN") < 0 ||
			bcls_rec->Tables["AUTO_INFO_IN"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "函数f_auto中找不到接收块名[AUTO_INFO_IN]或值为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		CString R_Mat_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["R_MAT_NO"].ToString();
		CString R_Mat_shape = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["R_MAT_SHAPE"].ToString();
		CString R_Stock_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["R_STOCK_NO"].ToString();
		CString R_Hall_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["R_HALL_NO"].ToString();
		CString R_Field_no = bcls_rec->Tables["AUTO_INFO_IN"].Rows[0]["R_FIELD_NO"].ToString();

		Log::Trace("", __FUNCTION__, "传入参数材料类型\t[{0}]", R_Mat_shape);

		if (R_Mat_shape == "C")
		{
			//材料类型为卷

			//--1.初始化材料信息
			WmsMat InfoMat;
			InfoMat.Setinfo(R_Mat_no, conn);

			//--2.加载库图信息
			WmsMain main(conn);
			main.CreatePeople(R_Hall_no, R_Stock_no, InfoMat, conn);

			//--3.推荐最优区域
			WmsMap bestmap = main.People.GetBestMap(InfoMat);

			//--4加载区域库位信息
			bestmap.AddStockplace(conn); 

			//--5.推荐最优库位
			WmsStockplace beststockplace = bestmap.GetBestStock();

		}
		else  if (R_Mat_shape == "S")
		{
			//材料类型为板

			//--1.初始化材料信息
			WmsMat InfoMat;
			InfoMat.Setinfo(R_Mat_no, conn);

			//--2.加载库图信息
			WmsMain main(conn);
			WmsMap bestmap = main.CreatePeople_Slab(R_Hall_no, R_Stock_no, InfoMat, conn);

			//--3.推荐最优库位
			WmsStockplace beststockplace = bestmap.GetBestStock();
		}

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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}