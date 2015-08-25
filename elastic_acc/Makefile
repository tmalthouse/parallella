CC = /usr/bin/gcc
CFLAGS = -Ofast
ESDK = ${EPIPHANY_HOME}
ELIBS = ${ESDK}/tools/host/lib
EINCS = ${ESDK}/tools/host/include
ELDF = ${ESDK}/bsps/current/internal.ldf

all:
	${CC} src/acc.c -o acc.elf -I ${EINCS} -L ${ELIBS} -le-hal -le-loader ${CFLAGS}

	e-gcc -O2 -T ${ELDF} src/isprime.c src/e_prime.c -o e_prime.elf -le-lib -lm
