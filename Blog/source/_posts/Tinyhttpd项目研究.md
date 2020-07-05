---
title: Tinyhttpd项目研究
date: 2020-04-10 17:53:18
index_img: /Picture/Http.png
categories:
- 开源项目
tags:
- 开源项目
---
**参考书本: The Linux Programming interface (A Linux and UNIX System Programming Handbook)**<br>
# Internet Socket Addresses 

## IPv4 socket addresses: struct sockaddr_in < Page:1202 >
An IPv4 socket address is stored in a sockaddr_in structure, defined in <netinet/in.h> <br>
As follows:<br>
```c++
struct in_addr{	/*IPv4 4-byte address*/
	in_addr_t s_addr;  /*Unsigned 32-bit integer*/
};
struct sockaddr_in {	/*IPv4 socket address*/
	sa_family_t	sin_family;	/*Address family(AF_INET)*/
	in_port_t	sin_port;	/*Port number*/
	struct in_addr 	sin_addr	/*IPv4 address*/
	unsigned char	__pad[X];	/*Pad to size of 'sockaddr' structure (16 bytes)*/
};
```
The *sin_family* field always set to *AF_INET*. <br>
The *sin_port* and *sin_addr* fields are the port number and IP address. Both in network byte order. The *in_port_t* and *in_addr_t* types are unsigned integer types, 16 and 32 bits in length respectively <br>

**What is AF_INET?** <br>
*AF_INET* is an address family that is used to designate the type of addresses that your socket can communicate with (in this case, internet Protocol version 4 addresses). when you create a socket, you have to specify it address family, and then you can only use addresses of that type with the socket. The Linux kernel ,for example. supports 29 other address families such as UNIX(AF_UNIX) sockets and IPX (AF_IPX) , and also communications with IRDA and Bluetooth (AF_IRDA and AF_BLUETOOTH).],


## Creating a Socket : *socket()* < Page:946 >
The *socket()* system call creates new socket.<br>

```c++
#include<sys/socket.h>
int socket (int domain, int type , int protocol)
		//Return file discriptor on success , or -1 on error
```
**Argument**<br>
- **domain:** The domain argument specifies the communication domain for socket. The domain argument specifies a communication domain; this selects
the protocol family which will be used for communication.<br>
- **type:** The type argument specifies the socket type. (SOCK_STREAM: create the stream socket . SOCK_DGRAM: create the datagram socket.)
	- SOCK_STREAM : Provides sequenced reliable two-way connection-base on byte steam, An out-of-band data transmission mechanism may be supported .
	- SOCK_DGRAM : supported datagram (connectionless unreliable message of fixed maximum length.)
- **protocol:** The protocol specifies a particular protocol to be used with the socket . Normally only one single protocol exists to support a particular socket type , in which case protocol can specific as 0 . However , it is possible that many protocol exists , in which case , a particular protocol must be specified in this manner , The protocol number to use is specified a communication domain  in which communication domain is to take place.

**The Protocol Number (in file /etc/protocols)**<br>
![protocols.png](protocols.png)<br>

**In fact , I didn't understand the explanation of protocol argument. We can see more detail on these websites following.**<br>
[Linux-Programming's-Manual :Socket(2)](http://man7.org/linux/man-pages/man2/socket.2.html)<br>
[protocols(5) - Linux man page](https://linux.die.net/man/5/protocols)<br>
[What is the HOPOPT protocol and how does socket() work?](https://stackoverflow.com/questions/46804125/what-is-the-hopopt-protocol-and-how-does-socket-work)

## htons() , htonl() , ntohs() , ntohl() < Page : 1199 >
**htons(),htonl(),ntohs(),ntohl() are defined (typecally as macors) for converting integers in either direction between host and network byte order.<br>**

```c++
#include<arpa/inet.h>
uint_16t htons(uint_16t host_uint16);
			 //Return host_uint16 converted to network byte order.
uint_32t htonl(uint_32t host_uint32);
			//Return host_uint32 converted to network byte order. 
uint_16t ntohs (uint_16t net_uint16);
			//Return net_uint16 converted to host byte order.
uint_32t ntohl (uint_32t net_uint32);
			//Return net_uint32 converted to host byte order.
```

### Network Byte Order and Host Byte Order <Page : 1198 >
Byte order possess two different order , **(the Big endian byte order and the little endian byte order)**<br>
![The-difference-byte-order](The-difference-byte-order.png)<br>

Different machine execute different byte order , The byte used on particular machine is called **host byte order**. The most notable example of little endian architecture is X86 .<br>

Since the IP address and the port numbers must be transmitted between and understood , all host on network , the standard byte order must be used. This order is called **network byte order** , and happen to be big endian . The host byte order must be converted to network byte order before that the host byte be copied into relevant filed of a socket address structure .<br>

## INADDR_ANY < Page : 1187 >
The constant INADDR_ANY is so-called IPv4 wildcard address. This willcard IP address is useful for application that bind internet domain socket on multihomed host . If an application on a multihomed host bind a socket to just one of its host's IP address . Then the socket can receive UDP datagram or TCP connection request sent to that IP address. However , we normally want the multihomed host to be able to receive UDP datagram and TCP connection request that specify one of any host's IP address and binding the socket to wildcard address is make this possible. The INADDR_ANY most implementations it as 0.0.0.0.

## Bind a Socket to an Address : *bind()* < Page : 1153 >
The bind() system call bind a socket to an address.<br>
```c++
#include<sys/socket.h>
int bind (int sockfd , const struct socketaddr *addr , socklen_t addrlen);
		//Return 0 on success , or -1 on error.
```

- **The sockfd argument** is a file descriptor obtained from a previous call socket().
- **The addr argument** is a address of structure specifying the address to which this socket to be bound.
- **The addrlen argument** specifies the size of the address structure. The socklen_t data type is an integer type specifies by SUSv3.

## getsockname() and getpeername() < Page : 1263 >
The getsockname()  and  getpeername()  system  calls  return,  respectively,  the  localaddress to which a socket is bound and the address of the peer socket to which thelocal socket is connected<br>
```c++
#include<sys/socket.h>
int getsockname (int sockfd , struct sockaddr *addr , socklen_t *addrlen);
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
			//Both return 0 on success, or –1 on error
```
- **sockfd**: is a file descriptor referring to a socket.
- **addr**: is a pointer to a suitably sized buffer that is used to return a structure containing a socket address.
- **addrlen**: before the call, it should be initialized to the length of the buffer pointed to by addr; On return it contain the number of bytes actually written to this buffer.

**Function of getsockname()**<br>
- Return the socket's address family and the address to which a socket is bound.<br>
- Calling getsockname() also can determine the ephemeral port number that kernel assigned to a socket , when performing an implicit bind of an Internet domain socket. The kernel performs an implicit bind in the following circumstances:
	- After a connect() or listen() call on a TCP socket that have not previously been bound to an address by bind().
	- On the first sendto() on a UDP socket that had not previously been bound to an address.
	- After the bind() call where the port number (sin_port) was specified as 0. In this case , the bind() specifies the IP address for the socket, but the kernel selects an ephemeral port number.

**Function of getpeername()**<br>
- The getpeername() system call return the address of the peer socket on the stream socket connection.<br>

## Listening for incoming Connections : *listen()* < Page : 1156 >
The listen() system call is used to marks the stream socket referred to by file descriptor as passive. The socket will subsequently to be used to accept connections from other (active) socket.
```c++
#include<sys/socket.h>
int listen (int sockfd , int backlog); 
		//Return 0 on success or -1 on error.
```
- **sockfd**: file descriptor of socket that have not previously been performed connect() , or return by a call to accept().
- **backlog**: The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow. If the connect request arrives when the queue is full. The client may receive a error with an indication of ECONNREFUSED.

**Pending Connection**<br>
![Pending-Connection](Pending-Connection.png)<br>
Since the server is busy handling some other client , the Client may call the connect() before the server call the accpet().<br>
The server must record the pending connection information , The connection block until the pending connection is accepted (via accept()) , and remove it from the queue of pending connection.<br>

## Accepting a Connection : *accept()* < Page : 1157 >
The accept() system call accept an incoming connection on listening stream socket referred to by file descriptor sockfd. If no pending connections when the accept() is called , The call blocks until a connection request.<br>
```c++
#include<sys/socket.h>
int accept( int sockfd , struct sockaddr *addr , socklen_t* addrlen);
			//Return file descriptor on success or -1 on error
```
**The key point to understand about *accept()* is that it create a new socket that be used to connect to the peer socket that performed the *connect()*. A file descriptor for the connected socket is returned as the function result of *accept()* call.**
- **The sockfd argument**: sockfd is a file descriptor (listen socket) that have performed been *listen()*.
- **The addr argument**: The addr argument pointer to a structure that is used to return the address of the peer socket.
- **The addrlen argument**: addrlen is a value-result argument . It pointer to an integer that prior to call , must be initialized to the size of the buffer pointed by addr. So that the kernel knows how much space is available to return the socket address. Upon return from accept() , this integer is set to indicate the number of byte of data actually copied into the buffer.

## Socket-Specific I/O System Calls : *recv() and send()* < Page : 1259 >
The *recv()* and *send()* system calls perform I/O on connected sockets. They provide socket-specific functionality that is not available with the traditional *read()* and *write()* system calls.
```c++
#include<sys/socket.h>
ssize_t recv (int sockfd , void *buffer , size_t length , int flags);
		//Return number of byte received , 0 on EOF , -1 on error
ssize_t send (int sockfd , const void *buffer , size_t length , int flags); 
		//Return number of byte sent , or -1 on error
```
**The return value and the first three arguments to *recv()* and *send()* are same as *read()* and *write()*. The last argument flags is a bit mask that modifies the behavior of the I/O operation .**<br>
[About the read() and write()](https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/)<br>

**For recv() the flags include the following:** <br>
*The detail can see the textbook page : 1259*
- MSG_DONTWAIT
- MSG_OOB
- MSG_PEEK
- MSG_WAITALL
- MSG_DONTWAIT
- MSG_MORE
- MSG_NOSIGNAL
- MSG_OOB

## Retrieving File information : stat()< Page : 279 >
The stat() , lstat() , fstat() system call retrieving information about a file.
```c++
#include<sys/stat.h>
int stat( const char *pathname , struct stat *statbuf );
int lstat( const char *pathname , struct stat *statbuf );
int fstat( int fd , struct stat *statbuf);
		//All return 0 on success , or -1 on error
```
**The *struct stat* structure**
```c++
struct stat {
	dev_t	st_dev;		/*IDs of device on which file resides*/
	ino_t	st_ino;		/*I-node number of file*/
	mode_t	st_mode;	/*File type and permissions*/
	nlink_t	st_nlink;	/*Number of (hard) link of file*/
	uid_t	st_uid;		/*User ID of file owner*/
	gid_t	st_gid;		/*Group ID of file owner*/
	dev_t	st_rdev;	/*IDs for device special files*/
	off_t	st_size;	/*Total file size (bytes)*/
	blksize_t st_blksize;	/*Optimal block size for I/O(bytes)*/
	blkcnt_t  st_block;	/*Number of (512B) blocks allocated*/
	time_t	st_atime;	/*Time of last file access*/
	time_t	st_mtime;	/*Time of last file modification*/
	time_t	st_ctime;	/*Time of last status change*/
	}
```
- **stat()**: Return information about a named file.
- **lstat()**: The lstaa() similar to stat(), except that if the named file is symbolic link , information about the link itself is returned, rather than the file to which the link pointer.
- **fstat()**: Return the information about the file referred to by file descriptor.

The stat() and lstat() system call don't require permissions on the file itself , but execute (search) permissions is required on all of parents directories specified in *pathname*. The fstat() always succeeds if provide a valid file descriptor.<br>

## strcasecmp() and strcmp() Compare strings

### strcasecmp()
```c++
#include<string.h>
int strcasecmp(const char* string1 , const char* string2);
```
**Description**:
- The strcasecmp() function compares string1 and string2 **without sensitivity to case.** Alphabetic characters in string1 and string2 is converted to lowercase before comparison.
- The strcasecmp() function operates on null terminated strings. The string arguments to the function are expected to contain a null character ('\0') marking the end of the string.

**Return value of strcasecmp()**

|Value|Meaning|
|----|----|
|Less than 0|string1 less than string2|
|Equal to 0 | string1 equal to string2|
|Greater than 0| string1 greater than string2|

### strcmp()

```c++
#include <string.h>
int strcmp(const char *string1, const char *string2);
```
**Description**:<br>
- The strcmp() function compares string1 and string2. **(Case Sensitive)**
-  The function operates on null-ended strings. The string arguments to the function should contain a null character (\0) that marks the end of the string.<br>

**Return value of strcasecmp()**

|Value|Meaning|
|----|----|
|Less than 0|string1 less than string2|
|Equal to 0 | string1 equal to string2|
|Greater than 0| string1 greater than string2|

### What different between strcmp() and strcasecmp()
- strcmp() : Case-Sensitive
- strcasecmp() : Case-Insensitive
