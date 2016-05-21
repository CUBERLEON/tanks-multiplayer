#include "ClientCore.hpp"

#include "sys/Debug.hpp"

ClientCore::ClientCore(const std::string& ip, unsigned short port)
: m_isConnected(false), m_serverIp(ip), m_serverPort(port), m_tcpSocket(new sf::TcpSocket()), m_udpSocket(new sf::UdpSocket())
{
    m_localPort = m_tcpSocket->getLocalPort();
    m_udpSocket->bind(m_localPort);
}

ClientCore::~ClientCore()
{
    delete m_tcpSocket;
    delete m_udpSocket;
}

void ClientCore::update(float updateTime) {
    // send({{"type", "input"}, {"input", }})
}

void ClientCore::connect(const std::string& name) {
    sf::Socket::Status status = m_tcpSocket->connect(sf::IpAddress(m_serverIp), m_serverPort);
    if (status != sf::Socket::Done) {
        throw std::runtime_error("Couldn't connect to server " + m_serverIp + ":" + std::to_string(m_serverPort));
        exit(1);
    }
    
    send({{"type", "connection"}, {"name", name}});
    json respose;
    if (receive(respose))
        Debug::info(respose.dump(4));
    
    m_isConnected = true;
}

void ClientCore::disconnect() {
    m_isConnected = false;
    m_tcpSocket->disconnect();
}

bool ClientCore::send(const json& r) {
    std::string data = r.dump();
    if (data.size() + 1 > TCP_MAX_LENGTH)
        return false;
    sf::Socket::Status res = m_tcpSocket->send(data.c_str(), data.size() + 1);
    
    if (res == sf::Socket::Disconnected)
        m_isConnected = false;
        
    return res == sf::Socket::Done;
}

bool ClientCore::receive(json& r) {
    sf::Socket::Status res;
    try {
        char data[TCP_MAX_LENGTH];
        std::size_t received;
        res = m_tcpSocket->receive(data, TCP_MAX_LENGTH, received);
        
        if (res == sf::Socket::Disconnected)
            m_isConnected = false;
        if (res == sf::Socket::Done)
            r = json::parse(data);
    } catch (const std::exception& e) {
        Debug::error(e.what());
        return false;
    }
    return res == sf::Socket::Done;
}

bool ClientCore::udpSend(const json& r) {
    std::string data = r.dump();
    if (data.size() + 1 > sf::UdpSocket::MaxDatagramSize)
        return false;
    sf::Socket::Status res = m_udpSocket->send(data.c_str(), data.size() + 1, sf::IpAddress(m_serverIp), m_serverPort);
    
    if (res == sf::Socket::Disconnected)
        m_isConnected = false;
        
    return res == sf::Socket::Done;
}

bool ClientCore::udpReceive(json& r) {
    sf::Socket::Status res;
    try {
        char data[sf::UdpSocket::MaxDatagramSize];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short port;
        res = m_udpSocket->receive(data, sf::UdpSocket::MaxDatagramSize, received, sender, port);
        
        if (sender.toString() != m_serverIp || port != m_serverPort)
            return false;
        
        if (res == sf::Socket::Disconnected)
            m_isConnected = false;
        if (res == sf::Socket::Done)
            r = json::parse(data);
    } catch (const std::exception& e) {
        Debug::error(e.what());
        return false;
    }
    return res == sf::Socket::Done;
}