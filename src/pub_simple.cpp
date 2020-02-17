#include <iostream>
#include <fstream>
#include <vector>
#include "Message.pb.h"
#include "mosquitto.h"
 
using namespace std;

namespace mesg {

class MessageHandler {
public:
    MessageHandler(const std::string& host, const int port)
        : _host(host)
        , _port(port)
    {}

    ~MessageHandler() {
        if (_connected) {
            mosquitto_destroy(_mosq);
        }
    }
    void init() {
        // init publish struct
        _mosq = mosquitto_new(nullptr, true, nullptr);
        if (mosquitto_connect(_mosq, _host.c_str(), _port, _keep_alive) != MOSQ_ERR_SUCCESS) {
            std::cerr << "connect error!!" << std::endl;
        } else {
            _connected = true;
        }
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
};
}
 
int main(int argc, char *argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    mesg::MessageHandler handle("mqtt.eclipse.org", 1883);
    handle.init();
    handle.pub_message(1, "test"); 

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
 
    return 0;
}
