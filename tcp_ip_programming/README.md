
# TCP_IP_Programing

TCP/IP 소켓 프로그래밍 공부 레파지토리입니다. 

- [TCP_IP_Programing](#tcp_ip_programing)
- [저장 경로 설명](#저장-경로-설명)
  - [just_study](#just_study)
  - [mini_project](#mini_project)
- [소켓 프로그래밍 기본 API 실행 흐름](#소켓-프로그래밍-기본-api-실행-흐름)
- [함수 또는 타입 정리.](#함수-또는-타입-정리)
  - [타입 또는 구조체](#타입-또는-구조체)
  - [함수](#함수)
    - [socket](#socket)
    - [bind](#bind)
    - [listen](#listen)
    - [accept](#accept)
    - [connect](#connect)
    - [inet_addr](#inet_addr)
    - [htons & htonl](#htons--htonl)
    - [write](#write)
    - [close](#close)
    - [getservent](#getservent)
    - [sendto](#sendto)
    - [recvfrom](#recvfrom)
- [IP 주소를 표현하는 법](#ip-주소를-표현하는-법)
- [기타](#기타)


# 저장 경로 설명

## just_study

* 해당 경로는 TCP/IP 소켓 프로그래밍 공부한 코드들을 정리한 폴더입니다. 

1. [basic](./just_study/1.basic/): TCP/IP 소켓 프로그래밍의 기초 틀을 구현한 코드입니다. 
2. [hello_world](./just_study/2.hello_world/): 서버가 클라이언트에게 "hello world" 문자열을 전달하여 이를 출력하는 코드입니다. 
3. [ls](./just_study/3.ls/): 클라이언트가 서버에게 "ls" 문자열을 전달하면, 서버는 클라이언트에게 서버 기준 현재 경로의 파일 리스트를 전달하는 코드입니다. 
4. [socketpair](./just_study/4.socketpair/): `socketpair()` 함수의 기초 사용법을 구현한 코드입니다. 
5. [getservent](./just_study/5.getservent/): `getservent()` 함수의 기초 사용법을 구현한 코드입니다. 
6. [network_ordering](./just_study/6.network_ordering/): 네트워크 바이트 순서와 빅엔디안, 리틀 엔디안을 이해할 수 있도록 구현한 코드입니다. 
7. [transport_address](./just_study/7.transport_address/): 주소 형식 변환을 제공하는 함수를 사용하는 코드입니다. 
8. [echo_service_tcp](./just_study/8.echo_service_tcp/): 소켓 프로그래밍을 사용하여 echo system을 개발한 코드입니다. (TCP)
9. [echo_service_udp](./just_study/9.echo_service_udp/): UDP 를 이용한 소켓 프로그래밍을 사용하여 echo system을 개발한 코드입니다. 
10. [broadcasting_udp](./just_study/10.broadcasting_udp/): UDP 통신을 이용한 브로드캐스팅 코드입니다.
11. [fork_example](./just_study/11.fork_example/): fork() 함수 예제
12. [fork_echo_service](./just_study/12.fork_echo_service/): fork()를 사용한 TCP echo system 개발 코드입니다. 
13. [signal_example](./just_study/13.signal_example/): 시그널 등록 및 처리 예제 코드입니다. 
14. [talk_program](./just_study/14.talk_program/): 채팅 프로그램.
15. [pipeline_test](./just_study/15.pipeline_test/): 파이프라인을 이용한 통신 예제.

## mini_project

* 해당 경로는 TCP/IP 소켓 프로그래밍 미니 프로젝트를 정리한 폴더입니다. 

1. [single_chatting](./mini_project/single_chatting/): 1대1 채팅 프로그램 (폐기 - 설계 미스) 


# 소켓 프로그래밍 기본 API 실행 흐름 

![flow](https://t1.daumcdn.net/cfile/tistory/995C23465C7DD7E30B)

# 함수 또는 타입 정리.

## 타입 또는 구조체 

* 주소 체계 종류: AF_INET, AF_INET6, AF_UNIX, AF_LOCAL, AF_LINK(Low level socket 인터페이스를 이용), AF_PACKET(IPX 노벨 프로토콜을 사용한다.)
* sockaddr: 소켓의 주소를 담는 기본 구조체
```c
struct sockaddr {
    u_short sa_family;      /* 주소체계를 구분하기 위한 변수, 2 bytes, u_short는 unsigned short를 의미 */
    char sa_data[14];       /* 실제 주소를 저장하기 위한 변수. 14 bytes*/
}
```
* sockaddr_in: sockaddr 구조체에서 sa_family가 AF_INET인 경우 (즉. 주소 체계를 IPv4를 사용하는 경우)
```c
struct sockaddr_in{
    short sin_family;           /* 주소 체계: AF_INET */
    u_short sin_port;           /* 포트 번호를 의미, 범위는 0 ~ 65535, 2 bytes */
    struct in_addr sin_addr;    /* 호스트 IP 주소 */
    char sin_zero[8];           /* 8 bytes dummy data. 반드시 모두 0으로 채워져 있어야 한다. 이유는 sockaddr 구조체와 크기를 일치시키기 위함이다. */
}

struct in_addr{
    u_long s_addr;              /* 32비트 IP 주소를 저장할 변수 */
}
```
* sockaddr_in6: AF_INET6인 경우 (IPv6 주소체계를 사용하는 경우)
```c
struct sockaddr_in6{
    sa_family_t     sin6_family;        /* AF_INET6 */
    in_port_t       sin6_port;          /* IPv6 포트를 저장, ntohs() 또는 htons()로 조작하는 것이 좋음 */
    uint32_t        sin6_flowinfo;      /* IPv6 헤더와 연관된 트래픽 클래스와 플로루 레이블을 포함 */
    struct          in6_addr sin6_addr; /* 16 bytes IPv6 주소를 저장하는 변수 */
    uint32_t        sin6_scope_id;      /* sin6_addr의 주소 범위에 따라 달라지는 식별자를 포함할 수 있다. */
}

struct in6_addr{
    unsigned char s6_addr[16];          /* IPv6 address store */
}
```
* sockaddr_un: 하나의 시스템에서 서로 다른 프로세스 사이의 통신에 사용되는 소켓의 주소 지정하는데 사용되는 구조체 (AF_UNIX, AF_LOCAL 인 경우)
```c
struct sockaddr_un{
    sa_family_t     sun_family;             /* AF_UNIX */
    char            sun_path[UNIX_PATH_MAX] /* 파일 시스템 경로 지정. NULL로 끝나는 문자열이여야 한다. 경로의 최대 길이는 NULL terminator를 포함해서 108 bytes 이다. */
}
```
* servent: `/etc/services`의 정보를 읽어 사용되는 구조체로, 현재 열려있는 포트 및 서비스 명을 나타낸다. 
```c
struct servent{
    char     *s_name;     /* 서비스 이름, 포트명 */
    char     **s_aliases; /* 서비스의 별명 리스트 */
    int      s_port;      /* 서비스가 이용하는 포트 */
    char     *s_proto;    /* 서비스가 사용하는 프로토콜 */
}
```
* hostent: 도메인 정보를 나타내는 구조체 이다. 
```c
struct hostent{
    char    *h_name;        /* 공식 도메인 이름 */
    char    **h_aliases;    /* 별명들 */
    int     h_addrtype;     /* 주소 유형 */
    int     h_length;       /* 주소 길이 */
    char    **h_addr_list;  /* 네트워크 바이트 순서의 이진값의 IP 주소 */
}
```

## 함수 

### socket

* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int socket(int domain, int type, int protocol);`
* 설명: 소켓을 생성하여 반환한다. 
* 입력 변수:
  * `int domain`:  프로토콜 family를 지정해주는 변수
    * `AF_UNIX`(프로토콜 내부), `AF_INET`(IPv4), `AF_INET6`(IPv6)
  * `int type`: 어떤 타입의 프로토콜을 사용할 것인지 설정
    * `SOCK_STREAM`(TCP), `SOCK_DGRAM`(UDP), `SOCK_RAW`(사용자 정의)
  * `int protocal`: 어떤 프로토콜의 값을 결정하는 것
    * `0`, `IPPROTO_TCP`(TCP), `IPPROTO_UDP`(UDP)
* 반환값:
  * 에러: `-1`
  * 정상: 소켓에 대한 파일디스크립터
    * 파일디스크립터 참고 자료: https://twofootdog.tistory.com/51

### bind

* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);`
* 설명: 소켓을 바인딩 하는 함수 (쉽게 말하자면 소켓과 해당 프로세스를 묶음으로서 해당 프로세스가 소켓을 통해 다른 컴퓨터로부터 연결을 받아들일 수 있게 한다.)
* 입력 변수:
  * `int sockfd`: 소켓 식별자 또는 소켓 디스크립터
  * `struct sockaddr *myaddr`: `sockaddr` 포인터
  * `socklen_t addrlen`: `myaddr` 구조체 크기
* 반환값:
  * 에러: `-1`
  * 성공: `0`

### listen
 * 헤더 파일: `<sys/socket.h>`
 * 함수 원형: `int listen(int sock, int backlog);`
 * 설명: 클라이언트가 연결요청 할 수 있는 상태로 만들어주는 함수.
 * 입력 변수:
   * `int sock`: 연결요청 대기상태에 두고자 하는 소켓의 파일 디스크립터 전달. 이 함수의 인자로 전달된 디스크립터의 소켓이 서버 소켓이 된다.
   * `int backlog`: 연결요청 대시 큐(queue)의 크기정보 전달. 
     * Ex. 5가 전달되면 큐의 크기가 5가 되어서 클라이언트의 연결 요청을 5개까지 대기시킬 수 있다. 
 * 반환값
   * 에러: `-1`
   * 성공: `0`

### accept
* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int accept(int sockfd, struct sockaddr* addr, socklent_t *addrlen);`
* 설명: 해당 소켓에 연결 요청이 왔을 때 연결을 받아들이는 함수이다. 
* 입력 변수:
  * `int sockfd`: 연결을 기다리는 소켓 디스크립터.
  * `struct sockaddr* addr`: 받아들인 클라이언트 주소 및 포트 정보가 저장될 구조체 주소값.
  * `socklen_t *addrlen`: `sockaddr` 구조체의 길이가 저장된 변수의 주소값
* 반환값
  * 에러: `-1`
  * 성공: 새로운 소켓 디스크립터

### connect

* 헤더 파일: `<sys/types.h>`, `<sys/socket.h>`
* 함수 원형: `int connect(int socket, const struct sockaddr *address, socklen_t address_len);`
* 설명: 클라이언트 소켓을 생성하고, 서버로 연결을 요청
* 입력 변수:
  * `int socket`: 클라이언트 소켓의 파일 디스크립터.
  * `const struct sockaddr *address`: 연결 요청을 보낼 서버의 주소 정보를 지닌 구조체 변수의 포인터.
  * `socklen_t address_len`: 포인터가 가리키는 주소 정보 구조체 변수의 크기. 
* 반환값
  * 에러: `-1`
  * 성공: `0`

### inet_addr
* 헤더 파일: `<arpa/inet.h>`
* 함수 원형: `in_addr_t inet_addr(const char *cp);`
* 설명: 인터넷 주소 cp를 32bit 바이너리 주소로 변경한 값을 리턴한다. 
* 입력 변수:
  * `const char *cp`: 인터넷 주소 char 값
* 반환값:
  * 에러: `-1`
  * 성공: `바이너리 주소 값 (이떄 리턴 값은 네트워크 바이트 오더를 따른다.)`

### htons & htonl
* 헤더 파일: `<netinet/in.h>`
* 함수 원형: 
  * `unsigned short int htons(unsigned short int hostshort);`
  * `unsigned long int htonl(unsigned long int hostshort);`
* 설명
  * `htonl()`함수는 long intger(일반적으로 4byte)데이터를 네트워크 byte order로 변경한다.
  * `htons()`함수는 short intger(일반적으로 2byte)데이터를 네트워크 byte order로 변경한다.

### write
* 헤더 파일: `<unistd.h>`
* 함수 원형: `ssize_t write(int fildes, const void *buf, size_t nbyte);`
* 설명: 파일에 데이터를 출력하는 함수.
* 입력 변수:
  * `int fileds`: 데이터 전송 영역을 나타내는 파일 디스크립터
  * `const void *buf`: 전송할 데이터를 가지고 있는 버퍼의 포인터
  * `size_t nbyte`: 전송할 데이터의 바이트 수 
* 반환값
  * 에러: `-1`
  * 성공: 전달한 바이트 수

### close
* 헤더 파일: `<unistd.h>`
* 함수 원형: `int close(int fildes);`
* 설명: 열려 있는 파일을 닫는다.
* 입력 변수:
  * `int fildes`: 파일 디스크립터
* 반환값
  * 에러: `-1`
  * 성공: `0`

### getservent
* 헤더 파일: `<netdb.h>`
* 함수 원형: `struct servent *getservent();`
* 설명: `/etc/services`에 있는 서비스 목록들을 가져와 주는 함수
* 입력 변수: 없음
* 반환값
  * 에러: `NULL`
  * 성공: `struct servent 포인터`

### sendto
* 헤더 파일: `<sys/socket.h>`
* 함수 원형: `ssize_t sendto(int socket, const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);`
* 설명: UDP/IP 통신에서 소켓으로 데이터를 전송하는 함수
* 입력 변수:
  * `int socket`: 소켓 디스크립터
  * `const void *buffer`: 전송할 데이터
  * `size_t length`: 데이터의 바이트 단위 길이
  * `int flags`: 전송을 위한 옵션
  * `const struct sockaddr *dest_addr`: 목적지 주소 정보
  * `socklen_t dest_len`: 목적지 주소 정보 크기
* 반환값
  * 에러: `-1`, 아래의 내용은 상세 errno의 대한 내용이다.
```
     [EACCES]           The SO_BROADCAST option is not set on the socket and a broadcast address is given as the destination.
     [EAGAIN]           The socket is marked non-blocking and the requested operation would block.
     [EBADF]            An invalid descriptor is specified.
     [ECONNRESET]       A connection is forcibly closed by a peer.
     [EFAULT]           An invalid user space address is specified for a parameter.
     [EHOSTUNREACH]     The destination address specifies an unreachable host.
     [EINTR]            A signal interrupts the system call before any data is transmitted.
     [EMSGSIZE]         The socket requires that message be sent atomically, and the size of the message to be sent makes this impossible. IOV_MAX.
     [ENETDOWN]         The local network interface used to reach the destination is down.
     [ENETUNREACH]      No route to the network is present.
     [ENOBUFS]          The system is unable to allocate an internal buffer.  The operation may succeed when buffers become available.
     [ENOBUFS]          The output queue for a network interface is full.  This generally indicates that the interface has stopped sending, but may be caused by transient congestion.
     [ENOTSOCK]         The argument socket is not a socket.
     [EOPNOTSUPP]       socket does not support (some of) the option(s) specified in flags.
     [EPIPE]            The socket is shut down for writing or the socket is connection-mode and is no longer connected.  In the latter case, and if the socket is of type SOCK_STREAM, the SIGPIPE signal is generated to the calling thread.
     [EADDRNOTAVAIL]    The specified address is not available or no longer available on this machine.
```
  * 성공: 실제 전송한 바이트 수 
  * `int flags`의 옵션들
    * MSG_OOB: SOCK_STRAM에서만 사용되며 out-of-band 데이터로 전송될 수 있음을 의미
    * MSG_DONTROUTE: 데이터는 라우팅 될수 없음으로 지정
    * MSG_DONTWAIT: NONE BLOCKING 통신이 가능하도록 설정
    * MSG_NOSIGNAL: 상대방과 연결이 끊겼을 때, SIGPIPE 시스널을 받지 않도록 한다.


### recvfrom 
* 헤더 파일: `<sys/socket.h>`
* 함수 원형: `ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict address, socklen_t *restrict address_len);`
* 설명: UDP 통신 프로그램으로부터 데이터를 수신한다. 
* 입력 변수: 
  * `int socket`: 소켓 디스크립터
  * `void *restrict buffer`: 수신한 데이터를 저장할 버퍼
  * `size_t length`: 읽을 데이터 크기
  * `int flags`: 읽을 데이터 유형 또는 읽는 방법에 대한 옵션
  * `struct sockaddr *restrict address`: 접속한 상대 시스템의 socket 주소 정보를 저장할 버퍼 
  * `socklen_t *restrict address_len`
    * INPUT: `address`의 크기를 설정한 후에 호출한다. 
    * OUTPUT: 호출 후에는 실제 할당된 `address`의 크기가 저장된다. 

* 반환값
  * 에러: `-1`, 아래 내용은 errno에 대한 내용이다.
```
    * EAGAIN or EWOULDBLOCK : time out이 발생하였거나 socket에 non-blocking이 설정된 경우 
    * EBADF : sockfd가 유효하지 않는 descriptor 
    * ECONNREFUSED : network상에서 접속 거부된 경우 
    * EFAULT : 읽을 데이터를 저장할 buf가 유효하지 않은 메모리인 경우 
    * EINTR : signal이 발생하여 interrupted 된 경우 
    * EINVAL : 파라미터의 값이 유효하지 않은 경우 
    * ENOMEM : 데이터 수신을 위한 메모리 할당이 되지 않은 경우 (recvmsg(2)호출시) 
    * ENOTCONN : connect(2), accept(2)가 호출되지 않은 상태인 경우 
    * ENOTSOCK : sockfd가 socket descriptor가 아닌 일반 파일인 경우
```
  * 성공: 실제로 수신한 데이터 길이를 리턴한다. 


# IP 주소를 표현하는 법

# 기타

* 네트워크 바이트 순서: Big Endian 
* AF_INET vs PF_INET
  * AF_INET: 주소 체계
  * PF_INET: 프로토콜 체계 
* `close()` vs `shutdown()`