//
// Created by Simon on 2020/5/10.
//

#ifndef QUOTE_MONITOR_MAIN_ENGINE_H
#define QUOTE_MONITOR_MAIN_ENGINE_H

#include <memory>
#include <map>
#include "def.h"
#include "base_data.h"
#include "app_config.h"

using QuoteCallback = void (void*);

class main_engine {
    std::map<base_data, QuoteCallback> _cb_map;
};


#endif //QUOTE_MONITOR_MAIN_ENGINE_H
