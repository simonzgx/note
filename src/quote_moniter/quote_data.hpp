//
// Created by Simon on 2020/5/10.
//

#ifndef QUOTE_MONITOR_QUOTEDATA_H
#define QUOTE_MONITOR_QUOTEDATA_H


#include <map>
#include <set>
#include "def.h"
#include "base_data.h"


class CStdQuoteData : public base_data {
public:
    void init(Node &node) override;

    //注册一些函数，比如check
    void register_self(MainEngine &) override;

    static void handle(const CStdQt&);

    static void check_init_quote(CStdQt&);

private:
    int _check_interval;
    std::set<UniqueID> _daytime_contracts;
    std::set<UniqueID> _night_contracts;

};

void CStdQuoteData::init(Node &node) {

}

void CStdQuoteData::register_self(MainEngine &) {

}

void CStdQuoteData::handle(CStdQt &) {

}


#endif //QUOTE_MONITOR_QUOTEDATA_H
