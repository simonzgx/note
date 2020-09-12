//
// Created by Administrator on 2020/9/9.
//

#ifndef HTTPCLIENT_REQUEST_H
#define HTTPCLIENT_REQUEST_H

#include <memory>
#include <functional>
#include <string>
#include "net/include/"
#include "curl/curl.h"

namespace httpclient {

    class Curl;


    class Request : public std::enable_shared_from_this<Request> {
        using DataCallback = std::function<void(const char *, int)>;
        using DoneCallback = std::function<void(Request *, int)>;

        Request(Curl *, const char *url);

        ~Request();

        void setDataCallback(const DataCallback &cb) { dataCb_ = cb; }

        void setDoneCallback(const DoneCallback &cb) { doneCb_ = cb; }

        void setHeaderCallback(const DataCallback &cb) { headerCb_ = cb; }

        void headerOnly();

        void setRange(const std::string &range);

        template<typename OPT>
        int setopt(OPT opt, long p) {
            return curl_easy_setopt(curl_, opt, p);
        }

    private:
        DataCallback dataCb_;
        DoneCallback doneCb_;
        DataCallback headerCb_;

        class Curl *owner_;

        CURL *curl_;
    };

    using RequestPtr = std::shared_ptr<Request>;

    class Curl {
    public :
        enum Option
        {
            kCURLnossl = 0,
            kCURLssl   = 1,
        };

        explicit Curl();
    };
}


#endif //HTTPCLIENT_REQUEST_H
