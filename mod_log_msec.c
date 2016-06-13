//
// log に 1970 01/01 からの msec を出すモジュール
// apr_time_now() は GMT を返すので、1970/01/01 00:00:00 からの msec を返す
// msec までの精度で時刻が必要な方にどうぞ
//
//
// How to Use.
// 1. Add "%M" in LogFormat line on httpd.conf.
// 2. Add "LoadModule msec_module modules/mod_log_msec.so" on httpd.conf.
//
// %...M:   msec from 1970 01/01 00:00:00.
//
//
#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

#include "apr_strings.h"
#include "mod_log_config.h"


//
// Convert apr_time_t to string.
//
// This code uses the same technique as apr_itoa();
// See the function in srclib/apr/strings/apr_strings.c for more infomation.
//
static char *timetoa(apr_pool_t *p, apr_time_t n)
{
    const int BUFFER_SIZE = sizeof(apr_time_t) * 3 + 2;
    char *buf = apr_palloc(p, BUFFER_SIZE);

    char *start = buf + BUFFER_SIZE - 1;

    *start = 0;

    do {
        *--start = '0' + (n % 10);
        n /= 10;
    } while (n);

    return start;
}

static const char *log_msec(request_rec *r, char *a)
{
#ifdef I_INSIST_ON_EXTRA_CYCLES_FOR_CLF_COMPLIANCE
    apr_time_t request_time = apr_time_now();
#else
    apr_time_t request_time = r->request_time;
#endif
    return timetoa(r->pool, apr_time_as_msec( request_time));
}


static int msec_pre_config( apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp)
{
    static APR_OPTIONAL_FN_TYPE(ap_register_log_handler) *log_register;

    log_register = APR_RETRIEVE_OPTIONAL_FN(ap_register_log_handler);
    if (!log_register)
        return DECLINED;

    log_register(p, "M", log_msec, 0);

    return OK;
}

static void msec_register_hooks(apr_pool_t *p)
{
    ap_hook_pre_config(msec_pre_config, NULL, NULL, APR_HOOK_LAST);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA log_msec_module = {
    STANDARD20_MODULE_STUFF, 
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    msec_register_hooks  /* register hooks                      */
};

