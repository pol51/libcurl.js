#pragma once
#include "curl.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef void CURLM;

typedef enum {
  CURLM_CALL_MULTI_PERFORM = -1, /* please call curl_multi_perform() or
                                    curl_multi_socket*() soon */
  CURLM_OK,
  CURLM_BAD_HANDLE,      /* the passed-in handle is not a valid CURLM handle */
  CURLM_BAD_EASY_HANDLE, /* an easy handle was not good/valid */
  CURLM_OUT_OF_MEMORY,   /* if you ever get this, you are in deep sh*t */
  CURLM_INTERNAL_ERROR,  /* this is a libcurl bug */
  CURLM_BAD_SOCKET,      /* the passed in socket argument did not match */
  CURLM_UNKNOWN_OPTION,  /* curl_multi_setopt() with unsupported option */
  CURLM_ADDED_ALREADY,   /* an easy handle already added to a multi handle was
                            attempted to get added - again */
  CURLM_RECURSIVE_API_CALL, /* an api function was called from inside a
                               callback */
  CURLM_WAKEUP_FAILURE,  /* wakeup is unavailable or failed */
  CURLM_BAD_FUNCTION_ARGUMENT, /* function called with a bad parameter */
  CURLM_ABORTED_BY_CALLBACK,
  CURLM_UNRECOVERABLE_POLL,
  CURLM_LAST
} CURLMcode;

typedef enum {
  CURLMSG_NONE, /* first, not used */
  CURLMSG_DONE, /* This easy handle has completed. 'result' contains
                   the CURLcode of the transfer */
  CURLMSG_LAST /* last, not used */
} CURLMSG;

struct CURLMsg {
  CURLMSG msg;       /* what this message means */
  CURL *easy_handle; /* the handle it concerns */
  union {
    void *whatever;    /* message-specific data */
    CURLcode result;   /* return code for transfer */
  } data;
};
typedef struct CURLMsg CURLMsg;

CURLM *curl_multi_init(void);

CURLMcode curl_multi_add_handle(CURLM *multi_handle, CURL *curl_handle);

CURLMcode curl_multi_remove_handle(CURLM *multi_handle, CURL *curl_handle);

CURLMcode curl_multi_cleanup(CURLM *multi_handle);

CURLMsg *curl_multi_info_read(CURLM *multi_handle, int *msgs_in_queue);

#define CURL_POLL_NONE   0
#define CURL_POLL_IN     1
#define CURL_POLL_OUT    2
#define CURL_POLL_INOUT  3
#define CURL_POLL_REMOVE 4

#define CURL_SOCKET_TIMEOUT CURL_SOCKET_BAD

#define CURL_CSELECT_IN   0x01
#define CURL_CSELECT_OUT  0x02
#define CURL_CSELECT_ERR  0x04

typedef int (*curl_multi_timer_callback)(CURLM *multi,    /* multi handle */
                                         long timeout_ms, /* see above */
                                         void *userp);    /* private callback
                                                             pointer */

CURLMcode curl_multi_socket_action(CURLM *multi_handle,
                                   curl_socket_t s,
                                   int ev_bitmask,
                                   int *running_handles);

typedef enum {
  /* This is the socket callback function pointer */
  CURLOPT(CURLMOPT_SOCKETFUNCTION, CURLOPTTYPE_FUNCTIONPOINT, 1),

  /* This is the argument passed to the socket callback */
  CURLOPT(CURLMOPT_SOCKETDATA, CURLOPTTYPE_OBJECTPOINT, 2),

    /* set to 1 to enable pipelining for this multi handle */
  CURLOPT(CURLMOPT_PIPELINING, CURLOPTTYPE_LONG, 3),

   /* This is the timer callback function pointer */
  CURLOPT(CURLMOPT_TIMERFUNCTION, CURLOPTTYPE_FUNCTIONPOINT, 4),

  /* This is the argument passed to the timer callback */
  CURLOPT(CURLMOPT_TIMERDATA, CURLOPTTYPE_OBJECTPOINT, 5),

  /* maximum number of entries in the connection cache */
  CURLOPT(CURLMOPT_MAXCONNECTS, CURLOPTTYPE_LONG, 6),

  /* maximum number of (pipelining) connections to one host */
  CURLOPT(CURLMOPT_MAX_HOST_CONNECTIONS, CURLOPTTYPE_LONG, 7),

  /* maximum number of requests in a pipeline */
  CURLOPT(CURLMOPT_MAX_PIPELINE_LENGTH, CURLOPTTYPE_LONG, 8),

  /* a connection with a content-length longer than this
     will not be considered for pipelining */
  CURLOPT(CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE, CURLOPTTYPE_OFF_T, 9),

  /* a connection with a chunk length longer than this
     will not be considered for pipelining */
  CURLOPT(CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE, CURLOPTTYPE_OFF_T, 10),

  /* a list of site names(+port) that are blocked from pipelining */
  CURLOPT(CURLMOPT_PIPELINING_SITE_BL, CURLOPTTYPE_OBJECTPOINT, 11),

  /* a list of server types that are blocked from pipelining */
  CURLOPT(CURLMOPT_PIPELINING_SERVER_BL, CURLOPTTYPE_OBJECTPOINT, 12),

  /* maximum number of open connections in total */
  CURLOPT(CURLMOPT_MAX_TOTAL_CONNECTIONS, CURLOPTTYPE_LONG, 13),

   /* This is the server push callback function pointer */
  CURLOPT(CURLMOPT_PUSHFUNCTION, CURLOPTTYPE_FUNCTIONPOINT, 14),

  /* This is the argument passed to the server push callback */
  CURLOPT(CURLMOPT_PUSHDATA, CURLOPTTYPE_OBJECTPOINT, 15),

  /* maximum number of concurrent streams to support on a connection */
  CURLOPT(CURLMOPT_MAX_CONCURRENT_STREAMS, CURLOPTTYPE_LONG, 16),

  CURLMOPT_LASTENTRY /* the last unused */
} CURLMoption;

CURLMcode curl_multi_setopt(CURLM *multi_handle, CURLMoption option, ...);

#ifdef __cplusplus
} /* end of extern "C" */
#endif
