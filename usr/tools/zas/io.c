#include <zas/zash.h>

#if (ZASMMAP)
#define zasgetc(map) ((map)->cur < (map)->lim ? *(map)->cur++ : EOF)
#elif (ZASBUF)
static int
zasgetc(int fd, int bufid)
{
    struct readbuf *buf = &readbuftab[bufid];
    ssize_t         nleft = ZASBUFSIZE;
    ssize_t         n;
    int             ch = EOF;
    long            l = nreadbuf;

    if (bufid >= nreadbuf) {
        nreadbuf <<= 1;
        readbuftab = realloc(readbuftab, nreadbuf * sizeof(struct readbuf));
        for ( ; l < nreadbuf ; l++) {
            readbuftab[l].data = malloc(ZASBUFSIZE);
        }
    }
    if (buf->cur < buf->lim) {
        ch = *buf->cur++;
    } else if (buf->cur == buf->lim) {
        n = 0;
        while (nleft) {
            n = read(fd, buf->data, ZASBUFSIZE);
            if (n < 0) {
                if (errno == EINTR) {
                    
                    continue;
                } else {
                    
                    return EOF;
                }
            } else if (n == 0) {

                break;
            } else {
                nleft -= n;
            }
        }
        if (nleft == ZASBUFSIZE) {

            return EOF;
        }
        buf->cur = buf->data;
        buf->lim = (uint8_t *)buf->data + ZASBUFSIZE - nleft;
        ch = *buf->cur++;
    }
    
    return ch;
}
#endif

