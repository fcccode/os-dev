; Socket types
SOCK_STREAM	= 1
SOCK_DGRAM	= 2
SOCK_RAW	= 3	; not supported by the kernel

; IP protocols
IPPROTO_IP	= 0
IPPROTO_ICMP	= 1	; not supported by the kernel
IPPROTO_TCP	= 6
IPPROTO_UDP	= 17

; Address families
AF_UNSPEC	= 0
AF_INET		= 2	; IPv4
;AF_INET6	= 28	; IPv6 (not supported)

PF_UNSPEC	= AF_UNSPEC
PF_INET		= AF_INET
;PF_INET6	= AF_INET6

; Flags for addrinfo
AI_PASSIVE	= 1
AI_CANONNAME	= 2
AI_NUMERICHOST	= 4
AI_NUMERICSERV	= 8
AI_ADDRCONFIG	= 0x400

; internal definition
AI_SUPPORTED	= 0x40F

struct sockaddr_in
	sin_len		db	?	; uint8_t
	sin_family	db	?	; sa_family_t
	sin_port	dw	?	; in_port_t
	sin_addr	dd	?	; struct in_addr
	sin_zero	rb	8	; zero
ends

struct addrinfo
	ai_flags	dd	?	; bitmask of AI_*
	ai_family	dd	?	; PF_*
	ai_socktype	dd	?	; SOCK_*
	ai_protocol	dd	?	; 0 or IPPROTO_*
	ai_addrlen	dd	?	; length of ai_addr
	ai_canonname	dd	?	; char*
	ai_addr		dd	?	; struct sockaddr*
	ai_next		dd	?	; struct addrinfo*
ends

EAI_ADDRFAMILY	= 1
EAI_AGAIN	= 2
EAI_BADFLAGS	= 3
EAI_FAIL	= 4
EAI_FAMILY	= 5
EAI_MEMORY	= 6
EAI_NONAME	= 8
EAI_SERVICE	= 9
EAI_SOCKTYPE	= 10
EAI_BADHINTS	= 12
EAI_PROTOCOL	= 13
EAI_OVERFLOW	= 14
