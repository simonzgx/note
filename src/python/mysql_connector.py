import pymysql
import sqlalchemy
import pandas as pd


class MysqlConfig:
    host: str
    port: int
    user: str
    password: str
    database: str
    charset: str

    def __init__(self, host: str, user: str, password: str, database: str):
        self.host = host
        self.port = 3306
        self.user = user
        self.charset = "utf8"
        self.password = password
        self.database = database


class MysqlClient:

    def __init__(self, config: MysqlConfig):
        self.config = config
        self.engine = sqlalchemy.create_engine(self.from_config())

    def QueryDF(self, sql: str) -> pd.DataFrame:
        return pd.read_sql_query(sql, self.engine)

    def InsertDF(self, df: pd.DataFrame, table: str) -> None:
        df.to_sql(table, self.engine, index=False)

    def from_config(self) -> str:
        return ('mysql+pymysql://%s:%s@%s:%d/%s' % self.config.user, self.config.password,
                self.config.host, self.config.port, self.config.database)


def test():
    print("hello")


if __name__ == "__main__":
    test()
