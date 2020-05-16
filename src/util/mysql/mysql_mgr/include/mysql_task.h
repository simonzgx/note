#pragma once

class CDatabaseMysql;

enum EMysqlError
{
    EME_ERROR = -1,
    EME_OK,
    EME_NOT_EXIST,
    EME_EXIST,
};

class mysql_task
{
public:
    mysql_task()=default;
    virtual ~mysql_task()=default;

public:
    virtual void Execute(CDatabaseMysql* poConn) = 0;
    virtual void Reply() = 0;
    virtual void Release() { delete this; };
};