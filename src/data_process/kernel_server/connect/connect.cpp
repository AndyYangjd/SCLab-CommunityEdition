#include "connect.hpp"

connect_info_t ConnectInfo::getConnectInfo() {
    connect_info_t re_conn_info{};

    return re_conn_info;
}

bool ConnectInfo::loadConnectFile(const std::string& in_file_path) {
    connect_file_path_ = in_file_path;

    return true;
}

std::string ConnectInfo::makeConnectUrl() {
    std::string re_url{};

    return re_url;
}

void ConnectInfo::connectIopub() {

}

void ConnectInfo::connectShell() {

}

void ConnectInfo::connectStdin() {

}

void ConnectInfo::connectHb() {

}

void ConnectInfo::connectControl() {

}

