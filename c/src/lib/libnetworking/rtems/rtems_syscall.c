/*
 *  $Id$
 */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/proc.h>
#include <sys/filio.h>

#include <net/if.h>
#include <net/route.h>

/*
 *********************************************************************
 *              Map RTEMS file descriptor to BSD socket              *
 *********************************************************************
 */
struct fdsock {
	int		indexFreeNext;
	struct socket	*sock;
};
static struct fdsock *fdsock;
static int fdsockCount;
static int indexFreeHead = -1;

/*
 * Convert an RTEMS file descriptor to a BSD socket pointer.
 */
static struct socket *
fdToSocket (int fd)
{
	int i;
	struct socket *s;

	if ((fd < 0)
	 || (rtems_file_descriptor_type(fd) != RTEMS_FILE_DESCRIPTOR_TYPE_SOCKET)
	 || ((i = rtems_file_descriptor_base(fd)) >= fdsockCount)
	 || ((s = fdsock[i].sock) == NULL)) {
		errno = EBADF;
		return NULL;
	}
	return s;
}

/*
 * Enlarge the size of the file-descritor/socket pointer map.
 */
static int
enlargeFdMap (void)
{
	struct fdsock *nfdsock;
	int i;

	nfdsock = realloc (fdsock, sizeof *fdsock * (fdsockCount + 20));
	if (nfdsock == NULL) {
		errno = ENFILE;
		return 0;
	}
	fdsock = nfdsock;
	for (i = fdsockCount, fdsockCount += 20 ; i < fdsockCount ; i++) {
		fdsock[i].sock = NULL;
		fdsock[i].indexFreeNext = indexFreeHead;
		indexFreeHead = i;
	}
	return 1;
}

/*
 * Create a file descriptor for a new socket
 */
static int
makeFd (struct socket *s)
{
	int i;

	if ((indexFreeHead < 0) && !enlargeFdMap ())
		return -1;
	i = indexFreeHead;
	indexFreeHead = fdsock[i].indexFreeNext;
	fdsock[i].sock = s;
	return rtems_make_file_descriptor(i,RTEMS_FILE_DESCRIPTOR_TYPE_SOCKET);
}

/*
 * Package system call argument into mbuf.
 */
static int
sockargstombuf (struct mbuf **mp, const void *buf, int buflen, int type)
{
	struct mbuf *m;

	if ((u_int)buflen > MLEN)
		return (EINVAL);
	m = m_get(M_WAIT, type);
	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	memcpy (mtod(m, caddr_t), buf, buflen);
	*mp = m;
	if (type == MT_SONAME) {
		struct sockaddr *sa;
		sa = mtod(m, struct sockaddr *);
		sa->sa_len = buflen;
	}
	return 0;
}

/*
 *********************************************************************
 *                       BSD-style entry points                      *
 *********************************************************************
 */
int
socket (int domain, int type, int protocol)
{
	int fd = -1;
	int error;
	struct socket *so;

	rtems_bsdnet_semaphore_obtain ();
	error = socreate(domain, &so, type, protocol, NULL);
	if (error == 0) {
		fd = makeFd (so);
		if (fd < 0)
			soclose (so);
	}
	else {
		errno = error;
		fd = -1;
	}
	rtems_bsdnet_semaphore_release ();
	return fd;
}

int
bind (int s, struct sockaddr *name, int namelen)
{
	int error;
	int ret = -1;
	struct socket *so;
	struct mbuf *nam;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) != NULL) {
		error = sockargstombuf (&nam, name, namelen, MT_SONAME);
		if (error == 0) {
			error = sobind (so, nam);
			if (error == 0)
				ret = 0;
			else
				errno = error;
			m_freem (nam);
		}
		else {
			errno = error;
		}
	}
	rtems_bsdnet_semaphore_release ();
	return ret;
}

int
connect (int s, struct sockaddr *name, int namelen)
{
	int error;
	int ret = -1;
	struct socket *so;
	struct mbuf *nam;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		errno = EALREADY;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = sockargstombuf (&nam, name, namelen, MT_SONAME);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = soconnect (so, nam);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if ((so->so_state & SS_NBIO) && (so->so_state & SS_ISCONNECTING)) {
		m_freem(nam);
		errno = EINPROGRESS;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	while ((so->so_state & SS_ISCONNECTING) && so->so_error == 0) {
		soconnsleep (so);
	}
	if (error == 0) {
		error = so->so_error;
		so->so_error = 0;
	}
	so->so_state &= ~SS_ISCONNECTING;
	m_freem (nam);
	if (error == 0)
		ret = 0;
	rtems_bsdnet_semaphore_release ();
	return ret;
}

int
listen (int s, int backlog)
{
	int error;
	int ret = -1;
	struct socket *so;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) != NULL) {
		error = solisten (so, backlog);
		if (error == 0)
			ret = 0;
		else
			errno = error;
	}
	rtems_bsdnet_semaphore_release ();
	return ret;
}

int
accept (int s, struct sockaddr *name, int *namelen)
{
	int fd;
	struct socket *head, *so;
	struct mbuf *nam;

	rtems_bsdnet_semaphore_obtain ();
	if ((head = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if ((head->so_options & SO_ACCEPTCONN) == 0) {
		errno = EINVAL;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
        if ((head->so_state & SS_NBIO) && head->so_comp.tqh_first == NULL) {
                errno = EWOULDBLOCK;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
        while (head->so_comp.tqh_first == NULL && head->so_error == 0) {
                if (head->so_state & SS_CANTRCVMORE) {
                        head->so_error = ECONNABORTED;
                        break;
                }
		soconnsleep (head);
        }
	if (head->so_error) {
		errno = head->so_error;
		head->so_error = 0;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}

	so = head->so_comp.tqh_first;
	TAILQ_REMOVE(&head->so_comp, so, so_list);
	head->so_qlen--;

	fd = makeFd (so);
	if (fd < 0) {
		TAILQ_INSERT_HEAD(&head->so_comp, so, so_list);
		head->so_qlen++;
		soconnwakeup (head);
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	so->so_state &= ~SS_COMP;
	so->so_head = NULL;

	nam = m_get(M_WAIT, MT_SONAME);
	(void) soaccept(so, nam);
	if (name) {
		 /* check length before it is destroyed */
		if (*namelen > nam->m_len)
			*namelen = nam->m_len;
		memcpy (name, mtod(nam, caddr_t), *namelen);
	}
	m_freem(nam);
	rtems_bsdnet_semaphore_release ();
	return (fd);

}

/*
 * All `transmit' operations end up calling this routine.
 */
ssize_t
sendmsg (int s, const struct msghdr *mp, int flags)
{
	int ret = -1;
	int error;
	struct uio auio;
	struct iovec *iov;
	struct socket *so;
	struct mbuf *to, *control;
	int i;
	int len;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_WRITE;
	auio.uio_offset = 0;
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if ((auio.uio_resid += iov->iov_len) < 0) {
			errno = EINVAL;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	if (mp->msg_name) {
		error = sockargstombuf (&to, mp->msg_name, mp->msg_namelen, MT_SONAME);
		if (error) {
			errno = error;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	else {
		to = NULL;
	}
	if (mp->msg_control) {
		if (mp->msg_controllen < sizeof (struct cmsghdr)) {
			errno = EINVAL;
			if (to)
				m_freem(to);
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
		sockargstombuf (&control, mp->msg_control, mp->msg_controllen, MT_CONTROL);
	}
	else {
		control = NULL;
	}
	len = auio.uio_resid;
	error = sosend (so, to, &auio, (struct mbuf *)0, control, flags);
	if (error) {
		if (auio.uio_resid != len && (error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error) 
		errno = error;
	else
		ret = len - auio.uio_resid;
	if (to)
		m_freem(to);
	rtems_bsdnet_semaphore_release ();
	return (ret);
}

/*
 * Send a message to a host
 */
ssize_t
sendto (int s, const void *buf, size_t buflen, int flags, const struct sockaddr *to, int tolen)
{
	struct msghdr msg;
	struct iovec iov;

	iov.iov_base = (void *)buf;
	iov.iov_len = buflen;
	msg.msg_name = (caddr_t)to;
	msg.msg_namelen = tolen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	return sendmsg (s, &msg, flags);
}

/*
 * Send a message to a connected host
 */
ssize_t
send (int s, const void *buf, size_t buflen, int flags)
{
	return sendto (s, buf, buflen, flags, NULL, 0);
}

/*
 * All `receive' operations end up calling this routine.
 */
ssize_t
recvmsg (int s, struct msghdr *mp, int flags)
{
	int ret = -1;
	int error;
	struct uio auio;
	struct iovec *iov;
	struct socket *so;
	struct mbuf *from = NULL, *control = NULL;
	int i;
	int len;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_offset = 0;
	auio.uio_resid = 0;
	iov = mp->msg_iov;
	for (i = 0; i < mp->msg_iovlen; i++, iov++) {
		if ((auio.uio_resid += iov->iov_len) < 0) {
			errno = EINVAL;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	len = auio.uio_resid;
	mp->msg_flags = flags;
	error = soreceive (so, &from, &auio, (struct mbuf **)NULL, 
			mp->msg_control ? &control : (struct mbuf **)NULL,
			&mp->msg_flags);
	if (error) {
		if (auio.uio_resid != len && (error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error) {
		errno = error;
	}
	else {
		ret = len - auio.uio_resid;
		if (mp->msg_name) {
			len = mp->msg_namelen;
			if ((len <= 0) || (from == NULL)) {
				len = 0;
			}
			else {
				if (len > from->m_len)
					len = from->m_len;
				memcpy (mp->msg_name, mtod(from, caddr_t), len);
			}
			mp->msg_namelen = len;
		}
		if (mp->msg_control) {
			struct mbuf *m;
			caddr_t ctlbuf;

			len = mp->msg_controllen;
			m = control;
			mp->msg_controllen = 0;
			ctlbuf = (caddr_t) mp->msg_control;

			while (m && (len > 0)) {
				unsigned int tocopy;

				if (len >= m->m_len) 
					tocopy = m->m_len;
				else {
					mp->msg_flags |= MSG_CTRUNC;
					tocopy = len;
				}
				memcpy(ctlbuf, mtod(m, caddr_t), tocopy);
				ctlbuf += tocopy;
				len -= tocopy;
				m = m->m_next;
			}
			mp->msg_controllen = ctlbuf - mp->msg_control;
		}
	}
	if (from)
		m_freem (from);
	if (control)
		m_freem (control);
	rtems_bsdnet_semaphore_release ();
	return (ret);
}

/*
 * Receive a message from a host
 */
ssize_t
recvfrom (int s, void *buf, size_t buflen, int flags, const struct sockaddr *from, int *fromlen)
{
	struct msghdr msg;
	struct iovec iov;
	int ret;

	iov.iov_base = buf;
	iov.iov_len = buflen;
	msg.msg_name = (caddr_t)from;
	msg.msg_namelen = *fromlen;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	ret = recvmsg (s, &msg, flags);
	if ((from != NULL) && (fromlen != NULL) && (ret >= 0))
		*fromlen = msg.msg_namelen;
	return ret;
}

/*
 * Receive a message from a connected host
 */
ssize_t
recv (int s, void *buf, size_t buflen, int flags)
{
	return recvfrom (s, buf, buflen, flags, NULL, NULL);
}

int
setsockopt (int s, int level, int name, const void *val, int len)
{
	struct socket *so;
	struct mbuf *m = NULL;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (len > MLEN) {
		errno = EINVAL;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (val) {
		error = sockargstombuf (&m, val, len, MT_SOOPTS);
		if (error) {
			errno = error;
			rtems_bsdnet_semaphore_release ();
			return -1;
		}
	}
	error = sosetopt(so, level, name, m);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	rtems_bsdnet_semaphore_release ();
	return 0;
}

int
getsockopt (int s, int level, int name, void *aval, int *avalsize)
{
	struct socket *so;
	struct mbuf *m = NULL, *m0;
	char *val = aval;
	int i, op, valsize;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (val)
		valsize = *avalsize;
	else
		valsize = 0;
	if (((error = sogetopt(so, level, name, &m)) == 0) && val && valsize && m) {
		op = 0;
		while (m && op < valsize) {
			i = valsize - op;
			if (i > m->m_len)
				i = m->m_len;
			memcpy (val, mtod(m, caddr_t), i);
			op += i;
			val += i;
			m0 = m;
			MFREE (m0, m);
		}
		*avalsize = op;
	}
	if (m != NULL)
		(void) m_free(m);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	rtems_bsdnet_semaphore_release ();
	return 0;
}

static int
getpeersockname (int s, struct sockaddr *name, int *namelen, int pflag)
{
	struct socket *so;
	struct mbuf *m;
	int len = *namelen;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	m = m_getclr(M_WAIT, MT_SONAME);
	if (m == NULL) {
		errno = ENOBUFS;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (pflag)
		error = (*so->so_proto->pr_usrreqs->pru_peeraddr)(so, m);
	else
		error = (*so->so_proto->pr_usrreqs->pru_sockaddr)(so, m);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	if (len > m->m_len) {
		len = m->m_len;
		*namelen = len;
	}
	memcpy (name, mtod(m, caddr_t), len);
	m_freem (m);
	rtems_bsdnet_semaphore_release ();
	return 0;
}

int
getpeername (int s, struct sockaddr *name, int *namelen)
{
	return getpeersockname (s, name, namelen, 1);
}
int
getsockname (int s, struct sockaddr *name, int *namelen)
{
	return getpeersockname (s, name, namelen, 0);
}

/*
 ************************************************************************
 *                 RTEMS EXTERNAL I/O HANDLER ROUTINES                  *
 ************************************************************************
 */
static int
rtems_bsdnet_close (int fd)
{
	struct socket *so;
	int error;
	int i;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (fd)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	i = rtems_file_descriptor_base(fd);
	fdsock[i].indexFreeNext = indexFreeHead;;
	indexFreeHead = i;
	error = soclose (so);
	rtems_bsdnet_semaphore_release ();
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

static int
rtems_bsdnet_read (int fd, void *buffer, unsigned32 count)
{
	return recv (fd, buffer, count, 0);
}

static int
rtems_bsdnet_write (int fd, const void *buffer, unsigned32 count)
{
	return send (fd, buffer, count, 0);
}

static int
so_ioctl (struct socket *so, unsigned32 command, void *buffer)
{
	switch (command) {
	case FIONBIO:
		if (*(int *)buffer)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		return 0;

	case FIONREAD:
		*(int *)buffer = so->so_rcv.sb_cc;
		return 0;
	}

	if (IOCGROUP(command) == 'i')
		return ifioctl (so, command, buffer, NULL);
	if (IOCGROUP(command) == 'r')
		return rtioctl (command, buffer, NULL);
	return (*so->so_proto->pr_usrreqs->pru_control)(so, command, buffer, 0);
}

static int
rtems_bsdnet_ioctl (int fd, unsigned32 command, void *buffer)
{
	struct socket *so;
	int error;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = fdToSocket (fd)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
	error = so_ioctl (so, command, buffer);
	rtems_bsdnet_semaphore_release ();
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

rtems_libio_handler_t rtems_bsdnet_io_handler = {
	NULL,			/* open */
	rtems_bsdnet_close,	/* close */
	rtems_bsdnet_read,	/* read */
	rtems_bsdnet_write,	/* write */
	rtems_bsdnet_ioctl,	/* ioctl */
	NULL,			/* lseek */
};

