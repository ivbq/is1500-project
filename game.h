
//gameDisplay.c
void display_ball(uint8_t x, uint8_t y);
void display_paddle(uint8_t x, uint8_t y);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

//gameDisplay.c??
char * itoaconv( int num );
void labwork(void);
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];

//??
extern char textbuffer[4][16];

//??
/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
uint8_t getbtns(void);
uint8_t getsw(void);
void enable_interrupt(void);

struct ball {
    float x;
    float y;
    float vx;
    float vy;
};

struct paddle {
    uint8_t y;
    uint8_t score;
};

struct bot {
    uint8_t dir;
    uint8_t diff;
};


