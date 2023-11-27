#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <string>
#include <array>
#include <cstring>
#include <iostream>
#include <deque>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "parse_redis_msg.hpp"
#include "data_buffer.hpp"
#include "hardware/timer.h"
#include "sleep.hpp"
#include "reboot.hpp"
#include "config_manager.hpp"

class TcpClient
{
public:
    TcpClient(ConfigManager *config, const std::string &domain, uint16_t port);
    ~TcpClient();

    TcpClient(const TcpClient &) = delete;            // Delete copy constructor
    TcpClient &operator=(const TcpClient &) = delete; // Delete copy assignment operator

    TcpClient(TcpClient &&) noexcept = default;            // Default move constructor
    TcpClient &operator=(TcpClient &&) noexcept = default; // Default move assignment operator

    void connect();

    ip4_addr remote_addr_;

    std::optional<redis_value> first_message();
    redis_value wait_message();
    void ping();
    void send_command(std::string);
    std::optional<std::string> next_channel_message(std::string);
    std::optional<std::string> next_door_message();

private:
    static const std::string pingMessage;

    absolute_time_t lastPingSent;
    absolute_time_t lastPingReceived;

    ConfigManager *config;
    static constexpr int POLL_TIME_S = 2;
    struct tcp_pcb *tcp_pcb_;
    int buffer_len_;
    int sent_len_;
    bool complete_;
    int run_count_;
    bool connected_;
    int tcp_port;
    std::optional<std::string> subscribedToChannel = std::nullopt;
    std::deque<redis_value> redis_buffer = std::deque<redis_value>();
    DataBuffer buffer = DataBuffer();

    // Callback functions
    static err_t tcpClientSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static err_t tcpClientConnected(void *arg, struct tcp_pcb *tpcb, err_t err);
    static err_t tcpClientRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static void tcpClientErr(void *arg, err_t err);

    err_t tcpClientClose();
};

#endif // TCPCLIENT_HPP
