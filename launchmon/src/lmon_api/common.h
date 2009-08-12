/*
 * $Header: $
 *--------------------------------------------------------------------------------
 * Copyright (c) 2008, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
 * LLNL-CODE-409469. All rights reserved.
 *
 * This file is part of LaunchMON. For details, see
 * https://computing.llnl.gov/?set=resources&page=os_projects
 *
 * Please also read LICENSE.txt -- Our Notice and GNU Lesser General Public License.
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License (as published by the Free Software
 * Foundation) version 2.1 dated February 1999.
 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 *--------------------------------------------------------------------------------
 *
 *
 *  Update Log:
 *              Aug 10 2009 DHA: Added HAVE_STDIO_H and HAVE_SYS_TYPES_H check
 *              Jun 06 2008 DHA: File created. Mostly from the autobook.
 *
 */

#ifndef LMON_API_COMMON_H
#define LMON_API_COMMON_H 1

#ifdef __cplusplus
# define BEGIN_C_DECLS       extern "C" {
# define END_C_DECLS         }
#else /* !__cplusplus */
# define BEGIN_C_DECLS
# define END_C_DECLS
#endif /* __cplusplus */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#else
# error stdio.h is required
#endif 

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#else
# error sys/types.h is required
#endif

#if STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#elif HAVE_STRINGS_H
# include <strings.h>
#endif /*STDC_HEADERS*/

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_ERRNO_H
# include <errno.h>
#endif /*HAVE_ERRNO_H*/

#ifndef errno
extern int errno;
#endif

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1
#endif

#endif /* LMON_API_COMMON_H */
