#ifdef _MSC_VER
#include "getopt.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef REPLACE_GETOPT
int opterr = 1;   /* if error message should be printed */
int optind = 1;   /* index into parent argv vector */
int optopt = '?'; /* character checked for validity */
#undef optreset   /* see getopt.h */
#define optreset __mingw_optreset
int optreset; /* reset getopt */
char* optarg; /* argument associated with option */
#endif

static void _vwarnx(const char* fmt, va_list ap)
{
  (void)fprintf(stderr, "%s: ", __progname);
  if (fmt != NULL) (void)vfprintf(stderr, fmt, ap);
  (void)fprintf(stderr, "\n");
}

static void warnx(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  _vwarnx(fmt, ap);
  va_end(ap);
}

/*
 * Compute the greatest common divisor of a and b.
 */
static int gcd(int a, int b)
{
  int c;

  c = a % b;
  while (c != 0) {
    a = b;
    b = c;
    c = a % b;
  }

  return (b);
}

/*
 * Exchange the block from nonopt_start to nonopt_end with the block
 * from nonopt_end to opt_end (keeping the same order of arguments
 * in each block).
 */
static void permute_args(int panonopt_start, int panonopt_end, int opt_end,
                         char* const* nargv)
{
  int cstart, cyclelen, i, j, ncycle, nnonopts, nopts, pos;
  char* swap;

  /*
   * compute lengths of blocks and number and size of cycles
   */
  nnonopts = panonopt_end - panonopt_start;
  nopts = opt_end - panonopt_end;
  ncycle = gcd(nnonopts, nopts);
  cyclelen = (opt_end - panonopt_start) / ncycle;

  for (i = 0; i < ncycle; i++) {
    cstart = panonopt_end + i;
    pos = cstart;
    for (j = 0; j < cyclelen; j++) {
      if (pos >= panonopt_end) pos -= nnonopts;
      else
        pos += nopts;
      swap = nargv[pos];
      /* LINTED const cast */
      ((char**)nargv)[pos] = nargv[cstart];
      /* LINTED const cast */
      ((char**)nargv)[cstart] = swap;
    }
  }
}

#ifdef REPLACE_GETOPT
/*
 * getopt --
 *    Parse argc/argv argument vector.
 *
 * [eventually this will replace the BSD getopt]
 */
int getopt(int nargc, char* const* nargv, const char* options)
{
  /*
   * We don't pass FLAG_PERMUTE to getopt_internal() since
   * the BSD getopt(3) (unlike GNU) has never done this.
   *
   * Furthermore, since many privileged programs call getopt()
   * before dropping privileges it makes sense to keep things
   * as simple (and bug-free) as possible.
   */
  return (getopt_internal(nargc, nargv, options, NULL, NULL, 0));
}
#endif /* REPLACE_GETOPT */

// extern int getopt(int nargc, char * const *nargv, const char *options);

#ifdef __cplusplus
}
#endif
/*
 * POSIX requires the `getopt' API to be specified in `unistd.h';
 * thus, `unistd.h' includes this header.  However, we do not want
 * to expose the `getopt_long' or `getopt_long_only' APIs, when
 * included in this manner.  Thus, close the standard __GETOPT_H__
 * declarations block, and open an additional __GETOPT_LONG_H__
 * specific block, only when *not* __UNISTD_H_SOURCED__, in which
 * to declare the extended API.
 */

#ifdef __cplusplus
extern "C" {
#endif

struct option /* specification for a long form option...    */
{
  const char* name; /* option name, without leading hyphens */
  int has_arg;      /* does it take an argument?        */
  int* flag;        /* where to save its status, or NULL    */
  int val;          /* its associated status value        */
};

enum /* permitted values for its `has_arg' field...    */
{
  no_argument = 0,   /* option never takes an argument    */
  required_argument, /* option always requires an argument    */
  optional_argument  /* option may take an argument        */
};

/*
 * parse_long_options --
 *    Parse long options in argc/argv argument vector.
 * Returns -1 if short_too is set and the option does not match long_options.
 */
static int parse_long_options(char* const* nargv, const char* options,
                              const struct option* long_options, int* idx,
                              int short_too)
{
  char *current_argv, *has_equal;
  size_t current_argv_len;
  int i, ambiguous, match;

#define IDENTICAL_INTERPRETATION(_x, _y)                       \
  (long_options[(_x)].has_arg == long_options[(_y)].has_arg && \
   long_options[(_x)].flag == long_options[(_y)].flag &&       \
   long_options[(_x)].val == long_options[(_y)].val)

  current_argv = place;
  match = -1;
  ambiguous = 0;

  optind++;

  if ((has_equal = strchr(current_argv, '=')) != NULL) {
    /* argument found (--option=arg) */
    current_argv_len = has_equal - current_argv;
    has_equal++;
  } else
    current_argv_len = strlen(current_argv);

  for (i = 0; long_options[i].name; i++) {
    /* find matching long option */
    if (strncmp(current_argv, long_options[i].name, current_argv_len)) continue;

    if (strlen(long_options[i].name) == current_argv_len) {
      /* exact match */
      match = i;
      ambiguous = 0;
      break;
    }
    /*
     * If this is a known short option, don't allow
     * a partial match of a single character.
     */
    if (short_too && current_argv_len == 1) continue;

    if (match == -1) /* partial match */
      match = i;
    else if (!IDENTICAL_INTERPRETATION(i, match))
      ambiguous = 1;
  }
  if (ambiguous) {
    /* ambiguous abbreviation */
    if (PRINT_ERROR) warnx(ambig, (int)current_argv_len, current_argv);
    optopt = 0;
    return (BADCH);
  }
  if (match != -1) { /* option found */
    if (long_options[match].has_arg == no_argument && has_equal) {
      if (PRINT_ERROR) warnx(noarg, (int)current_argv_len, current_argv);
      /*
       * XXX: GNU sets optopt to val regardless of flag
       */
      if (long_options[match].flag == NULL) optopt = long_options[match].val;
      else
        optopt = 0;
      return (BADARG);
    }
    if (long_options[match].has_arg == required_argument ||
        long_options[match].has_arg == optional_argument) {
      if (has_equal) optarg = has_equal;
      else if (long_options[match].has_arg == required_argument) {
        /*
         * optional argument doesn't use next nargv
         */
        optarg = nargv[optind++];
      }
    }
    if ((long_options[match].has_arg == required_argument) &&
        (optarg == NULL)) {
      /*
       * Missing argument; leading ':' indicates no error
       * should be generated.
       */
      if (PRINT_ERROR) warnx(recargstring, current_argv);
      /*
       * XXX: GNU sets optopt to val regardless of flag
       */
      if (long_options[match].flag == NULL) optopt = long_options[match].val;
      else
        optopt = 0;
      --optind;
      return (BADARG);
    }
  } else { /* unknown option */
    if (short_too) {
      --optind;
      return (-1);
    }
    if (PRINT_ERROR) warnx(illoptstring, current_argv);
    optopt = 0;
    return (BADCH);
  }
  if (idx) *idx = match;
  if (long_options[match].flag) {
    *long_options[match].flag = long_options[match].val;
    return (0);
  } else
    return (long_options[match].val);
#undef IDENTICAL_INTERPRETATION
}

/*
 * getopt_internal --
 *    Parse argc/argv argument vector.  Called by user level routines.
 */
static int getopt_internal(int nargc, char* const* nargv, const char* options,
                           const struct option* long_options, int* idx,
                           int flags)
{
  char* oli; /* option letter list index */
  int optchar, short_too;
  static int posixly_correct = -1;

  if (options == NULL) return (-1);

  /*
   * XXX Some GNU programs (like cvs) set optind to 0 instead of
   * XXX using optreset.  Work around this braindamage.
   */
  if (optind == 0) optind = optreset = 1;

  /*
   * Disable GNU extensions if POSIXLY_CORRECT is set or options
   * string begins with a '+'.
   *
   * CV, 2009-12-14: Check POSIXLY_CORRECT anew if optind == 0 or
   *                 optreset != 0 for GNU compatibility.
   */
  if (posixly_correct == -1 || optreset != 0)
    posixly_correct = (getenv("POSIXLY_CORRECT") != NULL);
  if (*options == '-') flags |= FLAG_ALLARGS;
  else if (posixly_correct || *options == '+')
    flags &= ~FLAG_PERMUTE;
  if (*options == '+' || *options == '-') options++;

  optarg = NULL;
  if (optreset) nonopt_start = nonopt_end = -1;
start:
  if (optreset || !*place) { /* update scanning pointer */
    optreset = 0;
    if (optind >= nargc) { /* end of argument vector */
      place = EMSG;
      if (nonopt_end != -1) {
        /* do permutation, if we have to */
        permute_args(nonopt_start, nonopt_end, optind, nargv);
        optind -= nonopt_end - nonopt_start;
      } else if (nonopt_start != -1) {
        /*
         * If we skipped non-options, set optind
         * to the first of them.
         */
        optind = nonopt_start;
      }
      nonopt_start = nonopt_end = -1;
      return (-1);
    }
    if (*(place = nargv[optind]) != '-' ||
        (place[1] == '\0' && strchr(options, '-') == NULL)) {
      place = EMSG; /* found non-option */
      if (flags & FLAG_ALLARGS) {
        /*
         * GNU extension:
         * return non-option as argument to option 1
         */
        optarg = nargv[optind++];
        return (INORDER);
      }
      if (!(flags & FLAG_PERMUTE)) {
        /*
         * If no permutation wanted, stop parsing
         * at first non-option.
         */
        return (-1);
      }
      /* do permutation */
      if (nonopt_start == -1) nonopt_start = optind;
      else if (nonopt_end != -1) {
        permute_args(nonopt_start, nonopt_end, optind, nargv);
        nonopt_start = optind - (nonopt_end - nonopt_start);
        nonopt_end = -1;
      }
      optind++;
      /* process next argument */
      goto start;
    }
    if (nonopt_start != -1 && nonopt_end == -1) nonopt_end = optind;

    /*
     * If we have "-" do nothing, if "--" we are done.
     */
    if (place[1] != '\0' && *++place == '-' && place[1] == '\0') {
      optind++;
      place = EMSG;
      /*
       * We found an option (--), so if we skipped
       * non-options, we have to permute.
       */
      if (nonopt_end != -1) {
        permute_args(nonopt_start, nonopt_end, optind, nargv);
        optind -= nonopt_end - nonopt_start;
      }
      nonopt_start = nonopt_end = -1;
      return (-1);
    }
  }

  /*
   * Check long options if:
   *  1) we were passed some
   *  2) the arg is not just "-"
   *  3) either the arg starts with -- we are getopt_long_only()
   */
  if (long_options != NULL && place != nargv[optind] &&
      (*place == '-' || (flags & FLAG_LONGONLY))) {
    short_too = 0;
    if (*place == '-') place++; /* --foo long option */
    else if (*place != ':' && strchr(options, *place) != NULL)
      short_too = 1; /* could be short option too */

    optchar = parse_long_options(nargv, options, long_options, idx, short_too);
    if (optchar != -1) {
      place = EMSG;
      return (optchar);
    }
  }

  if ((optchar = (int)*place++) == (int)':' ||
      (optchar == (int)'-' && *place != '\0') ||
      (oli = (char*)strchr(options, optchar)) == NULL) {
    /*
     * If the user specified "-" and  '-' isn't listed in
     * options, return -1 (non-option) as per POSIX.
     * Otherwise, it is an unknown option character (or ':').
     */
    if (optchar == (int)'-' && *place == '\0') return (-1);
    if (!*place) ++optind;
    if (PRINT_ERROR) warnx(illoptchar, optchar);
    optopt = optchar;
    return (BADCH);
  }
  if (long_options != NULL && optchar == 'W' && oli[1] == ';') {
    /* -W long-option */
    if (*place) /* no space */
      /* NOTHING */;
    else if (++optind >= nargc) { /* no arg */
      place = EMSG;
      if (PRINT_ERROR) warnx(recargchar, optchar);
      optopt = optchar;
      return (BADARG);
    } else /* white space */
      place = nargv[optind];
    optchar = parse_long_options(nargv, options, long_options, idx, 0);
    place = EMSG;
    return (optchar);
  }
  if (*++oli != ':') { /* doesn't take argument */
    if (!*place) ++optind;
  } else { /* takes (optional) argument */
    optarg = NULL;
    if (*place) /* no white space */
      optarg = place;
    else if (oli[1] != ':') {  /* arg not optional */
      if (++optind >= nargc) { /* no arg */
        place = EMSG;
        if (PRINT_ERROR) warnx(recargchar, optchar);
        optopt = optchar;
        return (BADARG);
      } else
        optarg = nargv[optind];
    }
    place = EMSG;
    ++optind;
  }
  /* dump back option letter */
  return (optchar);
}

/*
 * getopt_long --
 *    Parse argc/argv argument vector.
 */
int getopt_long(int nargc, char* const* nargv, const char* options,
                const struct option* long_options, int* idx)
{
  return (
      getopt_internal(nargc, nargv, options, long_options, idx, FLAG_PERMUTE));
}

/*
 * getopt_long_only --
 *    Parse argc/argv argument vector.
 */
int getopt_long_only(int nargc, char* const* nargv, const char* options,
                     const struct option* long_options, int* idx)
{
  return (getopt_internal(nargc, nargv, options, long_options, idx,
                          FLAG_PERMUTE | FLAG_LONGONLY));
}

// extern int getopt_long(int nargc, char * const *nargv, const char *options,
//     const struct option *long_options, int *idx);
// extern int getopt_long_only(int nargc, char * const *nargv, const char
// *options,
//     const struct option *long_options, int *idx);
/*
 * Previous MinGW implementation had...
 */

#ifdef __cplusplus
}
#endif
#endif
