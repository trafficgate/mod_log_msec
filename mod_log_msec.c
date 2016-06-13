#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

static int begin_msec_count(request_rec *r)
{
    struct timeval *stime;
    int rc;

    if (!ap_is_initial_req(r))
        return DECLINED;

    stime = ap_pcalloc(r->pool, sizeof(struct timeval));
    rc = gettimeofday(stime, NULL);
    if (rc == -1) {
        return DECLINED;
    }

    ap_table_setn(r->notes, "REQUEST_MICRO_SECONDS", (const char *)stime);

    return OK;
}

static int end_msec_count(request_rec *r)
{
    struct timeval *stime, etime;
    int rc;
    const char *msec;
    double start, end;

    stime = (struct timeval *)ap_table_get(r->notes, "REQUEST_MICRO_SECONDS");
    if (stime == NULL) {
        return DECLINED;
    }
    rc = gettimeofday(&etime, NULL);
    if (rc == -1) {
        return DECLINED;
    }

    start = stime->tv_sec + stime->tv_usec * 1e-6;
    end   = etime.tv_sec  + etime.tv_usec * 1e-6;

    msec = ap_psprintf(r->pool, "%.6f", end - start);
    while (r->next)
        r = r->next;
    ap_table_setn(r->subprocess_env, "D", msec);
    return OK;
}


/* Dispatch list for API hooks */
module MODULE_VAR_EXPORT log_msec_module = {
    STANDARD_MODULE_STUFF, 
    NULL,                  /* module initializer                  */
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    NULL,                  /* table of config file commands       */
    NULL,                  /* [#8] MIME-typed-dispatched handlers */
    NULL,                  /* [#1] URI to filename translation    */
    NULL,                  /* [#4] validate user id from request  */
    NULL,                  /* [#5] check if the user is ok _here_ */
    NULL,                  /* [#3] check access by host address   */
    NULL,                  /* [#6] determine MIME type            */
    NULL,                  /* [#7] pre-run fixups                 */
    end_msec_count,        /* [#9] log a transaction              */
    NULL,                  /* [#2] header parser                  */
    NULL,                  /* child_init                          */
    NULL,                  /* child_exit                          */
    begin_msec_count       /* [#0] post read-request              */
};

