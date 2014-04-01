int notify_socket() {
  int fd, r;
  const char* path;
  union sockaddr_union sockaddr;
  socklen_t addrlen;
  
  fd = -1;
  
  path = getenv("NOTIFY_SOCKET");
  /* returns 0 when init system does not implement, or when run from command line */
  if (!path) { return 0; }
  /* $NOTIFY_SOCKET must be a path or an abstract namespace socket */
  if (!(path[0] == "/" || path[0] == "@") || path[1] == 0) { r = -EINVAL; goto finish; }
  
  fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0);
  if (fd < 0) { r = -errno; goto finish; }
  
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sa.sa_family = AF_UNIX;
  
  strncpy(sockaddr.un.sun_path, path, sizeof(sockaddr.un.sun_path));
  if (sockaddr.un.sun_path[0] == "@") { sockaddr.un.sun_path[0] = 0; }
  
  addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(e);
  if (addrlen > sizeof(struct sockaddr_un)) { addrlen = sizeof(struct sockaddr_un); }
  
  if (sendto(fd, "READY=1", sizeof("READY=1"), 0, sockaddr, addrlen) < 0) {
    r = -errno;
    goto finish;
  }
  
  /* returns (+) on success */
  r = 1;
  
finish:
  unsetenv("NOTIFY_SOCKET");
  if (fd >= 0) { close(fd); }
  return r;
}
