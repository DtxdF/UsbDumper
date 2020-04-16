#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "listin.h"
#include "is.h"
#include "dirtoname.h"
#include "mkdir.h"
#include "concat_name.h"
#include "detect.h"

void usage();

int main(int argc, char *argv[]) {
	int do_help = 0;

	int opt;
	int index;
	int errcod = 0;

	char *shortopts = ":ht:o:";
	char *target = NULL;
	char *outdir = NULL;
	char *concat = NULL;

	struct option options[] = {
		{ "help",   no_argument,       &do_help, 1  },
		{ "outdir", required_argument, NULL,    'o' }

	};

	opterr = 0;
	index = 0;

	while ((opt = getopt_long(argc, argv, shortopts,
							  options, NULL)) != -1) {
		index += 1;

		switch (opt) {
			case 'h':
				do_help = 1;
				break;

			case 'o':
				outdir = optarg;
				break;

			case ':':
				fprintf(stderr, "Error: El parámetro \"-%c/--%s\" requiere un argumento\n",
								opt, options[index].name);

				return EXIT_FAILURE;

			case '?':
				fprintf(stderr, "Error: El parámetro \"-%c\" no se encuentra\n",
								opt);
				
				return ESRCH;

		}

	}

	if ((index == 0) && (opt == -1)) {
		fprintf(stderr, "Error: Usa el parámetro \"-h/--help\" para mostrar la ayuda\n");

		return EXIT_FAILURE;

	}

	if (do_help) {
		usage();

		return EXIT_SUCCESS;

	}

	printf("Esperando dispositivos...\n\n");

	while ((target = detect()) != NULL) {
		printf("Dispositivo objetivo: %s\n\n",
			   target);

		if (isdir(target)) {
			concat = concat_name(outdir, target);

			if (mkpath(concat, (outdir[0] == '/' ? 1 : 0), count_slash(concat)) != 0) {
				errcod = errno;

				perror("mkpath");

				free(concat);

				return errcod;

			}

			listin(target, outdir);

			free(concat);

			printf("\nTerminado: \"%s\"\n\n",
				   target);

		} else {
			fprintf(stderr, "%s no es un directorio válido.\n",
							target);

			return errno;

		}

	}

	printf("Terminado.\n");

	return EXIT_SUCCESS;

}

void usage() {
	fprintf(stderr, "UsbDumper [-h] [-o] OUTDIR\n");

}
