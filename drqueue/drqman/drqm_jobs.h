/*
 * $Header: /root/cvs/drqueue/drqman/drqm_jobs.h,v 1.30 2003/12/15 22:18:32 jorge Exp $
 */

#ifndef _DRQM_JOBS_H_
#define _DRQM_JOBS_H_

#include <gtk/gtk.h>
#include "libdrqueue.h"

struct drqmj_sesframes {
  GtkWidget *eframe_start;
  GtkWidget *eframe_end;
  GtkWidget *eframe_step;
};

struct drqmj_limits {
  GtkWidget *enmaxcpus;		/* Entries */
  GtkWidget *enmaxcpuscomputer;
  GtkWidget *lnmaxcpus;		/* Labels*/
  GtkWidget *lnmaxcpuscomputer;
  GtkWidget *cb_irix;		/* Check buttons */
  GtkWidget *cb_linux;
};

struct drqmj_flags {
  GtkWidget *cbmailnotify;	/* Check button */
  GtkWidget *cbdifemail;	/* Specific email for mail notification ? */
  GtkWidget *edifemail;		/* Entry for the email */
};

struct drqmj_koji_maya {
  GtkWidget *escene;
  GtkWidget *fsscene;		/* File selector for the scene */
  GtkWidget *erenderdir;
  GtkWidget *fsrenderdir;	/* File selector for the output directory */
  GtkWidget *eimage;
  GtkWidget *eviewcmd;
  GtkWidget *escript;		/* Entry script location */
  GtkWidget *fsscript;		/* File selectot for the script directory */
  GtkWidget *efile_owner;	/* Owner of the rendered files */
};

struct drqmj_koji_blender {
  GtkWidget *escene;
  GtkWidget *fsscene;						/* File selector for the scene */
  GtkWidget *eviewcmd;
  GtkWidget *escript;						/* Entry script location */
  GtkWidget *fsscript;		/* File selectot for the script directory */
};

struct drqmj_dnji {		/* dialog new job */
  GtkWidget *dialog;
  GtkWidget *vbox;
  GtkWidget *ename;
  GtkWidget *ecmd;
  GtkWidget *esf,*eef,*estf;	/* entry start frame, entry end frame, entry step frames */
  GtkWidget *cpri,*epri;	/* combo priority, entry priority */
  GtkWidget *fs;		/* File selector */

  /* koj */
  GtkWidget *ckoj;		/* combo koj */
  GtkWidget *fkoj;		/* frame koj */
  uint16_t koj;			/* koj */
  struct drqmj_koji_maya koji_maya; /* koj info for maya */
	struct drqmj_koji_blender koji_blender; /* koj info for blender */
  struct drqmj_limits limits;	/* limits info */
  struct drqmj_flags flags;	/* flags info */
};

struct drqmj_jddi {		/* job details dialog */
  GtkWidget *dialog;		/* Main dialog */
  GtkWidget *lname;		/* label name */
  GtkWidget *lowner;		/* label owner */
  GtkWidget *lstatus;		/* label status */
  GtkWidget *lcmd;		/* label command */
  GtkWidget *lstartend;		/* start and end frames */
  GtkWidget *lpri;		/* priority */
  GtkWidget *cpri;		/* Combo */
  GtkWidget *epri;		/* entry priority when changing */
  GtkWidget *lfrldf;		/* frames left, done and failed */
  GtkWidget *lavgt;		/* average time per frame */
  GtkWidget *lestf;		/* estimated finish time */
  GtkWidget *clist;		/* frame info clist */

  GtkWidget *menu;		/* Popup menu */
  gint row,column;		/* selected frame */
  int selected;			/* if a frame is selected */
  struct job job;		/* The struct of the selected job */
  struct drqmj_limits limits;	/* Limits info */
  struct drqmj_sesframes sesframes; /* Info about start, end, step frames */
};

struct drqm_jobs_info {
  GtkWidget *clist;		/* main clist */
  gint row, column;		/* selected job */
  GtkWidget *menu;		/* Popup menu */
  int selected;			/* if a job is selected */
  int ijob;			/* index to the selected job */
  uint32_t njobs;		/* Number of jobs in the list */
  struct job *jobs;		/* The job list */
  struct drqmj_dnji dnj;	/* dialog new job */
  struct drqmj_jddi jdd;	/* job details dialog */
};

struct info_drqm;

void CreateJobsPage (GtkWidget *notebook, struct info_drqm *info);
void drqm_update_joblist (struct drqm_jobs_info *info);

#endif /* _DRQM_JOBS_H */







