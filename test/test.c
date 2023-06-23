/*
 * Test program for c11threads.
 */

/*
 * Ensure both USE_THREADS_H and USE_C11THREADS_H are not defined.
 */

#if defined ( USE_THREADS_H ) && defined ( USE_C11THREADS_H )
# error Define only one of USE_THREADS_H or USE_C11THREADS_H
#endif /* if defined ( USE_THREADS_H ) && defined ( USE_C11THREADS_H ) */

 /*
  * OS hints.
  */

 /*
  * Default to C11THREADS on FreeBSD, due to a test
  * failure when using the system threads.h, for now.
  */

#ifdef __FreeBSD__
# ifndef USE_THREADS_H
#  undef USE_C11THREADS_H
#  define USE_C11THREADS_H 1
# endif /* ifndef USE_THREADS_H */
#endif /* ifdef __FreeBSD__ */

/*
 * Manual USE_THREADS_H and USE_C11THREADS_H overrides.
 */

#if defined ( USE_THREADS_H )
# include <threads.h>
# undef USING_THREADS_H
# define USING_THREADS_H 1
#elif defined ( USE_C11THREADS_H )
# include "c11threads.h"
# undef USING_C11THREADS_H
# define USING_C11THREADS_H 1
#endif /* if defined ( USE_THREADS_H ) || defined ( USE_C11THREADS_H ) */

#ifndef __STDC_NO_THREADS__

/*
 * C23 __has_include syntax, but functions in GNU11
 * mode using modern GCC and Clang-based compilers.
 */

# if !defined ( USING_THREADS_H ) && !defined ( USING_C11THREADS_H )
#  if defined __has_include
#   if __has_include ( <threads.h> )
#    include <threads.h>
#    undef USING_THREADS_H
#    define USING_THREADS_H 1
#   endif /* if __has_include ( <threads.h> ) */
#  endif /* if defined __has_include */
# endif /* if !defined ( USING_THREADS_H ) && !defined ( USING_C11THREADS_H ) */

#endif /* ifndef __STDC_NO_THREADS__ */

#if !defined ( USING_THREADS_H ) && !defined ( USING_C11THREADS_H )
# include "c11threads.h"
# undef USING_C11THREADS_H
# define USING_C11THREADS_H 1
#endif /* if !defined ( USING_THREADS_H ) && !defined ( USING_C11THREADS_H ) */

/*
 * Needed for memory leak detection.
 */

#ifdef _WIN32
# define _CRTDBG_MAP_ALLOC
# include <stdlib.h>
# include <crtdbg.h>
#endif /* ifdef _WIN32 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "sir.h"

#define CHK_THRD_EXPECTED(a, b) \
        assert_thrd_expected(a, b, __FILE__, __LINE__, #a, #b)

#define CHK_THRD(a) \
        CHK_THRD_EXPECTED(a, thrd_success)

#define CHK_EXPECTED(a, b) \
        assert_expected(a, b, __FILE__, __LINE__, #a, #b)

#define NUM_THREADS 16

mtx_t      mtx;
mtx_t      mtx2;
cnd_t      cnd;
cnd_t      cnd2;
tss_t      tss;
once_flag  once = ONCE_FLAG_INIT;
int        flag;

void run_thread_test(void);
void run_timed_mtx_test(void);
void run_cnd_test(void);
void run_tss_test(void);
void run_call_once_test(void);

int
main(void)
{

  /*
   * Instantiate the initialization structure.
   */

  loginit si = { 0 };

  /*
   * Configure levels for stdout.
   * Send debug, information, warning, and notice messages there.
   */

  si.d_stdout.levels = LOGL_DEBUG | LOGL_INFO | LOGL_WARN | LOGL_NOTICE;

  /*
   * Configure options for stdout.
   * Don't show the time stamp or process ID.
   */

  si.d_stdout.opts = LOGO_NOTIME | LOGO_NOPID;

  /*
   * Configure levels for stderr.
   * Send error and above there.
   */

  si.d_stderr.levels = LOGL_ERROR | LOGL_CRIT | LOGL_ALERT | LOGL_EMERG;

  /*
   * Configure options for stderr.
   * Don't show the time stamp or process ID.
   */

  si.d_stderr.opts = LOGO_NOTIME | LOGO_NOPID;

  /*
   * Configure options for syslog.
   * Don't send any output there.
   */

  si.d_syslog.levels = 0;

  /*
   * Configure a name to associate with our output.
   */

  strcpy(si.processName, "test-c11threads");

  /*
   * Initialize logging.
   */

  if (!log_init(&si))
    {
      (void)fprintf(stderr, "\rERROR: Unable to initialize logging.\r\n");
      return 1;
    }

  /*
   * Configure and add a log file.
   * Don't show the process name.
   * Send all severity levels there.
   */

  /* logfileid_t fileid1 = log_addfile("test.log", LOGL_ALL, LOGO_NONAME); */

  /* if (NULL == fileid1) */
  /*   { */
  /*     (void)fprintf(stderr, "\rERROR: Unable to initialize logging.\r\n"); */
  /*     return 1; */
  /*   } */

  (void)log_info("tests starting\n");

#ifdef TESTING
  (void)log_warn("TESTING defined");
#endif /* ifdef TESTING */

#ifdef USE_MONOTONIC
  (void)log_warn("USE_MONOTONIC defined");
#endif /* ifdef USE_MONOTONIC */

#ifdef __STDC_NO_THREADS__
  (void)log_info("__STDC_NO_THREADS__ defined");
#endif /* ifdef __STDC_NO_THREADS__ */

#ifdef USE_THREADS_H
  (void)log_warn("C11 standard threads explicitly requested");
#endif /* ifdef USE_THREADS_H */

#ifdef USE_C11THREADS_H
  (void)log_warn("c11threads.h wrapper explicitly requested");
#endif /* ifdef USE_C11THREADS_H */

#ifdef USING_THREADS_H
  (void)log_info("using C11 standard threads\n");
#endif /* ifdef USING_THREADS_H */

#ifdef USING_C11THREADS_H
# ifdef _WIN32
#  ifdef C11THREADS_PTHREAD_WIN32
  (void)log_info("using winpthreads via c11threads.h wrapper\n");
#  else
  (void)log_info("using NT threads via c11threads.h wrapper\n");
#  endif /* ifdef C11THREADS_PTHREAD_WIN32 */
# else
  (void)log_info("using POSIX threads via c11threads.h wrapper\n");
# endif /* ifdef _WIN32 */
#endif /* ifdef USING_C11THREADS_H */

  (void)log_notice("start thread test");
  run_thread_test();
  (void)log_notice("end thread test\n");

  (void)log_notice("start timed mutex test");
  run_timed_mtx_test();
  (void)log_notice("end timed mutex test\n");

  (void)log_notice("start condvar test");
  run_cnd_test();
  (void)log_notice("end condvar test\n");

  (void)log_notice("start thread-specific storage test");
  run_tss_test();
  (void)log_notice("end thread-specific storage test\n");

  (void)log_notice("start call once test");
  run_call_once_test();
  (void)log_notice("end call once test\n");

#if defined( _WIN32 ) && !defined( C11THREADS_PTHREAD_WIN32 )
    c11threads_win32_destroy();
#endif /* if defined( _WIN32 ) && !defined( C11THREADS_PTHREAD_WIN32 ) */

#ifdef _WIN32
    if (_CrtDumpMemoryLeaks())
      {
        abort();
      }
#endif /* ifdef _WIN32 */

  (void)log_info("tests finished");

  return 0;
}

void
assert_thrd_expected(int thrd_status, int expected, const char *file,
                     unsigned int line, const char *expr,
                     const char *expected_str)
{
  const char *thrd_status_str;

  if (thrd_status != expected)
    {
      fflush(stdout);

      switch (thrd_status)
        {
        case thrd_success:
          thrd_status_str = "thrd_success";
          break;

        case thrd_timedout:
          thrd_status_str = "thrd_timedout";
          break;

        case thrd_busy:
          thrd_status_str = "thrd_busy";
          break;

        case thrd_error:
          thrd_status_str = "thrd_error";
          break;

        case thrd_nomem:
          thrd_status_str = "thrd_nomem";
          break;

        default:
          (void)log_error(
            "%s:%u: %s: error %d, expected %s",
            file,
            line,
            expr,
            thrd_status,
            expected_str);
          abort();
        }

      (void)log_error(
        "%s:%u: %s: error %s, expected %s",
        file,
        line,
        expr,
        thrd_status_str,
        expected_str);
      abort();
    }
}

void
assert_expected(int res, int expected, const char *file, unsigned int line,
                const char *expr, const char *expected_str)
{
  if (res != expected)
    {
      fflush(stdout);
      (void)log_error(
        "%s:%u: %s: error %d, expected %s",
        file,
        line,
        expr,
        res,
        expected_str);
      abort();
    }
}

int
tfunc(void *arg)
{
  int              num;
  struct timespec  dur;

  num = (int)(size_t)arg;

  (void)log_debug("hello from thread %d", num);

  dur.tv_sec   = 1;
  dur.tv_nsec  = 0;
  CHK_EXPECTED(thrd_sleep(&dur, NULL), 0);

  (void)log_debug("thread %d done", num);

  return 0;
}

void
run_thread_test(void)
{
  int     i;
  thrd_t  threads[NUM_THREADS];

  for (i = 0; i < NUM_THREADS; i++)
    {
      CHK_THRD(thrd_create(threads + i, tfunc, (void *)(size_t)i));
    }

  for (i = 0; i < NUM_THREADS; i++)
    {
      CHK_THRD(thrd_join(threads[i], NULL));
    }
}

#if !defined( _WIN32 ) || defined( C11THREADS_PTHREAD_WIN32 ) \
  || !defined( C11THREADS_OLD_WIN32API )
  int
  hold_mutex_for_one_second(void *arg)
  {
    struct timespec dur;

    (void)arg;

    CHK_THRD(mtx_lock(&mtx));

    CHK_THRD(mtx_lock(&mtx2));

    flag = 1;

    CHK_THRD(cnd_signal(&cnd));
    CHK_THRD(mtx_unlock(&mtx2));

    dur.tv_sec   = 1;
    dur.tv_nsec  = 0;
    CHK_EXPECTED(thrd_sleep(&dur, NULL), 0);

    CHK_THRD(mtx_unlock(&mtx));

    return 0;
  }

  void
  run_timed_mtx_test(void)
  {
    thrd_t           thread;
    struct timespec  ts;
    struct timespec  dur;

    CHK_THRD(mtx_init(&mtx, mtx_timed));
    CHK_THRD(mtx_init(&mtx2, mtx_plain));
    CHK_THRD(cnd_init(&cnd));

    flag = 0;

    CHK_THRD(thrd_create(&thread, hold_mutex_for_one_second, NULL));

    CHK_THRD(mtx_lock(&mtx2));

    while (!flag)
      {
        CHK_THRD(cnd_wait(&cnd, &mtx2));
      }

    CHK_THRD(mtx_unlock(&mtx2));

    cnd_destroy(&cnd);
    mtx_destroy(&mtx2);

# ifndef _WIN32
#  ifndef _AIX
    CHK_EXPECTED(timespec_get(&ts, TIME_UTC), TIME_UTC);
#  endif /* ifndef _AIX */
# endif /* ifndef _WIN32 */

    ts.tv_nsec += 500000000;

    if (ts.tv_nsec >= 1000000000)
      {
        ++ts.tv_sec;
        ts.tv_nsec -= 1000000000;
      }

    CHK_THRD_EXPECTED(mtx_timedlock(&mtx, &ts), thrd_timedout);

    (void)log_debug("thread has locked mutex & we timed out waiting for it");

    dur.tv_sec   = 1;
    dur.tv_nsec  = 0;

    CHK_EXPECTED(thrd_sleep(&dur, NULL), 0);

# ifndef _WIN32
#  ifndef _AIX
    CHK_EXPECTED(timespec_get(&ts, TIME_UTC), TIME_UTC);
#  endif /* ifndef _AIX */
# endif /* ifndef _WIN32 */

    ts.tv_nsec += 500000000;

    if (ts.tv_nsec >= 1000000000)
      {
        ++ts.tv_sec;
        ts.tv_nsec -= 1000000000;
      }

# ifndef _WIN32
    CHK_THRD(mtx_timedlock(&mtx, &ts));
# endif /* ifndef _WIN32 */

    (void)log_debug("thread no longer has mutex & we grabbed it");

    CHK_THRD(mtx_unlock(&mtx));

    mtx_destroy(&mtx);

    CHK_THRD(thrd_join(thread, NULL));
  }
#endif /* if !defined( _WIN32 ) || defined( C11THREADS_PTHREAD_WIN32 ) || \
        * !defined( C11THREADS_OLD_WIN32API ) */

int
my_cnd_thread_func(void *arg)
{
  int thread_num;

  thread_num = (int)(size_t)arg;

  CHK_THRD(mtx_lock(&mtx));

  ++flag;

  CHK_THRD(cnd_signal(&cnd2));

  do
    {
      CHK_THRD(cnd_wait(&cnd, &mtx));
      (void)log_debug("thread %d: woke up", thread_num);
    }
  while (flag <= NUM_THREADS);

  (void)log_debug(
    "thread %d: flag > NUM_THREADS; incrementing flag and exiting",
    thread_num);

  ++flag;

  CHK_THRD(cnd_signal(&cnd2));
  CHK_THRD(mtx_unlock(&mtx));

  return 0;
}

void
run_cnd_test(void)
{
  struct timespec  dur;
  int              i;
  thrd_t           threads[NUM_THREADS];

  flag         = 0;
  dur.tv_sec   = 0;
  dur.tv_nsec  = 500000000;

  CHK_THRD(mtx_init(&mtx, mtx_plain));
  CHK_THRD(cnd_init(&cnd));
  CHK_THRD(cnd_init(&cnd2));

  for (i = 0; i < NUM_THREADS; i++)
    {
      CHK_THRD(
        thrd_create(threads + i, my_cnd_thread_func, (void *)(size_t)i));
    }

  CHK_THRD(mtx_lock(&mtx));

  while (flag != NUM_THREADS)
    {
      CHK_THRD(cnd_wait(&cnd2, &mtx));
    }

  CHK_THRD(mtx_unlock(&mtx));

  (void)log_info("main thread: threads are ready");

  /*
   * No guarantees, but this might unblock a thread.
   */

  (void)log_debug("main thread: cnd_signal()");

  CHK_THRD(cnd_signal(&cnd));
  CHK_THRD(thrd_sleep(&dur, NULL));

  /*
   * No guarantees, but this might unblock all threads.
   */

  (void)log_debug("main thread: cnd_broadcast()");

  CHK_THRD(cnd_broadcast(&cnd));
  CHK_THRD(thrd_sleep(&dur, NULL));

  CHK_THRD(mtx_lock(&mtx));

  flag = NUM_THREADS + 1;

  CHK_THRD(mtx_unlock(&mtx));

  (void)log_debug("main thread: set flag to NUM_THREADS + 1");

  /*
   * No guarantees, but this might unblock two threads.
   */

  (void)log_debug("main thread: sending cnd_signal() twice");

  CHK_THRD(cnd_signal(&cnd));
  CHK_THRD(cnd_signal(&cnd));
  CHK_THRD(thrd_sleep(&dur, NULL));

  CHK_THRD(mtx_lock(&mtx));

  while (flag == NUM_THREADS + 1)
    {
      CHK_THRD(cnd_wait(&cnd2, &mtx));
    }

  CHK_THRD(mtx_unlock(&mtx));

  (void)log_debug(
    "main thread: woke up, flag != NUM_THREADS + 1; sending "
    "cnd_broadcast() and joining threads");

  CHK_THRD(cnd_broadcast(&cnd));

  for (i = 0; i < NUM_THREADS; i++)
    {
      CHK_THRD(thrd_join(threads[i], NULL));
    }

  cnd_destroy(&cnd2);
  cnd_destroy(&cnd);

  mtx_destroy(&mtx);
}

void
my_tss_dtor(void *arg)
{
  (void)log_debug("dtor: content of tss: %d", (int)(size_t)arg);
  CHK_EXPECTED((int)(size_t)arg, 42);
}

int
my_tss_thread_func(void *arg)
{
  void *tss_content;

  (void)arg;

  tss_content = tss_get(tss);
  (void)log_debug(
    "thread func: initial content of tss: %d",
    (int)(size_t)tss_content);

  CHK_THRD(tss_set(tss, (void *)42));

  tss_content = tss_get(tss);
  (void)log_debug("thread func: content of tss now: %d", (int)(size_t)tss_content);

  CHK_EXPECTED((int)(size_t)tss_content, 42);

  return 0;
}

void
run_tss_test(void)
{
  thrd_t thread;

  CHK_THRD(tss_create(&tss, my_tss_dtor));
  CHK_THRD(thrd_create(&thread, my_tss_thread_func, NULL));
  CHK_THRD(thrd_join(thread, NULL));

  tss_delete(tss);
}

void
my_call_once_func(void)
{
  (void)log_debug("my_call_once_func() was called");
  ++flag;
}

int
my_call_once_thread_func(void *arg)
{
  (void)arg;

  (void)log_debug("my_call_once_thread_func() was called");
  call_once(&once, my_call_once_func);

  return 0;
}

void
run_call_once_test(void)
{
  int     i;
  thrd_t  threads[NUM_THREADS];

  flag = 0;

  for (i = 0; i < NUM_THREADS; i++)
    {
      CHK_THRD(thrd_create(threads + i, my_call_once_thread_func, NULL));
    }

  for (i = 0; i < NUM_THREADS; i++)
    {
      CHK_THRD(thrd_join(threads[i], NULL));
    }

  (void)log_debug("content of flag: %d", flag);

  CHK_EXPECTED(flag, 1);
}
