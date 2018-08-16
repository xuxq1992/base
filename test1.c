
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
class MyDB;

template <class TYPE>
class CSingleton
{
public:
    static TYPE* Instance(void)
    {
        if(m_pSingleton == NULL)
        {
            m_pSingleton = new CSingleton;
        }
        return &m_pSingleton->m_stInstance;
    }
protected:
    CSingleton(){}
protected:
    TYPE m_stInstance;
    static CSingleton<TYPE>* m_pSingleton;

};

template <class TYPE>
CSingleton<TYPE>* CSingleton<TYPE>::m_pSingleton = NULL;

class MyDB
{
public:
    MyDB(){}
    ~MyDB(){}

    int InitDB();
    int DoOperate();

private:
    MYSQL conn_;
};

int MyDB::DoOperate()
{
    int res;
    MYSQL_RES *result = NULL;
    res = mysql_query(&conn_,"select * from DBRank_1");
    if(res)
        cout << "error" << endl;
    else
        cout << "OK" << endl;
    result = mysql_store_result(&conn_);
    //路径数据的个数
    int rowcount = mysql_num_rows(result);
    cout << rowcount << endl;
    //字段的个数
    int fieldcount = mysql_num_fields(result);
    cout << fieldcount << endl;
    //显示所有字段
    for(int i = 0; i < fieldcount; i++)
    {
        field = mysql_fetch_field_direct(result,i);
        cout << field->name << "\t";
    }
    cout << endl;
    //显示各个字段下的所有数据
    MYSQL_ROW row = NULL;
    row = mysql_fetch_row(result);
    while(NULL != row)
    {
        for(int i=0; i<fieldcount; i++)
        {
            cout << row[i] << "\t"; 
        }
        cout << endl;
        row = mysql_fetch_row(result);
    }
    mysql_close(&conn_);
}

int MyDB::InitDB()
{
    mysql_init(&conn_);
    if(mysql_real_connect(&conn_,"localhost","root","","1_RoleDB",0,NULL,CLIENT_FOUND_ROWS))
    {
        cout << "connect success!" << endl;
    }
}

int main()
{
    MyDB *pMysql = CSingleton<MyDB>::Instance();
    if (NULL == pMysql)
    {
        return 0;
    }
    pMysql->InitDB();
    pMysql->DoOperate();

    return 0;
}
