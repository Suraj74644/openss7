/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* ryinitiator.c - generic interactive initiator */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/imisc/RCS/ryinitiator.c,v 9.0 1992/06/16 12:23:43 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/imisc/RCS/ryinitiator.c,v 9.0 1992/06/16 12:23:43 isode Rel
 *
 *
 * Log: ryinitiator.c,v
 * Revision 9.0  1992/06/16  12:23:43  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <varargs.h>
#include "ryinitiator.h"

#undef	TIMER
#undef	TMS
#ifdef	BSD42
#define	TIMER
#endif
#ifdef	SYS5
#define	TIMER
#ifndef	HPUX
#include <sys/times.h>
#define	TMS
#endif
#endif

/*    DATA */

static int count = 1;
int length = 536;

#ifdef	TIMER
#define	DS_RESULT(ds)	(timing ? timing_result : (ds) -> ds_result)

static int timing = 0;

static int timing_result();
#else
#define	DS_RESULT(ds)	((ds) -> ds_result)
#endif

static char *myname = "ryinitiator";

static int getline();
static void invoke();

#ifdef	TIMER
static void timer();

#ifndef	TMS
static void tvsub();
#endif
#endif

extern char *isodeversion;

/*    INITIATOR */

/* ARGSUSED */

ryinitiator(argc, argv, myservice, mycontext, mypci, ops, dispatches, quit)
	int argc;
	char **argv, *myservice, *mycontext, *mypci;
	struct RyOperation ops[];
	struct dispatch *dispatches;
	IFP quit;
{
	int iloop, sd;
	register char *cp, **ap;
	char buffer[BUFSIZ], *vec[NVEC + 1];
	register struct dispatch *ds;
	struct QOStype qos;
	struct SSAPref sfs;
	register struct SSAPref *sf;
	register struct PSAPaddr *pa;
	struct AcSAPconnect accs;
	register struct AcSAPconnect *acc = &accs;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;
	AEI aei;
	OID ctx, pci;
	struct PSAPctxlist pcs;
	register struct PSAPctxlist *pc = &pcs;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;

	if (myname = rindex(argv[0], '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = argv[0];

	isodetailor(myname, 1);

	qos.qos_reliability = HIGH_QUALITY;

	for (ap = argv + 1; cp = *ap; ap++) {
		if (*cp != '-')
			break;

		if (strcmp(cp, "-low") == 0) {
			qos.qos_reliability = LOW_QUALITY;
			continue;
		}
		if (strcmp(cp, "-high") == 0) {
			qos.qos_reliability = HIGH_QUALITY;
			continue;
		}
		if (strcmp(cp, "-c") == 0) {
			if ((cp = *++ap) == NULL || sscanf(cp, "%d", &count) != 1 || count < 1)
				adios(NULLCP, "usage: %s -c count", myname);
#ifdef	TIMER
			timing++;
#endif
			continue;
		}
		if (strcmp(cp, "-l") == 0) {
			if ((cp = *++ap) == NULL || sscanf(cp, "%d", &length) != 1 || length < 0)
				adios(NULLCP, "usage: %s -l length", myname);
			continue;
		}

		adios(NULLCP, "%s: unknown switch", cp);
	}

	if ((cp = *ap++) == NULL)
		adios(NULLCP, "usage: %s host [operation [ arguments ... ]]", myname);

	if ((aei = _str2aei(cp, myservice, mycontext, *ap == NULL, NULLCP, NULLCP)) == NULLAEI)
		adios(NULLCP, "unable to resolve service: %s", PY_pepy);
	if ((pa = aei2addr(aei)) == NULLPA)
		adios(NULLCP, "address translation failed");

	if ((ctx = ode2oid(mycontext)) == NULLOID)
		adios(NULLCP, "%s: unknown object descriptor", mycontext);
	if ((ctx = oid_cpy(ctx)) == NULLOID)
		adios(NULLCP, "out of memory");
	if ((pci = ode2oid(mypci)) == NULLOID)
		adios(NULLCP, "%s: unknown object descriptor", mypci);
	if ((pci = oid_cpy(pci)) == NULLOID)
		adios(NULLCP, "out of memory");
	pc->pc_nctx = 1;
	pc->pc_ctx[0].pc_id = 1;
	pc->pc_ctx[0].pc_asn = pci;
	pc->pc_ctx[0].pc_atn = NULLOID;

	if ((sf = addr2ref(PLocalHostName())) == NULL) {
		sf = &sfs;
		(void) bzero((char *) sf, sizeof *sf);
	}

	if (*ap == NULL) {
		(void) printf("%s", myname);
		if (sf->sr_ulen > 2)
			(void) printf(" running on host %s", sf->sr_udata + 2);
		if (sf->sr_clen > 2)
			(void) printf(" at %s", sf->sr_cdata + 2);
		(void) printf(" [%s, ", oid2ode(ctx));
		(void) printf("%s]\n", oid2ode(pci));
		(void) printf("using %s\n", isodeversion);

		(void) printf("%s... ", cp);
		(void) fflush(stdout);

		iloop = 1;
	} else {
		cp = *ap++;
		for (ds = dispatches; ds->ds_name; ds++)
			if (strcmp(ds->ds_name, cp) == 0)
				break;
		if (ds->ds_name == NULL)
			adios(NULLCP, "unknown operation \"%s\"", cp);

		iloop = 0;
	}

	if (AcAssocRequest(ctx, NULLAEI, aei, NULLPA, pa, pc, NULLOID,
			   0, ROS_MYREQUIRE, SERIAL_NONE, 0, sf, NULLPEP, 0, &qos, acc, aci)
	    == NOTOK)
		acs_adios(aca, "A-ASSOCIATE.REQUEST");

	if (acc->acc_result != ACS_ACCEPT) {
		if (iloop)
			(void) printf("failed\n");

		adios(NULLCP, "association rejected: [%s]", AcErrString(acc->acc_result));
	}
	if (iloop) {
		(void) printf("connected\n");
		(void) fflush(stdout);
	}

	sd = acc->acc_sd;
	ACCFREE(acc);

	if (RoSetService(sd, RoPService, roi) == NOTOK)
		ros_adios(rop, "set RO/PS fails");

	if (iloop) {
		for (;;) {
			if (getline(buffer) == NOTOK)
				break;

			if (str2vec(buffer, vec) < 1)
				continue;

			for (ds = dispatches; ds->ds_name; ds++)
				if (strcmp(ds->ds_name, vec[0]) == 0)
					break;
			if (ds->ds_name == NULL) {
				advise(NULLCP, "unknown operation \"%s\"", vec[0]);
				continue;
			}

			invoke(sd, ops, ds, vec + 1);
		}
	} else
		invoke(sd, ops, ds, ap);

	(*quit) (sd, (struct dispatch *) NULL, (char **) NULL, (caddr_t *) NULL);
}

/*    INVOKE */

static void
invoke(sd, ops, ds, args)
	int sd;
	struct RyOperation ops[];
	register struct dispatch *ds;
	char **args;
{
	register int i;
	int cc, result;
	caddr_t in;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;
	register struct RoSAPpreject *rop = &roi->roi_preject;

	in = NULL;
	if (ds->ds_argument && (*ds->ds_argument) (sd, ds, args, &in) == NOTOK)
		return;

#ifdef	TIMER
	if (timing) {
		register struct RyOperation *ryo = ops;
		PE pe;

		cc = 0;

		for (; ryo->ryo_name; ryo++)
			if (ryo->ryo_op == ds->ds_operation)
				break;
		if (!ryo->ryo_name || !ryo->ryo_arg_mod)
			goto nope;

		pe = NULLPE;
		if (enc_f(ryo->ryo_arg_index, ryo->ryo_arg_mod, &pe, 1, NULL, NULLCP, in) != NOTOK)
			cc = ps_get_abs(pe);
		if (pe)
			pe_free(pe);

	      nope:;
		timer(0, 0);
	}
#endif

	for (i = 0; i < count; i++)
		switch (result = RyStub(sd, ops, ds->ds_operation, RyGenID(sd),
					NULLIP, in, DS_RESULT(ds), ds->ds_error, ROS_SYNC, roi)) {
		case NOTOK:	/* failure */
			if (ROS_FATAL(rop->rop_reason))
				ros_adios(rop, "STUB");
			ros_advise(rop, "STUB");
			goto out;

		case OK:	/* got a result/error response */
			break;

		case DONE:	/* got RO-END? */
			adios(NULLCP, "got RO-END.INDICATION");
			/* NOTREACHED */

		default:
			adios(NULLCP, "unknown return from RyStub=%d", result);
			/* NOTREACHED */
		}

#ifdef	TIMER
	if (timing)
		timer(cc, count);
#endif

      out:;
	if (ds->ds_fr_mod && in)
		(void) fre_obj(in, ds->ds_fr_mod->md_dtab[ds->ds_fr_index], ds->ds_fr_mod, 1);
}

/*    INTERACTIVE */

static int
getline(buffer)
	char *buffer;
{
	register int i;
	register char *cp, *ep;
	static int sticky = 0;

	if (sticky) {
		sticky = 0;
		return NOTOK;
	}

	(void) printf("%s> ", myname);
	(void) fflush(stdout);

	for (ep = (cp = buffer) + BUFSIZ - 1; (i = getchar()) != '\n';) {
		if (i == EOF) {
			(void) printf("\n");
			clearerr(stdin);
			if (cp != buffer) {
				sticky++;
				break;
			}

			return NOTOK;
		}

		if (cp < ep)
			*cp++ = i;
	}
	*cp = NULL;

	return OK;
}

/*    TIMER */

#ifdef	TIMER

#ifndef	NBBY
#define	NBBY	8
#endif

#ifndef	TMS
static void
timer(bytes, pkts)
	int bytes, pkts;
{
	long ms;
	float bs, ps;
	struct timeval stop, td;
	static struct timeval start;

	if (pkts == 0) {
		(void) gettimeofday(&start, (struct timezone *) 0);
		return;
	} else
		(void) gettimeofday(&stop, (struct timezone *) 0);

	tvsub(&td, &stop, &start);
	ms = (td.tv_sec * 1000) + (td.tv_usec / 1000);
	bs = (((float) bytes * pkts * NBBY * 1000) / (float) (ms ? ms : 1)) / NBBY;
	ps = ((float) pkts * 1000) / (float) (ms ? ms : 1);

	(void) printf("%d operations in %d.%02d seconds (%.2f ops/s)",
		      pkts, td.tv_sec, td.tv_usec / 10000, ps);
	if (bytes > 0)
		(void) printf("; %d bytes/op for %.2f Kbytes/s", bytes, bs / 1024);
	(void) printf("\n");
}

static void
tvsub(tdiff, t1, t0)
	register struct timeval *tdiff, *t1, *t0;
{

	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	if (tdiff->tv_usec < 0)
		tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}
#else
long times();

static void
timer(bytes, pkts)
	int bytes, pkts;
{
	long ms;
	float bs, ps;
	long stop, td, secs, msecs;
	struct tms tm;
	static long start;

	if (pkts == 0) {
		start = times(&tm);
		return;
	} else
		stop = times(&tm);

	td = stop - start;
	secs = td / 60, msecs = (td % 60) * 1000 / 60;
	ms = (secs * 1000) + msecs;
	bs = (((float) bytes * pkts * NBBY * 1000) / (float) (ms ? ms : 1)) / NBBY;
	ps = ((float) pkts * 1000) / (float) (ms ? ms : 1);

	(void) printf("%d operations in %d.%02d seconds (%.2f ops/s)", pkts, secs, msecs / 10, ps);
	if (bytes > 0)
		(void) printf("; %d bytes/op for %.2f Kbytes/s", bytes, bs / 1024);
	(void) printf("\n");
}
#endif
#endif

/*  */

/* ARGSUSED */

static int
timing_result(sd, id, dummy, result, roi)
	int sd, id, dummy;
	caddr_t result;
	struct RoSAPindication *roi;
{
	return OK;
}

/*    ERRORS */

void
ros_adios(rop, event)
	register struct RoSAPpreject *rop;
	char *event;
{
	ros_advise(rop, event);

	_exit(1);
}

void
ros_advise(rop, event)
	register struct RoSAPpreject *rop;
	char *event;
{
	char buffer[BUFSIZ];

	if (rop->rop_cc > 0)
		(void) sprintf(buffer, "[%s] %*.*s", RoErrString(rop->rop_reason),
			       rop->rop_cc, rop->rop_cc, rop->rop_data);
	else
		(void) sprintf(buffer, "[%s]", RoErrString(rop->rop_reason));

	advise(NULLCP, "%s: %s", event, buffer);
}

/*  */

void
acs_adios(aca, event)
	register struct AcSAPabort *aca;
	char *event;
{
	acs_advise(aca, event);

	_exit(1);
}

void
acs_advise(aca, event)
	register struct AcSAPabort *aca;
	char *event;
{
	char buffer[BUFSIZ];

	if (aca->aca_cc > 0)
		(void) sprintf(buffer, "[%s] %*.*s",
			       AcErrString(aca->aca_reason),
			       aca->aca_cc, aca->aca_cc, aca->aca_data);
	else
		(void) sprintf(buffer, "[%s]", AcErrString(aca->aca_reason));

	advise(NULLCP, "%s: %s (source %d)", event, buffer, aca->aca_source);
}

/*  */

#ifndef	lint
static void _advise();

void
adios(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);

	_exit(1);
}
#else
/* VARARGS */

void
adios(what, fmt)
	char *what, *fmt;
{
	adios(what, fmt);
}
#endif

#ifndef	lint
void
advise(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);
}

static void
_advise(ap)
	va_list ap;
{
	char buffer[BUFSIZ];

    xsprintf (buffer, ap);

	(void) fflush(stdout);

	(void) fprintf(stderr, "%s: ", myname);
	(void) fputs(buffer, stderr);
	(void) fputc('\n', stderr);

	(void) fflush(stderr);
}
#else
/* VARARGS */

void
advise(what, fmt)
	char *what, *fmt;
{
	advise(what, fmt);
}
#endif

#ifndef	lint
void
ryr_advise(va_alist)
	va_dcl
{
	va_list ap;

	va_start(ap);

	_advise(ap);

	va_end(ap);
}
#else
/* VARARGS */

void
ryr_advise(what, fmt)
	char *what, *fmt;
{
	ryr_advise(what, fmt);
}
#endif