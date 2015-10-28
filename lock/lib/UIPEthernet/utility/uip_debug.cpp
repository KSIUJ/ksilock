#if defined(UIPDEBUG)
    #include <mbed.h>
    #include <inttypes.h>
    #include <uip_debug.h>
extern "C"
{
    #include "uip.h"
} extern Serial     pc;
struct uip_conn con[UIP_CONNS];

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPDebug::uip_debug_printconns(void) {
    for(uint8_t i = 0; i < UIP_CONNS; i++) {
        if(uip_debug_printcon(&con[i], &uip_conns[i])) {
            pc.printf("connection[");
            pc.printf("%d", i);
            pc.printf("] changed.\n");
        }
    }
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
bool UIPDebug::uip_debug_printcon(struct uip_conn* lhs, struct uip_conn* rhs) {
    bool    changed = false;
    if(!uip_ipaddr_cmp(lhs->ripaddr, rhs->ripaddr)) {
        pc.printf(" ripaddr: ");
        uip_debug_printbytes((const uint8_t*)lhs->ripaddr, 4);
        pc.printf(" -> ");
        uip_debug_printbytes((const uint8_t*)rhs->ripaddr, 4);
        pc.printf("\n");
        uip_ipaddr_copy(lhs->ripaddr, rhs->ripaddr);
        changed = true;
    }

    if(lhs->lport != rhs->lport) {
        pc.printf(" lport: ");
        pc.printf("%d", htons(lhs->lport));
        pc.printf(" -> ");
        pc.printf("%d\n", htons(rhs->lport));
        lhs->lport = rhs->lport;
        changed = true;
    }

    if(lhs->rport != rhs->rport) {
        pc.printf(" rport: ");
        pc.printf("%d", htons(lhs->rport));
        pc.printf(" -> ");
        pc.printf("%d\n", htons(rhs->rport));
        lhs->rport = rhs->rport;
        changed = true;
    }

    if((uint32_t) lhs->rcv_nxt[0] != (uint32_t) rhs->rcv_nxt[0]) {
        pc.printf(" rcv_nxt: ");
        uip_debug_printbytes(lhs->rcv_nxt, 4);
        pc.printf(" -> ");
        uip_debug_printbytes(rhs->rcv_nxt, 4);
        *((uint32_t*) &lhs->rcv_nxt[0]) = (uint32_t) rhs->rcv_nxt[0];
        pc.printf("\n");
        changed = true;
    }

    if((uint32_t) lhs->snd_nxt[0] != (uint32_t) rhs->snd_nxt[0]) {
        pc.printf(" snd_nxt: ");
        uip_debug_printbytes(lhs->snd_nxt, 4);
        pc.printf(" -> ");
        uip_debug_printbytes(rhs->snd_nxt, 4);
        *((uint32_t*) &lhs->snd_nxt[0]) = (uint32_t) rhs->snd_nxt[0];
        pc.printf("\n");
        changed = true;
    }

    if(lhs->len != rhs->len) {
        pc.printf(" len: ");
        pc.printf("%d", lhs->len);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->len);
        lhs->len = rhs->len;
        changed = true;
    }

    if(lhs->mss != rhs->mss) {
        pc.printf(" mss: ");
        pc.printf("%d", lhs->mss);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->mss);
        lhs->mss = rhs->mss;
        changed = true;
    }

    if(lhs->initialmss != rhs->initialmss) {
        pc.printf(" initialmss: ");
        pc.printf("%d", lhs->initialmss);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->initialmss);
        lhs->initialmss = rhs->initialmss;
        changed = true;
    }

    if(lhs->sa != rhs->sa) {
        pc.printf(" sa: ");
        pc.printf("%d", lhs->sa);
        pc.printf(" -> ");
        pc.printf("%d", rhs->sa);
        lhs->sa = rhs->sa;
        changed = true;
    }

    if(lhs->sv != rhs->sv) {
        pc.printf(" sv: ");
        pc.printf("%d", lhs->sv);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->sv);
        lhs->sv = rhs->sv;
        changed = true;
    }

    if(lhs->rto != rhs->rto) {
        pc.printf(" rto: ");
        pc.printf("%d", lhs->rto);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->rto);
        lhs->rto = rhs->rto;
        changed = true;
    }

    if(lhs->tcpstateflags != rhs->tcpstateflags) {
        pc.printf(" tcpstateflags: ");
        pc.printf("%d", lhs->tcpstateflags);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->tcpstateflags);
        lhs->tcpstateflags = rhs->tcpstateflags;
        changed = true;
    }

    if(lhs->timer != rhs->timer) {
        pc.printf(" timer: ");
        pc.printf("%d", lhs->timer);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->timer);
        lhs->timer = rhs->timer;
        changed = true;
    }

    if(lhs->nrtx != rhs->nrtx) {
        pc.printf(" nrtx: ");
        pc.printf("%d", lhs->nrtx);
        pc.printf(" -> ");
        pc.printf("%d\n", rhs->nrtx);
        lhs->nrtx = rhs->nrtx;
        changed = true;
    }

    return changed;
}

/**
 * @brief
 * @note
 * @param
 * @retval
 */
void UIPDebug::uip_debug_printbytes(const uint8_t* data, uint8_t len) {
    for(uint8_t i = 0; i < len; i++) {
        pc.printf("%d", data[i]);
        if(i < len - 1)
            pc.printf(",");
    }
}
#endif
