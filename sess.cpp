#include "sess.h"
#include "encoding.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include "CRC32.h"
// 16-bytes magic number for each packet
const size_t nonceSize = 16;

// 4-bytes packet checksum
const size_t crcSize = 4;

// overall crypto header size
const size_t cryptHeaderSize = nonceSize + crcSize;

// maximum packet size
//const size_t mtuLimit = 1500;

// FEC keeps rxFECMulti* (dataShard+parityShard) ordered packets in memory
const size_t rxFECMulti = 3;
#define KCP_DEBUG 0
void
dump(char *tag,  byte *text, size_t len)
{
#if KCP_DEBUG
    int i;
    printf("%s: \n", tag);
    for (i = 0; i < len; i++){
        if ((i % 16) == 0 && i != 0){
            printf("\n");
        }
        if ((i % 4) == 0 && ((i%16) != 0)){
            printf(" ");
            
        }
        printf("%02x", (uint8_t)text[i]);
        
        
    }
    printf("\n");
#endif
}
#define debug_print(args ...) if (KCP_DEBUG) fprintf(stderr, args)
UDPSession *
UDPSession::Dial(const char *ip, uint16_t port) {
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip, &(saddr.sin_addr));

    if (ret == 1) { // do nothing
    } else if (ret == 0) { // try ipv6
        return UDPSession::dialIPv6(ip, port);
    } else if (ret == -1) {
        return nullptr;
    }

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    
    
   
    
    if (sockfd == -1) {
        return nullptr;
    }
    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr)) < 0) {
        close(sockfd);
        return nullptr;
    }

    struct sockaddr_in localAddress;
    //socklen_t addressLength;
    socklen_t addressLength = sizeof(localAddress);
    getsockname(sockfd, (struct sockaddr*)&localAddress,   \
                &addressLength);
    printf("local address: %s\n", inet_ntoa( localAddress.sin_addr));
    printf("local port: %d\n", (int) ntohs(localAddress.sin_port));
    
    return UDPSession::createSession(sockfd);
}

UDPSession *
UDPSession::DialWithOptions(const char *ip, uint16_t port, size_t dataShards, size_t parityShards) {
    auto sess = UDPSession::Dial(ip, port);
    if (sess == nullptr) {
        return nullptr;
    }

    if (dataShards > 0 && parityShards > 0) {
        sess->fec = FEC::New(rxFECMulti * (dataShards + parityShards), dataShards, parityShards);
        sess->shards.resize(dataShards + parityShards, nullptr);
        sess->dataShards = dataShards;
        sess->parityShards = parityShards;
    }
    return sess;
};
// DialWithOptions connects to the remote address "raddr" on the network "udp" with packet encryption with block
UDPSession*
UDPSession::DialWithOptions(const char *ip, uint16_t port, size_t dataShards, size_t parityShards,BlockCrypt *block){
    auto sess = UDPSession::DialWithOptions(ip, port, dataShards, parityShards);
    if (sess == nullptr) {
        return nullptr;
    }
    
    sess->block = block;
    return sess;
}

UDPSession *
UDPSession::dialIPv6(const char *ip, uint16_t port) {
    struct sockaddr_in6 saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin6_family = AF_INET6;
    saddr.sin6_port = htons(port);
    if (inet_pton(AF_INET6, ip, &(saddr.sin6_addr)) != 1) {
        return nullptr;
    }

    int sockfd = socket(PF_INET6, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        return nullptr;
    }
    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in6)) < 0) {
        close(sockfd);
        return nullptr;
    }

    return UDPSession::createSession(sockfd);
}

UDPSession *
UDPSession::createSession(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        return nullptr;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        return nullptr;
    }

    UDPSession *sess = new(UDPSession);
    sess->m_sockfd = sockfd;
    sess->m_kcp = ikcp_create(IUINT32(rand()), sess);
    sess->m_kcp->output = sess->out_wrapper;
    
    return sess;
}

//recv data from udp socket
///
void
UDPSession::Update(uint32_t current) noexcept {
    for (;;) {
        ssize_t n = recv(m_sockfd, m_buf, sizeof(m_buf), 0);
        if (n < 0) {
            perror("fopen( \"nulltest.txt\", \"r\" )");
            debug_print("kcp udp socket read error");
        }
        if (n > 0) {
            dump((char*)"UDP Update", m_buf, n);
            //change by abigt
            bool dataValid = false;
            size_t outlen = n;
            //size_t orgsize = n;
            char *out = (char *)m_buf;
            //nonceSize = 16
            //outlen -= nonceSize;
            out += nonceSize;
            uint32_t sum = 0;
            if (block != NULL) {
                
                block->decrypt(m_buf, n, &outlen);
                
                memcpy(&sum, (uint8_t *)out, sizeof(uint32_t));
                out += crcSize;
                int32_t checksum = crc32_kr(0,(uint8_t *)out, n-cryptHeaderSize);
                if (checksum == sum){
                    dataValid = true;
                    
                }
            }else {
                
                memcpy(&sum, (uint8_t *)out, sizeof(uint32_t));
                out += crcSize;
                int32_t checksum = crc32_kr(0,(uint8_t *)out, n - cryptHeaderSize);
                if (checksum == sum){
                    dataValid = true;
                }
                
            }
            if (outlen != n) {
                debug_print("decrypt error outlen= %lu n = %lu\n",outlen,n);
            }
            if (dataValid == true) {
                memmove(m_buf, m_buf + cryptHeaderSize, n-cryptHeaderSize);
                KcpInPut(n - cryptHeaderSize);
            }
            
        } else {
            if (n == 0){
                debug_print("UDPSession recv nil");
            }
            break;
        }
    }
    m_kcp->current = current;
    ikcp_flush(m_kcp);
}
void
UDPSession::KcpInPut(size_t len) noexcept {
    if (fec.isEnabled()) {
        // decode FEC packet
        auto pkt = fec.Decode(m_buf, static_cast<size_t>(len));
        if (pkt.flag == kcptypeData) {
            auto ptr = pkt.data->data();
            // we have 2B size, ignore for typeData
            dump((char*)"ikcp input data:", (byte *) (ptr + 2),  pkt.data->size() - 2);
            ikcp_input(m_kcp, (char *) (ptr + 2), pkt.data->size() - 2);
        }
        
        // allow FEC packet processing with correct flags.
        if (pkt.flag == kcptypeData || pkt.flag == typeFEC) {
            // input to FEC, and see if we can recover data.
            auto recovered = fec.Input(pkt);
            
            // we have some data recovered.
            for (auto &r : recovered) {
                // recovered data has at least 2B size.
                if (r->size() > 2) {
                    auto ptr = r->data();
                    // decode packet size, which is also recovered.
                    uint16_t sz;
                    decode16u(ptr, &sz);
                    
                    // the recovered packet size must be in the correct range.
                    if (sz >= 2 && sz <= r->size()) {
                        // input proper data to kcp
                        dump((char*)"ikcp input data2:", (byte *) (ptr + 2), sz - 2);
                        ikcp_input(m_kcp, (char *) (ptr + 2), sz - 2);
                        
                        // std::cout << "sz:" << sz << std::endl;
                    }
                }
            }
        }
    } else { // fec disabled
        ikcp_input(m_kcp, (char *) (m_buf), len);
    }
}
void
UDPSession::Destroy(UDPSession *sess) {
    if (nullptr == sess) return;
    if (0 != sess->m_sockfd) { close(sess->m_sockfd); }
    if (nullptr != sess->m_kcp) { ikcp_release(sess->m_kcp); }
    delete sess;
}

ssize_t
UDPSession::Read(char *buf, size_t sz) noexcept {
    if (m_streambufsiz > 0) {
        size_t n = m_streambufsiz;
        if (n > sz) {
            n = sz;
        }
        memcpy(buf, m_streambuf, n);

        m_streambufsiz -= n;
        if (m_streambufsiz != 0) {
            memmove(m_streambuf, m_streambuf + n, m_streambufsiz);
        }
        return n;
    }

    int psz = ikcp_peeksize(m_kcp);
    if (psz <= 0) {
        return 0;
    }

    if (psz <= sz) {
        return (ssize_t) ikcp_recv(m_kcp, buf, int(sz));
    } else {
        ikcp_recv(m_kcp, (char *) m_streambuf, sizeof(m_streambuf));
        memcpy(buf, m_streambuf, sz);
        m_streambufsiz = psz - sz;
        memmove(m_streambuf, m_streambuf + sz, psz - sz);
        return sz;
    }
}

ssize_t
UDPSession::Write(const char *buf, size_t sz) noexcept {
    int n = ikcp_send(m_kcp, const_cast<char *>(buf), int(sz));
    if (n == 0) {
        return sz;
    } else return n;
}

int
UDPSession::SetDSCP(int iptos) noexcept {
    iptos = (iptos << 2) & 0xFF;
    return setsockopt(this->m_sockfd, IPPROTO_IP, IP_TOS, &iptos, sizeof(iptos));
}
char * UDPSession::getLocalIPAddr() noexcept{
    struct sockaddr_in localAddress;
    //socklen_t addressLength;
    socklen_t addressLength = sizeof(localAddress);
    getsockname(this->m_sockfd, (struct sockaddr*)&localAddress,   \
                &addressLength);
    printf("local address: %s\n", inet_ntoa( localAddress.sin_addr));
    printf("local port: %d\n", (int) ntohs(localAddress.sin_port));
    return inet_ntoa( localAddress.sin_addr);
}
void
UDPSession::SetStreamMode(bool enable) noexcept {
    if (enable) {
        this->m_kcp->stream = 1;
    } else {
        this->m_kcp->stream = 0;
    }
}
// output pipeline entry
// steps for output data processing:
// 0. Header extends
// 1. FEC
// 2. CRC32
// 3. Encryption
// 4. WriteTo kernel
int
UDPSession::out_wrapper(const char *buf, int len, struct IKCPCB *, void *user) {
    assert(user != nullptr);
    UDPSession *sess = static_cast<UDPSession *>(user);
    //test no cover
    dump((char*)"UDPSession kcp packet", (byte*)buf, (size_t)len);
    if (sess->fec.isEnabled()) {    // append FEC header
        BlockCrypt *block = sess->block;
        
        
        // extend to len + fecHeaderSizePlus2
        // i.e. 4B seqid + 2B flag + 2B size
        //crypto none also add nonce and crc
        
        uint8_t *nonce = BlockCrypt::ramdonBytes(nonceSize);
        
        memcpy(sess->m_buf, nonce, nonceSize);
        free(nonce);
        byte *ptr = sess->m_buf + cryptHeaderSize;
        
        memcpy(ptr + fecHeaderSizePlus2, buf, static_cast<size_t>(len));
        sess->fec.MarkData(ptr, static_cast<uint16_t>(len));
        
        int32_t sum =  crc32_kr(0,ptr  ,len +  fecHeaderSizePlus2);
        memcpy(sess->m_buf + nonceSize, &sum, 4);
        //dump("UDPSession header", (char *)header, nonceSize + crcSize);
        //sess->output(header, nonceSize + crcSize );
        
        
        
        // FEC calculation
        // copy "2B size + data" to shards
        auto slen = len + 2;
        sess->shards[sess->pkt_idx] =
        std::make_shared<std::vector<byte>>(&sess->m_buf[fecHeaderSize + cryptHeaderSize], &sess->m_buf[fecHeaderSize + cryptHeaderSize + slen]);
        size_t outlen = 0;
        if (block != NULL) {
            block->encrypt(sess->m_buf, len + fecHeaderSizePlus2 + cryptHeaderSize, &outlen);
            sess->output(sess->m_buf, outlen);
        }else {
            sess->output(sess->m_buf, len + fecHeaderSizePlus2 + cryptHeaderSize);
        }
        
        
        // count number of data shards
        sess->pkt_idx++;
        if (sess->pkt_idx == sess->dataShards) { // we've collected enough data shards
            sess->fec.Encode(sess->shards);
            // send parity shards
            //should add nonce and crc?
            
            for (size_t i = sess->dataShards; i < sess->dataShards + sess->parityShards; i++) {
                // append header to parity shards
                // i.e. fecHeaderSize + data(2B size included)
                // add nonce and crc
                uint8_t *nonce = BlockCrypt::ramdonBytes(nonceSize);
                
                memcpy(sess->m_buf, nonce, nonceSize);
                
                free(nonce);
                memcpy(ptr + fecHeaderSize, sess->shards[i]->data(), sess->shards[i]->size());
                sess->fec.MarkFEC(ptr);
                
                int32_t sum =  crc32_kr(0,ptr  ,sess->shards[i]->size()  +  fecHeaderSize);
                memcpy(sess->m_buf + nonceSize, &sum, 4);
                
                //go version write ecc to remote?
                if (block != NULL) {
                    block->encrypt(sess->m_buf, sess->shards[i]->size() + fecHeaderSize + cryptHeaderSize, &outlen);
                    sess->output(sess->m_buf,outlen);
                }else {
                    sess->output(sess->m_buf, sess->shards[i]->size() + fecHeaderSize + cryptHeaderSize);
                }
                
            }
            
            // reset indexing
            sess->pkt_idx = 0;
        }
        
        
        
        
    } else { // No FEC, just send raw bytes,
        //kcp-tun no use this
        sess->output(buf, static_cast<size_t>(len));
    }
    return 0;
}

ssize_t
UDPSession::output(const void *buffer, size_t length) {
    dump((char*)"UDPSession write socket", (byte *)buffer, length);
    ssize_t n = send(m_sockfd, buffer, length, 0);
    if (n != length) {
        debug_print("not full send\n");
    }
    if (n==-1) {
        perror("fopen( \"nulltest.txt\", \"r\" )");
        
    }
    return n;
}
