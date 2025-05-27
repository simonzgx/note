//
// Created by Administrator on 2020/11/14.
//

#ifndef NOTE_KAFKACLIENT_H
#define NOTE_KAFKACLIENT_H


#include <string>
#include <cassert>
#include <iostream>
#include <memory>
#include <signal.h>
#include <vector>

#include "rdkafkacpp.h"


static volatile sig_atomic_t run = 1;
static bool exit_eof = false;

static void sigterm(int sig) {
    run = 0;
}


class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
    void dr_cb(RdKafka::Message &message) {
        std::string status_name;
        switch (message.status()) {
            case RdKafka::Message::MSG_STATUS_NOT_PERSISTED:
                status_name = "NotPersisted";
                break;
            case RdKafka::Message::MSG_STATUS_POSSIBLY_PERSISTED:
                status_name = "PossiblyPersisted";
                break;
            case RdKafka::Message::MSG_STATUS_PERSISTED:
                status_name = "Persisted";
                break;
            default:
                status_name = "Unknown?";
                break;
        }
        std::cout << "Message delivery for (" << message.len() << " bytes): " <<
                  status_name << ": " << message.errstr() << std::endl;
        if (message.key())
            std::cout << "Key: " << *(message.key()) << ";" << std::endl;
    }
};


class ExampleEventCb : public RdKafka::EventCb {
public:
    void event_cb(RdKafka::Event &event) {
        switch (event.type()) {
            case RdKafka::Event::EVENT_ERROR:
                if (event.fatal()) {
                    std::cerr << "FATAL ";
                    run = 0;
                }
                std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                          event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_STATS:
                std::cerr << "\"STATS\": " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_LOG:
                fprintf(stderr, "LOG-%i-%s: %s\n",
                        event.severity(), event.fac().c_str(), event.str().c_str());
                break;

            default:
                std::cerr << "EVENT " << event.type() <<
                          " (" << RdKafka::err2str(event.err()) << "): " <<
                          event.str() << std::endl;
                break;
        }
    }
};


/* Use of this partitioner is pretty pointless since no key is provided
 * in the produce() call. */
class MyHashPartitionerCb : public RdKafka::PartitionerCb {
public:
    int32_t partitioner_cb(const RdKafka::Topic *topic, const std::string *key,
                           int32_t partition_cnt, void *msg_opaque) {
        return djb_hash(key->c_str(), key->size()) % partition_cnt;
    }

private:

    static inline unsigned int djb_hash(const char *str, size_t len) {
        unsigned int hash = 5381;
        for (size_t i = 0; i < len; i++)
            hash = ((hash << 5) + hash) + str[i];
        return hash;
    }
};


class Configure {
    using string = std::string;

public:

    Configure(string broker, string topic) : broker_(std::move(broker)), topic_(std::move(topic)) {
        assert(!broker_.empty());
        assert(!topic_.empty());
        conf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
        topicConf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
    }

private:
    string broker_;
    string topic_;
    std::shared_ptr<RdKafka::Conf> conf_;
    std::shared_ptr<RdKafka::Conf> topicConf_;
};


#endif //NOTE_KAFKACLIENT_H
