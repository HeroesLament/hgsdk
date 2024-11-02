
#include "typesdef.h"
#include "osal/mutex.h"
#include "osal/string.h"

struct os_mutex _print_m_;
int    _print_m_init_ = 0;

void print_lock(void)
{
    if (!_print_m_init_) {
        os_mutex_init(&_print_m_);
        _print_m_init_ = 1;
    }
    os_mutex_lock(&_print_m_, osWaitForever);
}

void print_unlock(void)
{
    os_mutex_unlock(&_print_m_);
}

int32 hex2int(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

int32 hex2char(char *hex)
{
    int a, b;
    a = hex2int(*hex++);
    if (a < 0) {
        return -1;
    }
    b = hex2int(*hex++);
    if (b < 0) {
        return -1;
    }
    return (a << 4) | b;
}

int32 hex2bin(char *hex_str, uint8 *bin, uint32 len)
{
    uint32 i = 0;
    uint32 str_len = 0;
    int32  a;
    char *ipos = hex_str;
    uint8 *opos = bin;

    if (!hex_str || !bin) {
        return i;
    }

    str_len = os_strlen(hex_str);
    for (i = 0; i < len && (i*2)+2 <= str_len; i++) {
        a = hex2char(ipos);
        if (a < 0) {
            return i;
        }
        *opos++ = (uint8)a;
        ipos += 2;
    }
    return i;
}

void str2mac(char *macstr, uint8 *mac)
{
    mac[0] = hex2char(macstr);
    mac[1] = hex2char(macstr + 3);
    mac[2] = hex2char(macstr + 6);
    mac[3] = hex2char(macstr + 9);
    mac[4] = hex2char(macstr + 12);
    mac[5] = hex2char(macstr + 15);
}

uint32 os_atoh(char *str)
{
    uint32 val = 0;
    uint8  s = 0;

    if (os_strlen(str) > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    while (*str) {
        if ((*str >= '0') && (*str <= '9')) {
            s = *str - '0';
        } else if ((*str >= 'A') && (*str <= 'F')) {
            s = 0x0A + (*str - 'A');
        } else if ((*str >= 'a') && (*str <= 'f')) {
            s = 0x0A + (*str - 'a');
        } else {
            break;
        }
        val = (val << 4) + s;
        str++;
    }
    return val;
}



void (*__print_redirect)(char *msg);
void osprint_redirect(void (*print)(char *msg))
{
    __print_redirect = print;
}
void hgprintf(const char *fmt, ...)
{
#define HGPRINTF_LEN (1024)
    static char _print_buff_p[HGPRINTF_LEN];
    void (*_print)(char *msg) = __print_redirect;
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vsnprintf(_print_buff_p, HGPRINTF_LEN - 1, fmt, ap);
    va_end(ap);
    if (ret > 0) {
        _print_buff_p[ret] = 0;
        if (_print) {
            _print(_print_buff_p);
        } else {
            printf("%s", _print_buff_p);
        }
    }
}


void dump_hex(char *str, uint8 *data, uint32 len)
{
    int i = 0;
    if (data && len) {
        if (str) { _os_printf("%s", str); }
        for (i = 0; i < len; i++) {
            if (i > 0) {
                if ((i & 0x7) == 0) { _os_printf("   "); }
                if ((i & 0xf) == 0) { _os_printf("\r\n"); }
            }
            _os_printf("%02x ", data[i] & 0xFF);
        }
        _os_printf("\r\n");
    }
}

void dump_key(uint8 *str, uint8 *key, uint32 len)
{
    int32 i = 0;
    if (key && len) {
        if (str) { _os_printf("%s:", str); }
        for (i = 0; i < len; i++) { _os_printf("%02x", key[i]); }
        _os_printf("\r\n");
    }
}

