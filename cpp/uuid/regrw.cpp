#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define IMX_OCOPT_BASE_ADDRESS   0x30350000
#define MAP_SIZE  0x800

void printhelp()
{
    printf("Invalid Command \r\n eg: regrw [r]/[w] [ADDR] [VAL]\r\n");
}

int main(int argc, char **argv)
{
    int desc = 0;
    void *vir_addr = NULL;
    unsigned int addr;
    char rw_op;
    unsigned int value=1;
    unsigned int idx=0;
    unsigned  int *p_value;
    unsigned int offset;

    if((argc < 3) || (argc > 4)) {
        printhelp();
        exit(1);
    }
    sscanf(argv[1], "%c", &rw_op);
    sscanf(argv[2], "%x", &addr);

    if(rw_op == 'w') {
        if(argc != 4) {
            printhelp();
            exit(1);
        }    
        sscanf(argv[3], "%x", &value);
    } else if((rw_op == 'r') && 
              (argc > 3)) {
        sscanf(argv[3], "%d", &value);
    }

    if((addr%4) != 0) {
        return 0;
    }

    /* open /dev/mem */
    if ((desc = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("Error: in open device \n");
        exit (-1);
    }
    vir_addr = (unsigned char *)mmap((void *)0x0,MAP_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,desc,IMX_OCOPT_BASE_ADDRESS);

    printf("Virtual address %p mapped to physical address %x \n", vir_addr, IMX_OCOPT_BASE_ADDRESS);

    p_value = (unsigned int*)vir_addr;
    offset  = addr-IMX_OCOPT_BASE_ADDRESS;
    p_value += (offset/4);

    if(rw_op == 'r') { 
        for(idx =0; idx < value; idx++) {
            printf(" %#x  :  0x%08x \r\n", (addr+(4 *idx)) ,*(p_value+idx));
        }
    }else if(rw_op == 'w') { /* Write */
        printf("Write Address %#x  Value 0x%08x \r\n",addr, value);
        *p_value = value;
    }else {
        printf("Invalid command \r\n");
    }

    return 0;
}
