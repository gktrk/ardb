
/*
 * I don't know of any WinCE platform that isn't 32 bits.
 * Redefine this if that is the case, and a patch to the sqlite-wince
 * project on SourceFourge would be great.
 */
#define SQLITE_PTR_SZ 4


/*
 * Define this to omit authorization code
 */

// #define SQLITE_OMIT_AUTHORIZATION 1

/********
 ** TODO: Add more SQLITE_OMIT_* things here (as comments)
 ********/

/*
 * Define this to omit file locking code.
 *
 * Only use this if you are sure you will never access a database
 * simultaneously from two processes.
 * For a database that is only used by a single instance of your
 * application (beware for multiple instances!), it is safe (and
 * faster) to omit this feature.
 * This is forced on for Windows CE versions prior than 3.0 (see below)
 */

// #define SQLITE_WCE_OMIT_FILELOCK  1



/**********************************************************************
 *
 * WinCE fixes...
 *
 *********************************************************************/

#include <windows.h>

/*
 * Eliminate some warnings in the SQLite code.
 * (This should be addressed in the SQLite source - TODO)
 */
//C4018: '_' : signed/unsigned mismatch
//C4244: '_' : conversion from '_' to '_', possible loss of data
//C4267: '_' : conversion from '_' to '_', possible loss of data (MSVC 7.1 ?)
//C4761: integral size mismatch in argument; conversion supplied
#pragma warning ( disable : 4018 4244 4267 4761 )

/*
 * This is defined on <dbgapi.h> and redefined on 'vdbe.c', so undefine it
 */
#undef VERIFY

/*
 * WinCE platform configuration
 *
 * On versions prior than 3.0, a lot of workarounds are needed.
 * For start, no isalpha(),isdigit(),etc. functions/macros are implemented
 * (only the Unicode versions, like iswalpha). The same happens to atof().
 * Another quirk is that the free() function has a diferent declaration
 * from the "normal" one, so it can't be used in pointer to function in
 * the same way (like it is used in 'tokenize.c').
 * The last workaround is for the getenv() function, as there is no
 * environment for WinCE.
 */

#if defined(_WIN32_WCE) && _WIN32_WCE < 300
#  define SQLITE_WCE_USE_OWN_ISXXXX    1
#  define SQLITE_WCE_USE_OWN_ATOF      1
#  define SQLITE_WCE_FREE_HACK         1
#  define SQLITE_WCE_STRCMP_HACK       1
#else
#  define SQLITE_WCE_USE_OWN_ISXXXX    0
#  define SQLITE_WCE_USE_OWN_ATOF      0
#  define SQLITE_WCE_FREE_HACK         0
#  define SQLITE_WCE_STRCMP_HACK       0
#endif

#if defined(_WIN32_WCE)
#  define FILENAME_MAX 256
#  define SQLITE_WCE_USE_OWN_GETENV		1
#else
#  define SQLITE_WCE_USE_OWN_GETENV		0
#endif

/*
** WinCE 4.x implements localtime, but appears it's only for MFC
** applications. To avoid to include the "wce_port" dir to this
** versions, I declare it here to use my implementation, only.
** Earlier versions MUST add the "wce_port" to the include list,
** bacuse the 'tm' structure needs to be defined, also.
*/
#if defined(_WIN32_WCE) && _WIN32_WCE >= 400
#  include <time.h>
#  define localtime	sqlitewce_localtime
struct tm * sqlitewce_localtime( const time_t *timer );
#endif

/*
 * In WinCE versions prior than 3.0 no file locking is performed.
 * If you are developing on this platforms, make sure to only use
 * SQLite on a single threaded project, and that only a process at a
 * time is accessing the database.
 *
 * You can also define this if you don't want any file locking.
 * This will speed up things a litle, but can only be used if you are
 * sure it will never be two processes accessing the same database at
 * the same time.
 */
#if defined(_WIN32_WCE) && _WIN32_WCE < 300 && !defined(SQLITE_WCE_OMIT_FILELOCK)
#  define SQLITE_WCE_OMIT_FILELOCK		1
#endif


/*
 * No ANSI equivalents to isXXX routines.
 * Use the UNICODE version.
 */
#if SQLITE_WCE_USE_OWN_ISXXXX
#  define isalpha		iswalpha
#  define isalnum		iswalnum
#  define isdigit		iswdigit
#  define isspace		iswspace
#  define isprint		iswprint
#  define islower		iswlower
#  define isupper		iswupper
#endif


/*
 * Quick hack for free() -- function attributes error in tokenize.c
 */
#if SQLITE_WCE_FREE_HACK
__inline void sqlitewce_free( void *p )	{ free(p); }
#  define free			sqlitewce_free
#endif

/*
 * Quick hack for strcmp() -- function attributes error in tokenize.c
 */
#if SQLITE_WCE_STRCMP_HACK
__inline int sqlitewce_strcmp( const char* s1, const char* s2 )	{ return strcmp(s1,s2); }
#  define strcmp		sqlitewce_strcmp
#endif


/*
 * No atof(), use our own
 */
#if SQLITE_WCE_USE_OWN_ATOF
double sqlitewce_atof( const char* s );
#  define atof			sqlitewce_atof
#endif


/*
 * No getenv(), simulate that no variables are defined
 */
#if SQLITE_WCE_USE_OWN_GETENV
#  define getenv(x)		NULL
#endif

