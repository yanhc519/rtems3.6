/*
 *  RTEMS Fake System Calls
 *
 *  This file contains "fake" versions of the system call routines
 *  which are reference by many libc implementations.  Once a routine
 *  has been implemented in terms of RTEMS services, it should be
 *  taken out of this file.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>  /* only for puts */

#include <rtems.h>
#include <rtems/libio.h>

#ifdef RTEMS_NEWLIB

int __rtems_fstat(int _fd, struct stat* _sbuf)
{
#ifdef HAVE_BLKSIZE
  _sbuf->st_blksize = 0;
#endif

  /*
   * For now assume stdin/stdout/stderr are always a TTY line
   *
   *  From Eric Norum:
   *
   *  The `fix' is not complete.  It still doesn't properly handle
   *  file descriptors for any files/devices other  than the console
   *  serial lines.....
   */
  if (_fd <= 2) {
    _sbuf->st_mode = S_IFCHR;
  } else {
    switch (rtems_file_descriptor_type (_fd)) {
    case RTEMS_FILE_DESCRIPTOR_TYPE_FILE:
      _sbuf->st_mode = S_IFREG;
      break;

    case RTEMS_FILE_DESCRIPTOR_TYPE_SOCKET:
#if !defined(__GO32__)
      _sbuf->st_mode = S_IFSOCK;
      break;
#endif

    default:
      puts( "__rtems_fstat -- unknown file descriptor type" );
      assert( 0 );
    }
  }
  return 0;
}

#if !defined(RTEMS_UNIX)
int stat( const char *path, struct stat *buf )
{
  if ( strncmp( "/dev/", path, 5 ) ) {
    return -1;
  }
  return __rtems_fstat( 0, buf );
}

int link( const char *existing, const char *new )
{
  /* always fail */
  return -1;
}

int unlink( const char *path )
{
  /* always fail */
  return -1;
}

char *getcwd( char *_buf, size_t _size)
{
/*  assert( FALSE ); */
  errno = ENOSYS;
  return 0;
}
#endif

#endif
