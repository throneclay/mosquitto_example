#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include "Message.pb.h"
#include "mosquitto.h"
 
using namespace std;

namespace mesg {
// using c apis
// use factory to manage messages
static Message message;

class MessageHandler {
public:
    MessageHandler(const std::string& host, const int port)
        : _host(host)
        , _port(port)
    {}

    ~MessageHandler() {
        if (_connected) {
            mosquitto_lib_cleanup();
            mosquitto_destroy(_mosq);
        }
    }
    void init() {
        // init publish struct
        _mosq = mosquitto_new(nullptr, true, nullptr);
        if (mosquitto_connect(_mosq, _host.c_str(), _port, _keep_alive) != MOSQ_ERR_SUCCESS) {
            std::cerr << "connect error!!" << std::endl;
            _connected = false;
            exit(-1);
        } else {
            _connected = true;
        }
        std::cout << "publish init finished" << std::endl;

        // init recive callback
        mosquitto_lib_init(); 

        auto register_callback = [=] {
            mosquitto_subscribe_callback(
                &MessageHandler::call_back_func, NULL,
                "MessageHandler/#", 0,
                _host.c_str(), _port,
                NULL, 60, true,
                NULL, NULL,
                NULL, NULL);
            // this api can not return??
        };
        std::thread register_callback_thread(register_callback);
        register_callback_thread.detach();
        std::cout << "finished callback init" << std::endl;
    }

    static int call_back_func(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {
        std::cout << "FROM topic: " << msg->topic << std::endl;
        message.ParseFromArray((const char *)msg->payload, msg->payloadlen);
        std::cout << "GOT message:\n" << message.DebugString();
        return 0;
    }

    int pub_message(int idx, const std::string& m) {

        Message message;
        message.set_idx(idx);
        message.mutable_head()->set_name("test");
        message.mutable_body()->add_context(m);
        message.mutable_body()->add_context(m);

        std::string message_string;
        message.SerializeToString(&message_string);
        if (mosquitto_publish(_mosq, nullptr, _mesg_pub_topic.c_str(),
                          static_cast<int>(message_string.length()) + 1,
                          message_string.c_str(), 0, 0) != MOSQ_ERR_SUCCESS) {

            std::cerr  << "MQTT publish error." << std::endl;
            return 1;
        }
        return 0;
    }

private:
    struct mosquitto* _mosq = nullptr;
    std::string _host;
    int _port;
    int _keep_alive = 60;
    bool _connected = false;
    std::string _mesg_pub_topic = "MessageHandler/publish";
    std::string _mesg_cb_topic = "MessageHandler/call_back";
};
}
 
int main(int argc, char *argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    mesg::MessageHandler handle("mqtt.eclipse.org", 1883);
    std::cout << "start init" << std::endl;
    handle.init();
 
    while (true) {
        std::cout << "press and key to exit" << std::endl;
        char c = getchar();
        break;
    }
    std::cout << "exiting" << std::endl;

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
 
    return 0;
}
