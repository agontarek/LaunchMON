# $Header: $
#
# x_ac_testnnodes.m4
#
# --------------------------------------------------------------------------------
# Copyright (c) 2008, Lawrence Livermore National Security, LLC. Produced at
# the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
# LLNL-CODE-409469 All rights reserved.
# 
#  This file is part of LaunchMON. For details, see
#  https://computing.llnl.gov/?set=resources&page=os_projects
# 
#  Please also read LICENSE -- Our Notice and GNU Lesser General Public License.
# 
# 
#  This program is free software; you can redistribute it and/or modify it under the
#  terms of the GNU General Public License (as published by the Free Software
#  Foundation) version 2.1 dated February 1999.
# 
#  This program is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU
#  General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc., 59 Temple
#  Place, Suite 330, Boston, MA 02111-1307 USA
# --------------------------------------------------------------------------------
#
#   Update Log:
#         Aug 03 2010 DHA: Added middleware hostlist support
#         Dec 15 2008 DHA: File created.
#

AC_DEFUN([X_AC_TESTNNODES], [
  AC_MSG_CHECKING([the number of compute nodes that standard test cases should use])
  AC_ARG_WITH([testnnodes],
    AS_HELP_STRING(--with-testnnodes@<:@=NNodes@:>@,specify the number of compute nodes test cases should use @<:@BGL Note: use the number of IO nodes instead@:>@ @<:@default=2@:>@), 
    [with_tnn=$withval],
    [with_tnn="check"])

  tnn_given="no"
  tnn=2

  if test "x$with_tnn" != "xno" -a "x$with_tnn" != "xcheck"; then
      tnn_given="yes"
      tnn=$with_tnn
  fi 

  AC_SUBST(TESTNNODES, $tnn)	
  AC_MSG_RESULT($tnn)
])

AC_DEFUN([X_AC_NCORE_SMP], [
  AC_MSG_CHECKING([the number of cores per SMP node @<:@BGL: the number of compute nodes per IO node@:>@])
  AC_ARG_WITH([ncore-per-CN],
    AS_HELP_STRING(--with-ncore-per-CN@<:@=NCores@:>@,specify the core-count per compute node @<:@BGL Note: use the number of compute nodes per IO node instead@:>@ @<:@default=NCore of the configure host@:>@),
    [with_smp=$withval],
    [with_smp="check"])

  ncoresmp=1

  if test "x$with_smp" != "xno" -a "x$with_smp" != "xcheck"; then
      ncoresmp=$with_smp
  else
      if test x$ac_target_os = "xlinux"; then
         ncoresmp=`cat /proc/cpuinfo | grep processor | wc -l`
      else
         AC_MSG_ERROR([how to get the ncores of this configure system not known])
      fi
  fi

  AC_SUBST(SMPFACTOR,$ncoresmp)
  AC_MSG_RESULT($ncoresmp)
])

AC_DEFUN([X_AC_MW_HOSTLIST], [
  AC_MSG_CHECKING([a set of hosts that middleware testing should use])
  AC_ARG_WITH([mw-hostlist],
    AS_HELP_STRING(--with-mw-hostlist@<:@=host1:host2@:>@,specify the list of hosts that middelware testing should use),
    [with_hostlist=$withval],
    [with_hostlist="check"])

  hostlist=""

  if test "x$with_hostlist" != "xno" -a "x$with_hostlist" != "xcheck"; then
     hostlist=$with_hostlist
  else
      hostlist=`hostname`
  fi

  #
  # Substituting @LMONHL@ to the hostlist 
  #
  AC_SUBST(LMONHL,$hostlist)
  AC_MSG_RESULT($hostlist)
])

