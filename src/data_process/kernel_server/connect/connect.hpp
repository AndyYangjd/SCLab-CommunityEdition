#ifndef SCLAB_TEST_CONNECT_HPP
#define SCLAB_TEST_CONNECT_HPP

#include <string>

using channel_socket_types_t = struct {
    std::string hb_type;
    std::string shell_type;
    std::string iopub_type;
    std::string stdin_type;
    std::string control_type;
};

using connect_info_t = struct {
    int32_t shell_port_;
    int32_t iopub_port_;
    int32_t stdin_port_;
    int32_t control_port_;
    int32_t hb_port_;
    std::string ip_;
    std::string key_;
    std::string transport_;
    std::string signature_scheme_;
    std::string kernel_name_;
};


class ConnectInfo final {
public:
    ConnectInfo() = default;
    ~ConnectInfo() = default;

    connect_info_t getConnectInfo();
    bool loadConnectFile(const std::string&);
private:
    std::string connect_file_path_{};

    std::string makeConnectUrl();
    void connectIopub();
    void connectShell();
    void connectStdin();
    void connectHb();
    void connectControl();
};

#endif //SCLAB_TEST_CONNECT_HPP
