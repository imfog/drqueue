/* $Id: computer_info.c,v 1.8 2001/08/31 09:28:10 jorge Exp $ */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#ifdef __IRIX
#include <sys/sysmp.h>
#endif

#include "constants.h"
#include "computer_info.h"

#ifdef __LINUX			/* __LINUX */

void get_hwinfo (struct computer_hwinfo *hwinfo)
{
  if (gethostname (hwinfo->name,MAXNAMELEN-1) == -1) {
    perror ("get_hwinfo: gethostname");
    kill(0,SIGINT);
  }
  hwinfo->arch = ARCH_INTEL;
  hwinfo->os = OS_LINUX;
  hwinfo->proctype = get_proctype();
  hwinfo->procspeed = get_procspeed();
  hwinfo->numproc = get_numproc();
  hwinfo->speedindex = get_speedindex (hwinfo);
}

t_proctype get_proctype (void)
{
  t_proctype proctype = PROCTYPE_UNKNOWN;
  FILE *cpuinfo;
  char buf[BUFFERLEN];
  int found = 0;

  if ((cpuinfo = fopen("/proc/cpuinfo","r")) == NULL) {
    perror ("get_proctype: fopen");
    kill (0,SIGINT);
  }

  while (!(found || feof (cpuinfo))) {
    fgets (buf,BUFFERLEN-1,cpuinfo);
    if (strstr(buf,"model name") != NULL) {
      if (strstr(buf,"Pentium(R) 4") != NULL) {
	proctype = PROCTYPE_PENTIUM4;
      } else if (strstr(buf,"Pentium III") != NULL) {
	proctype = PROCTYPE_PENTIUMIII;
      } else if (strstr(buf,"Pentium II") != NULL) {
	proctype = PROCTYPE_PENTIUMII;
      } else if (strstr(buf,"Pentium ") != NULL) {
	proctype = PROCTYPE_PENTIUM;
      }
      found = 1;
    }
  }

  if (!found) {
    fprintf (stderr,"ERROR: Proc type not found on /proc/cpuinfo\n");
    kill(0,SIGINT);
  }

  fclose (cpuinfo);

  return proctype;
}

int get_procspeed (void)
{
  FILE *cpuinfo;
  int procspeed = 1;
  char buf[BUFFERLEN];
  float st;			/* speed temp */
  int found = 0;
  int index = 0;
  
  if ((cpuinfo = fopen("/proc/cpuinfo","r")) == NULL) {
    perror ("get_procspeed: fopen");
    kill (0,SIGINT);
  }

  while (!(found || feof (cpuinfo))) {
    fgets (buf,BUFFERLEN-1,cpuinfo);
    if (strstr(buf,"cpu MHz") != NULL) {
      while (!isdigit(buf[index]))
	index++;
      sscanf (&buf[index],"%f\n",&st);
      procspeed = (int) st;
      found = 1;
    }
  }

  if (!found) {
    fprintf (stderr,"ERROR: Proc speed not found on /proc/cpuinfo\n");
    kill (0,SIGINT);
  }

  fclose (cpuinfo);

  return procspeed;
}

int get_numproc (void)
{
  FILE *cpuinfo;
  int numproc = 0;
  char buf[BUFFERLEN];
  
  if ((cpuinfo = fopen("/proc/cpuinfo","r")) == NULL) {
    perror ("get_numproc: fopen");
    kill (0,SIGINT);
  }

  while (!feof (cpuinfo)) {
    fgets (buf,BUFFERLEN-1,cpuinfo);
    if (strstr(buf,"processor") != NULL) {
      numproc++;
    }
  }

  fclose (cpuinfo);

  return numproc;
}

#else 
# ifdef __IRIX
void get_hwinfo (struct computer_hwinfo *hwinfo)
{
  if (gethostname (hwinfo->name,MAXNAMELEN-1) == -1) {
    perror ("get_hwinfo: gethostname");
    kill(0,SIGINT);
  }
  hwinfo->arch = ARCH_MIPS;
  hwinfo->os = OS_IRIX;
  hwinfo->proctype = get_proctype();
  hwinfo->procspeed = get_procspeed();
  hwinfo->numproc = get_numproc();
  hwinfo->speedindex = get_speedindex (hwinfo);
}

t_proctype get_proctype (void)
{
  FILE *hinv;
  char buf[BUFFERLEN];
  t_proctype proctype = PROCTYPE_UNKNOWN;
  int found = 0;

  if ((hinv = popen ("/sbin/hinv","r")) == NULL) {
    fprintf (stderr,"Warning: Problems executing '/sbin/hinv'\n");
    return proctype;
  }

  while (fgets (buf,BUFFERLEN,hinv) != NULL) {
    if (strstr(buf,"CPU:") != NULL) {
      if (strstr(buf,"R5000") != NULL) {
	proctype = PROCTYPE_MIPSR5000;
	found = 1;
      } else if (strstr(buf,"R10000") != NULL) {
	proctype = PROCTYPE_MIPSR10000;
	found = 1;
      }
    }
  }

  pclose (hinv);

  if (!found) {
    fprintf (stderr,"Warning processor type not listed or couldn't be found\n");
  }
  
  return proctype;
}

int get_procspeed (void)
{
  FILE *hinv;
  int procspeed = 1;
  char buf[BUFFERLEN];
  int nprocs;			/* number of cpus */
  int found = 0;

  if ((hinv = popen ("/sbin/hinv","r")) == NULL) {
    fprintf (stderr,"Warning: Problems executing '/sbin/hinv'\n");
    return procspeed;
  }

  while (fgets (buf,BUFFERLEN,hinv) != NULL) {
    if (strstr(buf,"MHZ") != NULL) {
      /* The MHz are the second number on this line */
      if (sscanf (buf,"%i %i",&nprocs,&procspeed) == 2) {
	found = 1;
      } else if (sscanf (buf,"Processor %i:  %i",&nprocs,&procspeed) == 2) {
	found = 1;
      }
    }
  }

  if (!found) {
    fprintf (stderr,"ERROR: Proc speed not found on\n");
  }

  pclose (hinv);

  return procspeed;
}

int get_numproc (void)
{
  return sysmp (MP_NPROCS);
}
# else 
#  error You need to define the OS, or OS defined not supported
# endif
#endif

int get_speedindex (struct computer_hwinfo *hwinfo)
{
  int speedindex;

  speedindex = (hwinfo->proctype + 1) * hwinfo->procspeed;

  return speedindex;
}


void report_hwinfo (struct computer_hwinfo *hwinfo)
{
  printf ("HWINFO Report\n");
  printf ("Name:\t\t\t%s\n",hwinfo->name);
  printf ("Architecture:\t\t%s\n",archstring(hwinfo->arch));
  printf ("OS:\t\t\t%s\n",osstring(hwinfo->os));
  printf ("Processor type:\t\t%s\n",proctypestring(hwinfo->proctype));
  printf ("Processor speed:\t%i MHz\n",hwinfo->procspeed);
  printf ("Number of processors:\t%i\n",hwinfo->numproc);
  printf ("Speed index:\t\t%i\n",hwinfo->speedindex);
}

char *osstring (t_os os)
{
  static char osstring[BUFFERLEN];

  osstring[MAXCMDLEN-1] = 0;
  switch (os) {
  case OS_UNKNOWN:
    strncpy (osstring,"UNKNOWN",BUFFERLEN-1);
    break;
  case OS_IRIX:
    strncpy (osstring,"Irix",BUFFERLEN-1);
    break;
  case OS_LINUX:
    strncpy (osstring,"Linux",BUFFERLEN-1);
    break;
  case OS_WINDOWS:
    strncpy (osstring,"Windows",BUFFERLEN-1);
    break;
  default:
    strncpy (osstring,"DEFAULT (?!)",BUFFERLEN-1);
    fprintf (stderr,"os == DEFAULT\n");
  }

  return osstring;
}

char *archstring (t_arch arch)
{
  static char archstring[BUFFERLEN];

  archstring[BUFFERLEN-1] = 0;
  switch (arch) {
  case ARCH_UNKNOWN:
    strncpy (archstring,"UNKNOWN",BUFFERLEN-1);
    break;
  case ARCH_INTEL:
    strncpy (archstring,"Intel (Little Endian)",BUFFERLEN-1);
    break;
  case ARCH_MIPS:
    strncpy (archstring,"Mips (Big Endian)",BUFFERLEN-1);
    break;
  default:
    strncpy (archstring,"DEFAULT (?!)",BUFFERLEN-1);
    fprintf (stderr,"arch == DEFAULT\n");
  }

  return archstring;
}

char *proctypestring (t_proctype proctype)
{
  static char proctypestring[BUFFERLEN];

  proctypestring[BUFFERLEN-1] = 0;
  switch (proctype) {
  case PROCTYPE_UNKNOWN:
    strncpy (proctypestring,"UNKNOWN",BUFFERLEN-1);
    break;
  case PROCTYPE_PENTIUM:
    strncpy (proctypestring,"Pentium",BUFFERLEN-1);
    break;
  case PROCTYPE_PENTIUMII:
    strncpy (proctypestring,"Pentium II",BUFFERLEN-1);
    break;
  case PROCTYPE_PENTIUMIII:
    strncpy (proctypestring,"Pentium III",BUFFERLEN-1);
    break;
  case PROCTYPE_PENTIUM4:
    strncpy (proctypestring,"Pentium 4",BUFFERLEN-1);
    break;
  case PROCTYPE_MIPSR5000:
    strncpy (proctypestring,"R5000",BUFFERLEN-1);
    break;
  case PROCTYPE_MIPSR10000:
    strncpy (proctypestring,"R10000",BUFFERLEN-1);
    break;
  default:
    strncpy (proctypestring,"DEFAULT (?!)",BUFFERLEN-1);
    fprintf (stderr,"proctype == DEFAULT\n");
  }

  return proctypestring;
}

