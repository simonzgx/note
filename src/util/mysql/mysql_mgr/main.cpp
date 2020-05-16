
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <mysql.h>

void testQuery(MYSQL* mysql);	// 测试查询数据

int main()
{
    MYSQL* mysql = NULL;

    /*初始化MYSQL连接句柄*/
    mysql = mysql_init((MYSQL*)0);
    if (!mysql)
    {
        return 0;
    }

    mysql = mysql_real_connect(mysql, "localhost", "root", "abcd1234",
                               "vnpy", 0, NULL, 0);
    if (mysql)
    {
        printf("connection succellfull the database!\n");
    }
    else
    {
        printf("connection error:%d, %s\n", mysql_errno(mysql), mysql_error(mysql));
    }

    // 查询数据
    testQuery(mysql);

    // 关闭连接
    mysql_close(mysql);

    system("pause");
    return 0;
}

// 测试查询
void testQuery(MYSQL* mysql)
{
    MYSQL_ROW row;
    MYSQL_RES* res = NULL;
    MYSQL_FIELD* fields = NULL;
    int i, field_count;
    const char* sql = "select * from dbbardata";
    int flag = mysql_real_query(mysql, sql, (unsigned long)strlen(sql));
    if (flag)
    {
        printf("Query error:%d, %s\n", mysql_errno(mysql), mysql_error(mysql));
        return;
    }

    // 将查询结果读到内存当中
    res = mysql_store_result(mysql);

    // 获取结果集中的所有字段
    fields = mysql_fetch_fields(res);

    // 字段数量
    field_count = mysql_field_count(mysql);
    for (i = 0; i < field_count; i++)
    {
        printf("%s\t", fields[i].name);
    }

    printf("\n");

    // 遍历结果集的每一行数据
    while (row = mysql_fetch_row(res))
    {
        for (i = 0; i < field_count; i++)
        {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }
}