//
// Created by Simon on 2020/5/10.
//

#ifndef QUOTE_MONITOR_APP_CONFIG_H
#define QUOTE_MONITOR_APP_CONFIG_H

class main_engine;

class app_config {
    friend main_engine;
public:

    explicit app_config(const char *FILE);

    void load();

protected:
    void register_(main_engine &);

};


#endif //QUOTE_MONITOR_APP_CONFIG_H
