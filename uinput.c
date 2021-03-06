#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

#define die(str, args...) do { \
        perror(str); \
        exit(EXIT_FAILURE); \
} while(0)

int main(void)
{
    int                    fd;
    struct uinput_user_dev uidev;
    struct input_event     ev;
    int                    dx, dy;
    int                    i;

     fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
if(fd < 0) die("error: open");

//config uinput working mode,  mouse or touchscreen?  relative coordinates or absolute coordinate?
    if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)         //support key button
        die("error: ioctl");
    if(ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) < 0)  //support mouse left key
        die("error: ioctl");

    if(ioctl(fd, UI_SET_EVBIT, EV_REL) < 0)       //uinput use relative coordinates
        die("error: ioctl");
    if(ioctl(fd, UI_SET_RELBIT, REL_X) < 0)         //uinput use x coordinates
        die("error: ioctl");
    if(ioctl(fd, UI_SET_RELBIT, REL_Y) < 0)         //uinput use y coordinates
        die("error: ioctl");

    memset(&uidev, 0, sizeof(uidev));                  //creat an virtul input device node in /dev/input/***
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    if(write(fd, &uidev, sizeof(uidev)) < 0)
        die("error: write");

    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: ioctl");

    sleep(2);
    srand(time(NULL));

while(1) {
//simulate( x,y) coordinates for mouse
        switch(rand() % 4) {
        case 0:
            dx = -10;
            dy = -1;
            break;
        case 1:
            dx = 10;
            dy = 1;
            break;
        case 2:
            dx = -1;
            dy = 10;
            break;
        case 3:
            dx = 1;
            dy = -10;
            break;
        }

        for(i = 0; i < 20; i++) {
//send input event to kernel input system
            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_REL;         //send x coordinates
            ev.code = REL_X;
            ev.value = dx;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_REL;  //send y coordinates
            ev.code = REL_Y;
            ev.value = dy;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_KEY;  //mouse left key
            ev.code = BTN_LEFT;
            ev.value = 1;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            memset(&ev, 0, sizeof(struct input_event));
            ev.type = EV_SYN; // inform input system to process this input event
            ev.code = 0;
            ev.value = 0;
            if(write(fd, &ev, sizeof(struct input_event)) < 0)
                die("error: write");

            usleep(15000);
        }

        sleep(5);
    }

    sleep(2);

    if(ioctl(fd, UI_DEV_DESTROY) < 0)
        die("error: ioctl");
    close(fd);
    return 0;
}

