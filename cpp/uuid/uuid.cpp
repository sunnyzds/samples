#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define IMX_OCOPT_BASE_ADDRESS   0x30350000
#define MAP_SIZE  0x1000  // MB

#define IMX_OCOTP_ADDR_CTRL		0x0000
#define IMX_OCOTP_ADDR_CTRL_SET		0x0004
#define IMX_OCOTP_ADDR_CTRL_CLR		0x0008
#define IMX_OCOTP_ADDR_TIMING		0x0010
#define IMX_OCOTP_ADDR_DATA		0x0020

#define IMX_OCOTP_BM_CTRL_ADDR		0x0000007F
#define IMX_OCOTP_BM_CTRL_BUSY		0x00000100
#define IMX_OCOTP_BM_CTRL_ERROR		0x00000200
#define IMX_OCOTP_BM_CTRL_REL_SHADOWS	0x00000400

typedef struct {
    unsigned char p0[4];
    unsigned char p1[4];
} otpuuid;

typedef struct {
    unsigned int data[128];
} encdata;

enum {
    GET_UUID_ERROR = 0x1,    
};

int encrypt_uuid(otpuuid* uuid, encdata* data)
{

}

int decrypt_uuid(otpuuid* uuid, encdata* data)
{

}

int get_uuid(otpuuid* uuid)
{
    int ret = 0;
    int fd = 0;
    void *vir_addr = 0;
    unsigned int tmp, mask;
    int count;
    fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (fd < 0) {
        ret = -(GET_UUID_ERROR);
        goto error;
    }
    printf("start mmap\n");
    vir_addr = (unsigned char *)mmap((void *)0x0, MAP_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, fd, IMX_OCOPT_BASE_ADDRESS);
    if (vir_addr == 0) {
        printf("mmap failed\n");
        ret = -(GET_UUID_ERROR+1);
        goto error;
    }
    mask = IMX_OCOTP_BM_CTRL_BUSY | IMX_OCOTP_BM_CTRL_ERROR;
    for (count = 10000; count >= 0; count--) {
        printf("read ctrl\n");
        tmp = *((unsigned int*)vir_addr);
        printf("ctrl:0x%08x\n", tmp);
        if (!(tmp & mask))
            break;
    }
    
    if (count < 0) {    
        if (tmp & IMX_OCOTP_BM_CTRL_ERROR) {
            ret = -(GET_UUID_ERROR+2);
            goto error;
        }
        ret = -(GET_UUID_ERROR+3);
        goto error;
    }

    printf("read uuid\n");
    tmp = *((unsigned int*)((unsigned char*)vir_addr + 0x410));
    uuid->p0[0] = (unsigned char)(tmp & 0xff);
    uuid->p0[1] = (unsigned char)((tmp >> 8) & 0xff);
    uuid->p0[2] = (unsigned char)((tmp >> 16) & 0xff);
    uuid->p0[3] = (unsigned char)((tmp >> 24) & 0xff);

    tmp = *((unsigned int*)((unsigned char*)vir_addr + 0x420));
    uuid->p1[0] = (unsigned char)(tmp & 0xff);
    uuid->p1[1] = (unsigned char)((tmp >> 8) & 0xff);
    uuid->p1[2] = (unsigned char)((tmp >> 16) & 0xff);
    uuid->p1[3] = (unsigned char)((tmp >> 24) & 0xff);

    return 0;
error:
    printf("ret:%d\n", ret);
    return ret;
}

void dump_uuid(otpuuid* uuid)
{
    printf("0x%02x 0x%02x 0x%02x 0x%02x \n", uuid->p0[0], uuid->p0[1], uuid->p0[2], uuid->p0[3]);
    printf("0x%02x 0x%02x 0x%02x 0x%02x \n", uuid->p1[0], uuid->p1[1], uuid->p1[2], uuid->p1[3]);
}

int main(int argc, char** argv)
{
    int ret = 0;
    otpuuid uuid;
    ret = get_uuid(&uuid);
    dump_uuid(&uuid);
    return ret;
}

