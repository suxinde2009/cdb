/* SPARCs running Solaris 2.5.1 at CS Dept., Princeton University */

#include <string.h>

static char rcsid[] = "$Id$";

#ifndef LCCDIR
#define LCCDIR "/usr/local/lib/lcc/"
#endif
#ifndef SUNDIR
#define SUNDIR "/opt/SUNWspro/SC4.0/lib/"
#endif

static char lccdir[512] = LCCDIR;
char *suffixes[] = { ".c", ".i", ".s", ".o", ".out", 0 };
char inputs[256] = "";
char *cpp[] = { LCCDIR "cpp",
	"-D__STDC__=1", "-Dsparc", "-D__sparc__", "-Dsun", "-D__sun__", "-Dunix",
	"$1", "$2", "$3", 0 };
char *include[] = { "-I" LCCDIR "include", "-I/usr/local/include",
	"-I/usr/include", 0 };
char *com[] = { LCCDIR "rcc", "-target=sparc/solaris", "",
	"$1", "$2", "$3", 0 };
char *as[] = { "/usr/ccs/bin/as", "-Qy", "-s", "-o", "$3", "$1", "$2", 0 };
char *ld[] = { "/usr/ccs/bin/ld", "", "-o", "$3", "$1",
	SUNDIR "crti.o", SUNDIR "crt1.o",
	SUNDIR "values-xa.o", "$2", "",
	"-Y", "P," SUNDIR ":/usr/ccs/lib:/usr/lib", "-Qy",
	"-L" LCCDIR, "-llcc", "", "-lm", "-lc", SUNDIR "crtn.o", "", 0 };

extern char *stringf(const char *, ...);

int option(char *arg) {
	if (strncmp(arg, "-lccdir=", 8) == 0) {
		strcpy(lccdir, &arg[8]);
		if (lccdir[strlen(lccdir)-1] == '/')
			lccdir[strlen(lccdir)-1] = '\0';
		cpp[0] = stringf("%s/cpp", lccdir);
		include[0] = stringf("-I%s/include", lccdir);
		ld[13] = stringf("-L%s", lccdir);
		com[0] = stringf("%s/rcc", lccdir);
	} else if (strcmp(arg, "-g") == 0)
		;
	else if (strcmp(arg, "-p") == 0) {
		ld[5] = SUNDIR "mcrt1.o";
		ld[11] = "P," SUNDIR "libp:/usr/ccs/lib/libp:/usr/lib/libp:"
			 SUNDIR ":/usr/ccs/lib:/usr/lib";
	} else if (strcmp(arg, "-b") == 0)
		;
	else if (strncmp(arg, "-ld=", 4) == 0)
		ld[0] = &arg[4];
	else if (strcmp(arg, "-g4") == 0) {
		extern char *tempdir;
		extern int getpid(void);
		char *tmp = stringf("%s/%d", tempdir, getpid());
		if (lccdir[strlen(lccdir)-1] == '/')
			lccdir[strlen(lccdir)-1] = '\0';
		ld[0] = stringf("sh %s/prelink.sh -o %s.o", lccdir, tmp);
		ld[1] = "$2";
		ld[2] = "\n/usr/ccs/bin/ld -o";
		ld[6] = stringf("%s/startup.o", lccdir);
		ld[9] = stringf("%s.o", tmp);
		ld[15] = "-lnub -lsocket -lnsl";
		ld[19] = stringf("\nrm -f %s.obj %s.c", tmp, tmp);
		com[2] = "-g4";
	} else
		return 0;
	return 1;
}
