#include "tcp.hpp"

const std::string TcpClient::pingMessage = std::string("some_ping_payload");

void dns_found(const char *name, const ip_addr_t *ipaddr_, void *arg)
{
    TcpClient *client = (TcpClient *)arg;
    ip4_addr *ipaddr = (ip4_addr *)ipaddr_;
    if ((ipaddr) && (ipaddr->addr))
        client->remote_addr_ = *ipaddr;
}

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void print_redis_msg(redis_value val)
{
    std::cout << "redis msg: ";
    if (std::holds_alternative<std::vector<std::variant<int, std::string>>>(val))
    {
        auto subArr = std::get<std::vector<std::variant<int, std::string>>>(val);
        for (size_t k = 0; k < subArr.size(); k++)
        {
            auto subVal = subArr[k];
            if (std::holds_alternative<int>(subVal))
            {
                std::cout << std::get<int>(subVal) << ", ";
            }
            else if (std::holds_alternative<std::string>(subVal))
            {
                std::cout << std::get<std::string>(subVal) << ", ";
            }
        }
    }
    else if (std::holds_alternative<int>(val))
    {
        std::cout << std::get<int>(val);
    }
    else if (std::holds_alternative<std::string>(val))
    {
        std::cout << std::get<std::string>(val);
    }
    std::cout << std::endl;
}

void print_redis_arr(std::deque<redis_value> buffer)
{
    std::cout << "# " << buffer.size() << std::endl;

    for (size_t j = 0; j < buffer.size(); j++)
    {
        print_redis_msg(buffer.at(j));
    }
}

TcpClient::TcpClient(ConfigManager *config, const std::string &domain, uint16_t port)
{
    this->config = config;
    buffer_len_ = 0;
    sent_len_ = 0;
    complete_ = false;
    run_count_ = 0;
    connected_ = false;
    lastPingSent = get_absolute_time();
    lastPingReceived = get_absolute_time();
    ip4_addr resolved_ip;
    tcp_port = port;
    if (ip4addr_aton(domain.c_str(), &resolved_ip) == 1)
    {
        printf("ip4\n");
        remote_addr_ = resolved_ip;
    }
    else
    {
        printf("dns\n");
        resolved_ip.addr = 0;
        ip4_addr dnsServerIp;
        remote_addr_ = resolved_ip;
        ip4addr_aton("1.1.1.1", &dnsServerIp);
        dns_setserver(0, (ip_addr_t *)&dnsServerIp);
        err_t result = dns_gethostbyname(domain.c_str(), (ip_addr_t *)&resolved_ip, dns_found, this);

        if (result == ERR_INPROGRESS)
            while (remote_addr_.addr == 0)
                sleepMs(5);
    }

    tcp_pcb_ = tcp_new_ip_type(IPADDR_TYPE_V4);
    tcp_arg(tcp_pcb_, this);
    tcp_sent(tcp_pcb_, TcpClient::tcpClientSent);
    tcp_recv(tcp_pcb_, TcpClient::tcpClientRecv);
    tcp_err(tcp_pcb_, TcpClient::tcpClientErr);
}

TcpClient::~TcpClient()
{
    if (tcp_pcb_ != nullptr)
        tcp_close(tcp_pcb_);
}

void TcpClient::connect()
{
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(tcp_pcb_, (ip_addr_t *)&remote_addr_, tcp_port, TcpClient::tcpClientConnected);
    cyw43_arch_lwip_end();

    if (err != ERR_OK)
    {
        std::cerr << "TCP connection failed with error code: " << (int32_t)err << "\n";
        throw std::runtime_error("TCP connection failed");
    }
    lastPingSent = get_absolute_time();
    lastPingReceived = get_absolute_time();
    send_command("auth " + config->get_str("redis_user") + " " + config->get_str("redis_pw"));
    wait_message();
}

void TcpClient::send_command(std::string command_)
{
    std::string command(command_);
    if (!ends_with(command, "\r\n"))
        command += "\r\n";

    if (subscribedToChannel.has_value())
    {
        auto oldChannel = subscribedToChannel.value_or("-");
        subscribedToChannel = std::nullopt;
        send_command("UNSUBSCRIBE " + oldChannel);
        wait_message();
    }

    cyw43_arch_lwip_begin();
    tcp_write(tcp_pcb_, command.c_str(), command.length(), TCP_WRITE_FLAG_COPY);
    cyw43_arch_lwip_end();
}

void TcpClient::ping()
{
    lastPingSent = get_absolute_time();
    send_command("ping " + pingMessage);
}

err_t TcpClient::tcpClientSent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    return ERR_OK;
}

err_t TcpClient::tcpClientConnected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    auto *client = static_cast<TcpClient *>(arg);
    if (err != ERR_OK)
    {
        std::cerr << "Connect failed with error code: " << err << "\n";
        return ERR_OK;
    }
    client->connected_ = true;
    std::cout << "Connected.\n";
    return ERR_OK;
}

err_t TcpClient::tcpClientRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    auto *client = static_cast<TcpClient *>(arg);
    if (!p)
        return ERR_OK;

    if (p->tot_len <= 0)
        pbuf_free(p);

    struct pbuf *q = p;
    while (q != nullptr)
    {
        buffer_len new_data = {reinterpret_cast<const uint8_t *>(q->payload), q->len};
        client->buffer.add_new_data(new_data);
        auto buffer_data = client->buffer.get_current_data();

        auto result = parse_redis_msg(buffer_data);
        auto new_redis = result.first;
        auto new_buffer = result.second;
        client->buffer.did_consume_prefix(new_buffer);
        if (new_redis.has_value())
            client->redis_buffer.push_back(new_redis.value());
        q = q->next;
    }

    pbuf_free(p);

    return ERR_OK;
}

void TcpClient::tcpClientErr(void *arg, err_t err)
{
    if (err != ERR_ABRT)
    {
        std::cerr << "TCP client error with code: " << (int32_t)err << "\n";
    }
    else
    {
        std::cerr << "ABRT " << err << "\n";
    }
}

err_t TcpClient::tcpClientClose()
{
    if (tcp_pcb_ != nullptr)
    {
        tcp_arg(tcp_pcb_, nullptr);
        tcp_poll(tcp_pcb_, nullptr, 0);
        tcp_sent(tcp_pcb_, nullptr);
        tcp_recv(tcp_pcb_, nullptr);
        tcp_err(tcp_pcb_, nullptr);
        err_t err = tcp_close(tcp_pcb_);
        if (err != ERR_OK)
        {
            std::cerr << "Close failed with error code: " << err << ", calling abort\n";
            tcp_abort(tcp_pcb_);
            err = ERR_ABRT;
        }
        tcp_pcb_ = nullptr;
    }
    return ERR_OK;
}

std::optional<redis_value> TcpClient::first_message()
{
    // Check if we should send a ping:
    if (absolute_time_diff_us(lastPingSent, get_absolute_time()) > 1000 * 1000 * 30)
        ping();

    // Check if ping failed:
    if (absolute_time_diff_us(lastPingReceived, get_absolute_time()) > 1000 * 1000 * 60)
    {
        std::cout << "Ping timeout. Rebooting..." << std::endl;
        sleepMs(1000);
        reboot();
    }

    if (redis_buffer.size() == 0)
        return std::nullopt;

    redis_value first = redis_buffer.front();
    redis_buffer.pop_front();

    if (std::holds_alternative<std::string>(first) && std::get<std::string>(first) == pingMessage)
    {
        lastPingReceived = get_absolute_time();
        return first_message();
    }

    return first;
}

redis_value TcpClient::wait_message()
{
    while (true)
    {
        auto msg = first_message();
        if (msg.has_value())
            return msg.value();
        sleepMs(1);
    }
}

std::optional<std::string> TcpClient::next_door_message()
{
    return next_channel_message("door_" + config->get_str("redis_door"));
}

std::optional<std::string> TcpClient::next_channel_message(std::string channelName)
{
    if (subscribedToChannel.value_or("-") != channelName)
    {
        send_command("SUBSCRIBE " + channelName);

        wait_message();
        subscribedToChannel = channelName;
    }

    auto data = first_message();
    if (!data.has_value())
        return std::nullopt;
    if (!std::holds_alternative<std::vector<std::variant<int, std::string>>>(data.value()))
        return std::nullopt;
    auto vect = std::get<std::vector<std::variant<int, std::string>>>(data.value());
    if (vect.size() < 3)
        return std::nullopt;

    auto kind = vect[0];
    auto msg = vect[2];
    if (!std::holds_alternative<std::string>(kind))
        return std::nullopt;
    if (std::get<std::string>(kind) != "message")
        return std::nullopt;
    if (!std::holds_alternative<std::string>(msg))
        return std::nullopt;
    return std::get<std::string>(msg);
}
